#ifdef _MSC_VER
#include <ciso646>
#endif

// Stdlib
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
// Renderer
#include "OpenGLRenderer.h"
#include "RenderPass.h"
#include "Buffer.h"
// Voxel
#include "ArrayHelpers.h"
#include "FileUtilities.h"
#include "Trackball.h"
// Other
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace voxel;


struct ApplicationSettings
{
	/** Window width */
	static constexpr std::uint32_t k_uiWindowWidth = 500;
	/** Window height */
	static constexpr std::uint32_t k_uiWindowHeight = 500;
	/** Window title */
	static constexpr char* const k_szWindowTitle = "Cube";
	/** Whether or not the window is resizable */
	static constexpr bool k_bResizableWindow = false;
	/** Major OpenGL version */
	static const std::uint32_t k_uiMajorOpenGLVersion = 4;
	/** Minor OpenGL version */
	static const std::uint32_t k_uiMinorOpenGLVersion = 5;
};


struct RuntimeData
{
	Trackball t;
	glm::mat4 old_tb_matrix;
	glm::mat4 tb_matrix;
};


int main()
{
	if(not glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ApplicationSettings::k_uiMajorOpenGLVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ApplicationSettings::k_uiMinorOpenGLVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, ApplicationSettings::k_bResizableWindow ? GLFW_TRUE : GLFW_FALSE);

	GLFWwindow* const pxWindow = glfwCreateWindow(
		ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight, ApplicationSettings::k_szWindowTitle, nullptr, nullptr);
	if(pxWindow == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(pxWindow);

	// initialize renderer
	COpenGLRenderer* const pxRenderer = new COpenGLRenderer();
	pxRenderer->initialize();
	// initialize vertex/index buffers
	constexpr float fLeft = -0.5f;
	constexpr float fRight = 0.5f;
	constexpr float fUp = 0.5f;
	constexpr float fDown = -0.5f;
	constexpr float fFront = -0.5f;
	constexpr float fBack = 0.5f;
	// Initialize mesh
	const auto rgVertices = make_array(
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
		fLeft, fDown, fFront,    fLeft, fDown, fBack,    fRight, fDown, fBack,    fRight, fDown, fFront
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
	Buffer xVertexBuffer(static_cast<std::uint32_t>(rgVertices.size() * sizeof(float)), Buffer::Usage::eVertex);
	void* const pVertexMemory = xVertexBuffer.map();
	memcpy(pVertexMemory, rgVertices.data(), xVertexBuffer.size());
	xVertexBuffer.unmap();
	Buffer xIndexBuffer(static_cast<std::uint32_t>(rgIndices.size() * sizeof(std::uint32_t)), Buffer::Usage::eIndex);
	void* const pIndexMemory = xIndexBuffer.map();
	memcpy(pIndexMemory, rgIndices.data(), xIndexBuffer.size());
	xIndexBuffer.unmap();
	// initialize shader programs
	const std::string strVertexShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.vert");
	const std::string strFragmentShaderSource = readFile(GLSL_SHADER_PATH "/basic/basic.frag");
	const std::string strNormalVertexShaderSource = readFile(GLSL_SHADER_PATH "/normal/normal.vert");
	const std::string strNormalGeometryShaderSource = readFile(GLSL_SHADER_PATH "/normal/normal.geom");
	// Uniform buffers
	Buffer xModelMatrix(sizeof(glm::mat4), Buffer::Usage::eUniform);
	Buffer xCubeColor(sizeof(glm::vec3), Buffer::Usage::eUniform);
	Buffer xNormalColor(sizeof(glm::vec3), Buffer::Usage::eUniform);
	Buffer xNormalData(sizeof(float), Buffer::Usage::eUniform);
	// Vertex attributes
	const std::vector<std::pair<std::uint8_t, VertexAttribute>> vecAttributes =
	{
		{ 0, VertexAttribute(VertexAttribute::Format::eRGB32F, 0) }
	};
	// initialize render passes
	RenderPass xPass(strVertexShaderSource, strFragmentShaderSource, vecAttributes, { xModelMatrix, xCubeColor }, {}, {});
	RenderPass xNormalPass(strNormalVertexShaderSource, strFragmentShaderSource, strNormalGeometryShaderSource, vecAttributes, { xModelMatrix, xNormalColor, xNormalData }, {}, {});
	// initialize view and projection matrices
	const glm::mat4 xProjection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	const glm::mat4 xView = glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 xViewProjection = xProjection * xView;

	RuntimeData xData;
	xData.old_tb_matrix = glm::mat4(1.0f);
	xData.tb_matrix = glm::mat4(1.0f);

	const auto xMousePressed = [](GLFWwindow* const pxWindow, const std::int32_t iButton, const std::int32_t iAction, const std::int32_t iModifier)
	{
		if(iButton == GLFW_MOUSE_BUTTON_LEFT and iAction == GLFW_PRESS)
		{
			double dMouseX = 0.0;
			double dMouseY = 0.0;
			glfwGetCursorPos(pxWindow, &dMouseX, &dMouseY);
			const glm::vec2 xMouseCoordinates = 2.0f * glm::vec2(dMouseX, dMouseY) / glm::vec2(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight) - 1.0f;
			RuntimeData& xData = *static_cast<RuntimeData*>(glfwGetWindowUserPointer(pxWindow));
			xData.t.beginDrag(glm::vec2(xMouseCoordinates.x, -xMouseCoordinates.y));
			xData.old_tb_matrix = xData.tb_matrix;
		}
	};

	const auto xMouseMoved = [](GLFWwindow* const pxWindow, const double dDeltaX, const double dDeltaY)
	{
		if(glfwGetMouseButton(pxWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			RuntimeData& xData = *static_cast<RuntimeData*>(glfwGetWindowUserPointer(pxWindow));
			const glm::vec2 xMouseCoordinates = 2.0f * glm::vec2(dDeltaX, dDeltaY) / glm::vec2(ApplicationSettings::k_uiWindowWidth, ApplicationSettings::k_uiWindowHeight) - 1.0f;
			xData.tb_matrix = xData.t.drag(glm::vec2(xMouseCoordinates.x, -xMouseCoordinates.y)) * xData.old_tb_matrix;
		}
	};

	glfwSetWindowUserPointer(pxWindow, &xData);
	glfwSetMouseButtonCallback(pxWindow, xMousePressed);
	glfwSetCursorPosCallback(pxWindow, xMouseMoved);

	void* const pModelMatrixMemory = xModelMatrix.map();
	void* const pNormalDataMemory = xNormalData.map();
	*static_cast<float*>(pNormalDataMemory) = 0.5f;
	xNormalData.unmap();
	void* const pCubeColorMemory = xCubeColor.map();
	*static_cast<glm::vec3*>(pCubeColorMemory) = glm::vec3(0.75f);
	xCubeColor.unmap();
	void* const pNormalColor = xNormalColor.map();
	*static_cast<glm::vec3*>(pNormalColor) = glm::vec3(0.0f, 0.75f, 0.0f);
	xNormalColor.unmap();
	while(not glfwWindowShouldClose(pxWindow))
	{
		glfwPollEvents();

		pxRenderer->clearScreen();
		// calculate model-view-projection matrix
		const float fAngle = static_cast<float>(glfwGetTime() * 1.0f);
		const glm::mat4 xModel = glm::rotate(glm::mat4(1.0f), fAngle, glm::vec3(0.4f, 1.0f, 0.0f));
		const glm::mat4 xModelViewProjection = xViewProjection * xData.tb_matrix;

		*static_cast<glm::mat4*>(pModelMatrixMemory) = xModelViewProjection;

		xPass.display();
		glBindVertexBuffer(0, xVertexBuffer.m_uiHandle, 0, sizeof(float) * 3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, xIndexBuffer.m_uiHandle);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(rgIndices.size()), GL_UNSIGNED_INT, nullptr);
		xNormalPass.display();
		glBindVertexBuffer(0, xVertexBuffer.m_uiHandle, 0, sizeof(float) * 3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, xIndexBuffer.m_uiHandle);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(rgIndices.size()), GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(pxWindow);
	}

	glfwTerminate();

	return 0;
}
