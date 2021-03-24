#ifdef _MSC_VER
#include <ciso646>
#endif

// Stdlib
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <functional>
// Renderer
#include "OpenGLRenderer.h"
#include "RenderPass.h"
#include "Buffer.h"
#include "Texture.h"
// Voxel
#include "ArrayHelpers.h"
#include "FileUtilities.h"
#include "Trackball.h"
#include "Window.h"
// Other
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

using namespace voxel;


struct ApplicationSettings
{
	/** Window width */
	static constexpr std::uint32_t k_uiWindowWidth = 800;
	/** Window height */
	static constexpr std::uint32_t k_uiWindowHeight = 800;
	/** Window title */
	static constexpr char* const k_szWindowTitle = "SobelCube";
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

	/**
	 * Index buffer containing the indices
	 */
	Buffer* m_pIndexBuffer;

	/**
	* Main render pass
	*/
	RenderPass* m_pPass;

	/**
	 * Trackball instance
	 */
	Trackball m_xTrackball;

	/**
	 * Old trackball matrix
	 */
	glm::mat4 m_xOldTrackballMatrix;

	/**
	 * (New) trackball matrix
	 */
	glm::mat4 m_xTrackballMatrix;

	/**
	 * Switch value
	 */
	std::int32_t m_iSwitchValue;

	/**
	 * Projection matrix
	 */
	glm::mat4 m_xProjection;

	/**
	 * View matrix
	 */
	glm::mat4 m_xViewMatrix;

	/**
	 * Filter kernel
	 */
	glm::mat3 m_xKernel;

	/**
	* Pointer to the memory location of the gamma uniform
	*/
	float* m_pGammaUniformMemory;

	/**
	 * Amount of indices
	 */
	std::uint32_t m_uiIndexCount;

	Buffer* m_pCubeBuffer;

	RenderPass* m_pCubePass;

	void* m_pModelMatrixMemory;

	void* m_pKernelMemory;

	void* m_pTextureSwitchMemory;

	bool m_bDragging = false;

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
Application::Application() noexcept
	: m_xWindow(&m_xRenderer, ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight, {
		{ std::bind(&Application::render, this) },
		{ std::bind(&Application::sizeChanged, this, std::placeholders::_1, std::placeholders::_2) },
		{ std::bind(&Application::mouseMoved, this, std::placeholders::_1, std::placeholders::_2) },
		{ std::bind(&Application::mousePressed, this, std::placeholders::_1, std::placeholders::_2) }
	})
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
	constexpr float fLeft = -0.5f;
	constexpr float fRight = -fLeft;
	constexpr float fUp = 0.5f;
	constexpr float fDown = -fUp;
	constexpr float fFront = 0.5f;
	constexpr float fBack = -fFront;
	// Initialize mesh
	const auto rgVertices = make_array(
		// Vertices
		// Front face
		fLeft, fUp, fFront,    fLeft, fDown, fFront,    fRight, fDown, fFront,    fRight, fUp, fFront,
		// Right face
		fRight, fUp, fFront,    fRight, fDown, fFront,    fRight, fDown, fBack,    fRight, fUp, fBack,
		// Back face
		fRight, fUp, fBack,     fRight, fDown, fBack,     fLeft, fDown, fBack,    fLeft, fUp, fBack,
		// Left face
		fLeft, fUp, fBack,    fLeft, fDown, fBack,     fLeft, fDown, fFront,    fLeft, fUp, fFront,
		// Top face
		fLeft, fUp, fBack,    fLeft, fUp, fFront,    fRight, fUp, fFront,    fRight, fUp, fBack,
		// Bottom face
		fLeft, fDown, fFront,    fLeft, fDown, fBack,    fRight, fDown, fBack,    fRight, fDown, fFront,

		// Indices
		// Front face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,
		// Right face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,
		// Back face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,
		// Left face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,
		// Top face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,
		// Bottom face
		0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f
	);
	const auto rgIndices = make_array<std::uint32_t>(
		// Front face
		0,  1,  2,     0,  2,  3,
		// Right face
		4,  5,  6,     4,  6,  7,
		// Back face
		8,  9, 10,     8, 10, 11,
		// Left face
		12, 13, 14,    12, 14, 15,
		// Top face
		16, 17, 18,    16, 18, 19,
		// Bottom face
		20, 21, 22,    20, 22, 23
		);
	m_uiIndexCount = static_cast<std::uint32_t>(rgIndices.size());
	m_pVertexBuffer = new Buffer(static_cast<std::uint32_t>(rgVertices.size() * sizeof(float)), Buffer::Usage::eVertex);
	void* const pVertexMemory = m_pVertexBuffer->map();
	memcpy(pVertexMemory, rgVertices.data(), m_pVertexBuffer->size());
	m_pVertexBuffer->unmap();
	m_pIndexBuffer = new Buffer(static_cast<std::uint32_t>(rgIndices.size() * sizeof(std::uint32_t)), Buffer::Usage::eIndex);
	void* const pIndexMemory = m_pIndexBuffer->map();
	memcpy(pIndexMemory, rgIndices.data(), m_pIndexBuffer->size());
	m_pIndexBuffer->unmap();
	// initialize shader programs
	const std::string strVertexShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.vert");
	const std::string strFragmentShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.frag");
	// Uniform buffers
	Buffer xModelMatrix(sizeof(glm::mat4), Buffer::Usage::eUniform);
	Buffer xCubeColor(sizeof(glm::vec3), Buffer::Usage::eUniform);
	Buffer xTextureSwitch(sizeof(std::uint32_t), Buffer::Usage::eUniform);
	// Vertex attributes
	const std::vector<std::pair<std::uint8_t, VertexAttribute>> vecAttributes =
	{
		{ 0, VertexAttribute(VertexAttribute::Format::eRGB32F, 0) },
	{ 1, VertexAttribute(VertexAttribute::Format::eRG32F, 0) },
	};
	// initialize texture
	const ImageFile xFloorImage = readImage(TEXTURE_PATH "/floor.tga");
	const ImageFile xCubeImage = readImage(TEXTURE_PATH "/cube.tga");
	const Texture xFloorTexture(xFloorImage.m_pImageData, xFloorImage.m_uiWidth, xFloorImage.m_uiHeight);
	const Texture xCubeTexture(xCubeImage.m_pImageData, xCubeImage.m_uiWidth, xCubeImage.m_uiHeight);
	// initialize render passes
	Texture xRenderTexture(nullptr, ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight);
	m_pPass = new RenderPass(strVertexShaderSource, strFragmentShaderSource, vecAttributes, { xModelMatrix, xCubeColor, xTextureSwitch }, { xFloorTexture, xCubeTexture }, { xRenderTexture, xRenderTexture });
	// initialize view and projection matrices
	m_xProjection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	// initialize screen quad
	const auto cubeVertices = make_array(
		-1.0f, 1.0f,  -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  -1.0f, 1.0f,

		0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f
	);
	m_pCubeBuffer = new Buffer(sizeof(float) * static_cast<std::uint32_t>(cubeVertices.size()), Buffer::Usage::eVertex);
	void* const pCubeBufferMemory = m_pCubeBuffer->map();
	memcpy(pCubeBufferMemory, cubeVertices.data(), m_pCubeBuffer->size());
	m_pCubeBuffer->unmap();
	const std::vector<std::pair<std::uint8_t, VertexAttribute>> vecCubeAttributes =
	{
		{ 0, VertexAttribute(VertexAttribute::Format::eRG32F, 0) },
		{ 1, VertexAttribute(VertexAttribute::Format::eRG32F, 0) },
	};
	const std::string strCubeVertex = readFile(GLSL_SHADER_PATH "/cube/cube.vert");
	const std::string strCubeFragment = readFile(GLSL_SHADER_PATH "/cube/cube.frag");
	Buffer xKernelBuffer(sizeof(float) * 12, Buffer::Usage::eUniform);
	Buffer xScreenSizeBuffer(sizeof(glm::vec2), Buffer::Usage::eUniform);
	m_pCubePass = new RenderPass(strCubeVertex, strCubeFragment, vecCubeAttributes, { xKernelBuffer, xScreenSizeBuffer }, { xRenderTexture }, {});

	m_xOldTrackballMatrix = glm::mat4(1.0f);
	m_xTrackballMatrix = glm::mat4(1.0f);
	m_iSwitchValue = 0;
	m_xViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_xKernel = glm::mat3(
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	);

/*
	const auto xKeyPressed = [](GLFWwindow* const pxWindow, const std::int32_t iKey, const std::int32_t iScanCode, const std::int32_t iAction, const std::int32_t iModifier)
	{
		RuntimeData& xData = *static_cast<RuntimeData*>(glfwGetWindowUserPointer(pxWindow));
		if(iKey == GLFW_KEY_SPACE and iAction == GLFW_PRESS)
		{
			xData.switchValue = not xData.switchValue;
		}
		else if(iKey == GLFW_KEY_1 and iAction == GLFW_PRESS)
		{
			// identity
			xData.kernel = glm::mat3(
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f
			);
		}
		else if(iKey == GLFW_KEY_2 and iAction == GLFW_PRESS)
		{
			// sobel filter
			xData.kernel = glm::mat3(
				-1.0f, -1.0f, -1.0f,
				-1.0f, 8.0f, -1.0f,
				-1.0f, -1.0f, -1.0f
			);
		}
		else if(iKey == GLFW_KEY_3 and iAction == GLFW_PRESS)
		{
			// sharpen
			xData.kernel = glm::mat3(
				0.0f, -1.0f, 0.0f,
				-1.0f, 5.0f, -1.0f,
				0.0f, -1.0f, 0.0f
			);
		}
		else if(iKey == GLFW_KEY_4 and iAction == GLFW_PRESS)
		{
			// box blur
			const float fFactor = 1.0f / 9.0f;
			xData.kernel = glm::mat3(
				fFactor, fFactor, fFactor,
				fFactor, fFactor, fFactor,
				fFactor, fFactor, fFactor
			);
		}
		else if(iKey == GLFW_KEY_5 and iAction == GLFW_PRESS)
		{
			// gaussian blur
			xData.kernel = glm::mat3(
				1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
				2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
				1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
			);
		}
	};

	const auto xScrolled = [](GLFWwindow* const pxWindow, const double dXScroll, const double dYScroll)
	{
		RuntimeData& xData = *static_cast<RuntimeData*>(glfwGetWindowUserPointer(pxWindow));
		xData.view_matrix = glm::translate(xData.view_matrix, glm::vec3(0.0f, 0.0f, dYScroll));
	};
*/

	m_pModelMatrixMemory = xModelMatrix.map();
	void* const pCubeColorMemory = xCubeColor.map();
	*static_cast<glm::vec3*>(pCubeColorMemory) = glm::vec3(0.75f);
	xCubeColor.unmap();
	m_pTextureSwitchMemory = xTextureSwitch.map();
	void* const pScreenSizeMemory = xScreenSizeBuffer.map();
	*static_cast<glm::vec2*>(pScreenSizeMemory) = glm::vec2(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight);
	xScreenSizeBuffer.unmap();
	m_pKernelMemory = xKernelBuffer.map();
}


/**
* Renders the data to the screen
*/
void Application::render()
{
	m_xRenderer.clearScreen();
	// calculate model-view-projection matrix
	const glm::mat4 xModelViewProjection = m_xProjection * m_xViewMatrix * m_xTrackballMatrix;

	*static_cast<glm::mat4*>(m_pModelMatrixMemory) = xModelViewProjection;
	*static_cast<std::int32_t*>(m_pTextureSwitchMemory) = m_iSwitchValue;

	*static_cast<glm::vec4*>(m_pKernelMemory) = glm::vec4(m_xKernel[0], 0.0f);
	*(static_cast<glm::vec4*>(m_pKernelMemory) + 1) = glm::vec4(m_xKernel[1], 0.0f);
	*(static_cast<glm::vec4*>(m_pKernelMemory) + 2) = glm::vec4(m_xKernel[2], 0.0f);

	m_pPass->display();

	glViewport(0, 0, ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight);
	m_xRenderer.clearScreen();
	glBindVertexBuffer(0, m_pVertexBuffer->m_uiHandle, 0, sizeof(float) * 3);
	glBindVertexBuffer(1, m_pVertexBuffer->m_uiHandle, sizeof(float) * 72, sizeof(float) * 2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBuffer->m_uiHandle);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_uiIndexCount), GL_UNSIGNED_INT, nullptr);

	m_pCubePass->display();
	glViewport(0, 0, ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight);
	glBindVertexBuffer(0, m_pCubeBuffer->m_uiHandle, 0, sizeof(float) * 2);
	glBindVertexBuffer(1, m_pCubeBuffer->m_uiHandle, sizeof(float) * 12, sizeof(float) * 2);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


/**
* Callback for when the window size changed
*
* @param dWidth   new width
* @param dHeight  new height
*/
void Application::sizeChanged(const double dWidth, const double dHeight)
{
}


/**
* Callback for when the mouse was moved
*
* @param dDeltaX  delta x
* @param dDeltaY  delta y
*/
void Application::mouseMoved(const double dDeltaX, const double dDeltaY)
{
	if(m_bDragging)
	{
		const glm::vec2 xMouseCoordinates = 2.0f * glm::vec2(dDeltaX, dDeltaY) / glm::vec2(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight) - 1.0f;
		m_xTrackballMatrix = m_xTrackball.drag(glm::vec2(xMouseCoordinates.x, -xMouseCoordinates.y)) * m_xOldTrackballMatrix;
	}
}


/**
* Callback for when the state of a mouse button changed
*
* @param eButton  button which changes
* @param eAction  new state of the button
*/
void Application::mousePressed(const Window::EMouseButton eButton, const Window::EAction eAction)
{
	m_bDragging = (eAction == Window::EAction::ePressed);
	double dMouseX = 0.0;
	double dMouseY = 0.0;
	glfwGetCursorPos(m_pWindowHandle, &dMouseX, &dMouseY);
	const glm::vec2 xMouseCoordinates = 2.0f * glm::vec2(dMouseX, dMouseY) / glm::vec2(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight) - 1.0f;
	m_xTrackball.beginDrag(glm::vec2(xMouseCoordinates.x, -xMouseCoordinates.y));
	m_xOldTrackballMatrix = m_xTrackballMatrix;
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
