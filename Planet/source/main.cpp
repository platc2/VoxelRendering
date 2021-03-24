#ifdef _MSC_VER
	#include <ciso646>
#endif

// Stdlib
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <exception>
#include <functional>
// Renderer
#include "OpenGLRenderer.h"
#include "RenderPass.h"
#include "Buffer.h"
#include "Window.h"
// Voxel
#include "ArrayHelpers.h"
#include "FileUtilities.h"
#include "Trackball.h"
// Other
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace voxel;


struct ApplicationSettings
{
	/** Window width */
	static constexpr std::uint32_t k_uiWindowWidth = 500;
	/** Window height */
	static constexpr std::uint32_t k_uiWindowHeight = 500;
	/** Window title */
	static constexpr char* const k_szWindowTitle = "Planet";
	/** Whether or not the window is resizable */
	static constexpr bool k_bResizableWindow = true;
	/** Major OpenGL version */
	static const std::uint32_t k_uiMajorOpenGLVersion = 4;
	/** Minor OpenGL version */
	static const std::uint32_t k_uiMinorOpenGLVersion = 5;
};


/**
 * Application class
 */
class Application final
{
private:
	/**
	 * GLFW window handle
	 */
	GLFWwindow* m_pWindowHandle;

	/**
	 * Renderer object
	 */
	COpenGLRenderer m_xRenderer;

	/**
	 * Window handle
	 */
	Window m_xWindow;

	/**
	 * Vertex buffer containing the vertex data
	 */
	Buffer* m_pVertexBuffer;

	Buffer* m_pIndexBuffer;

	std::uint32_t m_uiIndexCount;

	/**
	 * Main render pass
	 */
	RenderPass* m_pRenderPass;

	void* m_pMatrixMemory;
	void* m_pTessellationMemory;
	void* m_pLightMemory;

public:
	/**
	 * Default ctor
	 */
	Application() noexcept;

	/**
	 * Frees all allocated memory
	 */
	~Application() noexcept;

	/**
	 * Initializes the application and all its dependencies, e.g. render objects.
	 */
	void initialize();

	/**
	 * Launches the application and keeps it running until it is terminated.
	 */
	void launch();

private:
	/**
	 * Initializes render data, e.g. buffers, render passes, ...
	 */
	void initializeRenderData();

	/**
	 * Renders the data to the screen
	 */
	void render();

	/**
	 * Callback for when the window size changed
	 *
	 * @param dWidth   new width
	 * @param dHeight  new height
	 */
	void sizeChanged(const double dWidth, const double dHeight);

	/**
	 * Callback for when the mouse was moved
	 *
	 * @param dDeltaX  delta x
	 * @param dDeltaY  delta y
	 */
	void mouseMoved(const double dDeltaX, const double dDeltaY);

	/**
	 * Callback for when the state of a mouse button changed
	 *
	 * @param eButton  button which changes
	 * @param eAction  new state of the button
	 */
	void mousePressed(const Window::EMouseButton eButton, const Window::EAction eAction);

	/**
	* Throws a given exception.
	 *
	 * @param xException  exception to throw
	 * @throws std::exception
	 */
	static void error(const std::exception& xException);

	/**
	 * Throws an std::runtime_error exception given an error description.
	 *
	 * @param strDescription  description of the error
	 * @throws std::runtime_error
	 */
	static void error(const std::string& strDescription);
};


int main() noexcept
{
	try
	{
		// Create, initialize and launch the application
		Application xApplication;
			xApplication.initialize();
			xApplication.launch();
		return 0;
	}
	// global exception handling
	catch(std::exception& xException)
	{
		std::cerr << "==================================================\n"
		          << "std::exception thrown\n"
		          << "==================================================\n"
		          << xException.what() << std::endl;
		return -1;
	}
}


/**
 * Default ctor
 */
Application::Application() noexcept :
	m_xWindow(&m_xRenderer, ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight, Window::Callbacks{
		{ std::bind(&Application::render, this) },
		{ std::bind(&Application::sizeChanged, this, std::placeholders::_1, std::placeholders::_2) },
		{},
		{}})
{
}


/**
 * Frees all allocated memory
 */
Application::~Application() noexcept
{
	// doesn't break if GLFW is not initialized
	glfwTerminate();
}


/**
 * Initializes the application and all its dependencies, e.g. render objects.
 */
void Application::initialize()
{
	if(glfwInit() == GLFW_FALSE)
	{
		error("Failed to initialize GLFW");
	}

	glfwSetErrorCallback([](const std::int32_t iErrorCode, const char* const szDescription)
	{
		error(szDescription);
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ApplicationSettings::k_uiMajorOpenGLVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ApplicationSettings::k_uiMinorOpenGLVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, ApplicationSettings::k_bResizableWindow ? GLFW_TRUE : GLFW_FALSE);

	m_pWindowHandle = glfwCreateWindow(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight, ApplicationSettings::k_szWindowTitle, nullptr, nullptr);
	if(m_pWindowHandle == nullptr)
	{
		error("Failed to create GLFW window");
	}

	glfwSetWindowUserPointer(m_pWindowHandle, &m_xWindow);
	glfwSetFramebufferSizeCallback(m_pWindowHandle, [](GLFWwindow* const pWindow, const std::int32_t iWidth, const std::int32_t iHeight)
		{
			Window* const pApplicationWindow = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
			pApplicationWindow->setSize(iWidth, iHeight);
		});
	glfwSetCursorPosCallback(m_pWindowHandle, [](GLFWwindow* const pWindow, const double dCursorX, const double dCursorY)
		{
			Window* const pApplicationWindow = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
			pApplicationWindow->setMousePosition(dCursorX, dCursorY);
		});
	glfwSetMouseButtonCallback(m_pWindowHandle, [](GLFWwindow* const pWindow, const std::int32_t iButton, const std::int32_t iAction, const std::int32_t iModifier)
		{
			Window* const pApplicationWindow = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
			Window::EMouseButton eButton;
			switch(iButton)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				eButton = Window::EMouseButton::eLeft;
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				eButton = Window::EMouseButton::eRight;
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				eButton = Window::EMouseButton::eMiddle;
				break;
			}
			Window::EAction eState;
			switch(iAction)
			{
			case GLFW_PRESS:
				eState = Window::EAction::ePressed;
				break;
			case GLFW_RELEASE:
				eState = Window::EAction::eReleased;
				break;
			}
			pApplicationWindow->setMouseButtonState(eButton, eState);
		});

	glfwMakeContextCurrent(m_pWindowHandle);

	m_xRenderer.initialize();

	initializeRenderData();
}


/**
 * Initializes render data, e.g. buffers, render passes, ...
 */
void Application::initializeRenderData()
{
	// initialize vertex/index buffers
	const auto rgFaces = make_array(
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		5, 4, 0,
		1, 5, 0,

		11, 6,  7,
		11, 7,  8,
		11, 8,  9,
		11, 9,  10,
		11, 10, 6,

		1, 2, 6,
		2, 3, 7,
		3, 4, 8,
		4, 5, 9,
		5, 1, 10,

		2,  7, 6,
		3,  8, 7,
		4,  9, 8,
		5, 10, 9,
		1, 6, 10
	);

	const auto rgVertices = make_array(
		0.000f,  0.000f,  1.000f,
		0.894f,  0.000f,  0.447f,
		0.276f,  0.851f,  0.447f,
		-0.724f,  0.526f,  0.447f,
		-0.724f, -0.526f,  0.447f,
		0.276f, -0.851f,  0.447f,
		0.724f,  0.526f, -0.447f,
		-0.276f,  0.851f, -0.447f,
		-0.894f,  0.000f, -0.447f,
		-0.276f, -0.851f, -0.447f,
		0.724f, -0.526f, -0.447f,
		0.000f,  0.000f, -1.000f
	);

	m_uiIndexCount = rgFaces.size();

	m_pVertexBuffer = new Buffer(static_cast<std::uint32_t>(rgVertices.size()) * sizeof(float), Buffer::Usage::eVertex);
	void* const pVertexBufferMemory = m_pVertexBuffer->map();
	memcpy(pVertexBufferMemory, rgVertices.data(), m_pVertexBuffer->size());
	m_pVertexBuffer->unmap();

	m_pIndexBuffer = new Buffer(static_cast<std::uint32_t>(rgFaces.size()) * sizeof(std::uint32_t), Buffer::Usage::eIndex);
	void* const pIndexBufferMemory = m_pIndexBuffer->map();
	memcpy(pIndexBufferMemory, rgFaces.data(), m_pIndexBuffer->size());
	m_pIndexBuffer->unmap();
	// initialize shader programs
	const std::string strVertexShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.vert");
	const std::string strFragmentShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.frag");
	const std::string strGeometryShader = readFile(GLSL_SHADER_PATH "/basic/basic.geom");
	const std::string strTessellationControlShader = readFile(GLSL_SHADER_PATH "/basic/basic.tesc");
	const std::string strTessellationEvaluationShader = readFile(GLSL_SHADER_PATH "/basic/basic.tese");
	// initialize vertex attributes
	const std::vector<std::pair<std::uint8_t, VertexAttribute>> vecAttributes =
	{
		{ 0, VertexAttribute(VertexAttribute::Format::eRGB32F, 0) },
	};
	// initialize uniform buffers
	Buffer xMatrixUniform(sizeof(glm::mat4) + sizeof(glm::mat4) + (sizeof(float) * 12), Buffer::Usage::eUniform);
	Buffer xTessellationUniform(sizeof(float) * 2, Buffer::Usage::eUniform);
	Buffer xLightUniform(sizeof(float) * 4 * 3, Buffer::Usage::eUniform);
	// initialize render pass
	m_pRenderPass = new RenderPass(strVertexShaderSource, strFragmentShaderSource, strGeometryShader, strTessellationControlShader, strTessellationEvaluationShader, vecAttributes, { xMatrixUniform, xTessellationUniform, xLightUniform }, {}, {});

	m_pMatrixMemory = xMatrixUniform.map();
	m_pTessellationMemory = xTessellationUniform.map();
	m_pLightMemory = xLightUniform.map();
}


/**
* Renders the data to the screen
*/
void Application::render()
{
	m_xRenderer.clearScreen();

	m_pRenderPass->display();
	glBindVertexBuffer(0, m_pVertexBuffer->m_uiHandle, 0, sizeof(float) * 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBuffer->m_uiHandle);
	glDrawElements(GL_PATCHES, m_uiIndexCount, GL_UNSIGNED_INT, nullptr);

	static int iTessLevelInner = 2;
	static int iTessLevelOuter = 3;

	ImGui::Begin("Settings", (bool*)0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SliderInt("TessLevelInner", &iTessLevelInner, 1, 50);
		ImGui::SliderInt("TessLevelOuter", &iTessLevelOuter, 1, 50);
	ImGui::End();

	const glm::mat4 xPerspective = glm::frustum(-0.5f, 0.5f, -0.5f, 0.5f, 5.0f, 150.0f);
	const glm::mat4 xModelview = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(), glm::vec3(0, 1, 0));
	const glm::mat3 xNormalMatrix = glm::mat3(xModelview);

	*static_cast<glm::mat4*>(m_pMatrixMemory) = xPerspective;
	*(static_cast<glm::mat4*>(m_pMatrixMemory) + 1) = xModelview;
	*(reinterpret_cast<glm::vec4*>(static_cast<glm::mat4*>(m_pMatrixMemory) + 2)) = glm::vec4(xNormalMatrix[0], 0.0f);
	*(reinterpret_cast<glm::vec4*>(static_cast<glm::mat4*>(m_pMatrixMemory) + 2) + 1) = glm::vec4(xNormalMatrix[1], 0.0f);
	*(reinterpret_cast<glm::vec4*>(static_cast<glm::mat4*>(m_pMatrixMemory) + 2) + 2) = glm::vec4(xNormalMatrix[2], 0.0f);

	*static_cast<float*>(m_pTessellationMemory) = iTessLevelInner;
	*(static_cast<float*>(m_pTessellationMemory) + 1) = iTessLevelOuter;

	*(static_cast<glm::vec4*>(m_pLightMemory)) = glm::vec4(0.25f, 0.25f, 1.0f, 0.0f);
	*(static_cast<glm::vec4*>(m_pLightMemory) + 1) = glm::vec4(0.0f, 0.75f, 0.75f, 0.0f);
	*(static_cast<glm::vec4*>(m_pLightMemory) + 2) = glm::vec4(0.04f, 0.04f, 0.04f, 0.0f);
}


/**
 * Callback for when the window size changed
 *
 * @param dWidth   new width
 * @param dHeight  new height
 */
void Application::sizeChanged(const double dWidth, const double dHeight)
{
	const std::uint32_t uiOriginalWidth = ApplicationSettings::k_uiWindowWidth;
	const std::uint32_t uiOriginalHeight = ApplicationSettings::k_uiWindowHeight;

	const double dOriginalAspectRatio = (static_cast<double>(uiOriginalWidth) / uiOriginalHeight);
	const double dAspectRatio = dWidth / dHeight;

	if(dOriginalAspectRatio > dAspectRatio)
	{
		const std::uint32_t uiWidth = static_cast<std::uint32_t>(dWidth);
		const std::uint32_t uiHeight = static_cast<std::uint32_t>(dHeight * dAspectRatio / dOriginalAspectRatio);
		glViewport(0, static_cast<std::uint32_t>((dHeight - uiHeight) / 2), uiWidth, uiHeight);
	}
	else
	{
		const std::uint32_t uiWidth = static_cast<std::uint32_t>(dWidth * dOriginalAspectRatio / dAspectRatio);
		const std::uint32_t uiHeight = static_cast<std::uint32_t>(dHeight);
		glViewport(static_cast<std::uint32_t>((dWidth - uiWidth) / 2), 0, uiWidth, uiHeight);
	}
}


/**
 * Launches the application and keeps it running until it is terminated.
 */
void Application::launch()
{
	while(not glfwWindowShouldClose(m_pWindowHandle))
	{
		glfwPollEvents();

		m_xWindow.display();

		glfwSwapBuffers(m_pWindowHandle);
	}
}


/**
 * Throws a given exception.
 *
 * @param xException  exception to throw
 * @throws std::exception
 */
void Application::error(const std::exception& xException)
{
	throw xException;
}


/**
 * Throws an std::runtime_error exception given an error description.
 *
 * @param strDescription  description of the error
 * @throws std::runtime_error
 */
void Application::error(const std::string& strDescription)
{
	throw std::runtime_error(strDescription);
}
