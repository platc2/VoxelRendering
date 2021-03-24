#ifdef _MSC_VER
	#include <ciso646>
#endif

#include "OpenGLRenderer.h"
#include "Texture.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>


using namespace voxel;


static void errorCallback(const GLenum eSource, const GLenum eType, const GLuint uiId, const GLenum eSeverity, const GLsizei iLength, const GLchar* const szMessage, const void* const pUserParameter);


COpenGLRenderer::COpenGLRenderer()
{
}


COpenGLRenderer::~COpenGLRenderer()
{
}


const char* szVS = "\
#version 450\n\
layout(location = 0) in vec2 Position;\n\
layout(location = 1) in vec2 UV;\n\
layout(location = 2) in vec4 Color;\n\
\n\
uniform mat4 ProjMtx;\n\
\n\
out vec2 Frag_UV;\n\
out vec4 Frag_Color;\n\
\n\
void main()\n\
{\n\
	Frag_UV = UV;\n\
	Frag_Color = Color;\n\
\n\
	gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n\
}\n\
";

const char* szFS = "\
#version 450\n\
in vec2 Frag_UV;\n\
in vec4 Frag_Color;\n\
\n\
uniform sampler2D Texture;\n\
\n\
layout (location = 0) out vec4 Out_Color;\n\
\n\
void main()\n\
{\n\
	Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n\
}\n\
";


static void checkShader(const GLuint uiShader)
{
	GLint iStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetShaderInfoLog(uiShader, iInfoLogLength, nullptr, vecInfoLog.data());
		std::string strInfoLog(vecInfoLog.begin(), vecInfoLog.end());
		std::cout << strInfoLog << std::endl;
		exit(0);
	}
}


static void checkProgram(const GLuint uiProgram)
{
	GLint iStatus;
	glGetProgramiv(uiProgram, GL_LINK_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetProgramiv(uiProgram, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetProgramInfoLog(uiProgram, iInfoLogLength, nullptr, vecInfoLog.data());
		std::string strInfoLog(vecInfoLog.begin(), vecInfoLog.end());
		std::cout << strInfoLog << std::endl;
		exit(0);
	}
}


void COpenGLRenderer::initialize()
{
	if(glewInit() not_eq GLEW_OK)
	{
		throw std::runtime_error("Failed to initialize GLEW");
	}

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT);
	// Ensure nothing else is executed when an error occurs
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(errorCallback, this);
#endif

	// initialize DearImGui
	ImGui::CreateContext();
	m_pIO = &ImGui::GetIO();

//	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	std::int32_t iWidth = 0;
	std::int32_t iHeight = 0;
	std::uint8_t* pPixels = nullptr;
	m_pIO->Fonts->GetTexDataAsRGBA32(&pPixels, &iWidth, &iHeight);

	m_xTexture = new Texture(pPixels, iWidth, iHeight);
	m_pIO->Fonts->TexID = reinterpret_cast<void*>(m_xTexture->m_uiHandle);

	const GLuint uiVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(uiVertexShader, 1, &szVS, nullptr);
		glCompileShader(uiVertexShader);
		checkShader(uiVertexShader);
	const GLuint uiFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(uiFragmentShader, 1, &szFS, nullptr);
		glCompileShader(uiFragmentShader);
		checkShader(uiFragmentShader);

	m_uiProgram = glCreateProgram();
		glAttachShader(m_uiProgram, uiVertexShader);
		glAttachShader(m_uiProgram, uiFragmentShader);
		glLinkProgram(m_uiProgram);
		checkProgram(m_uiProgram);

	glGenBuffers(1, &m_uiVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glGenBuffers(1, &m_uiEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiEBO);

	glCreateVertexArrays(1, &m_uiVAO);
		glBindVertexArray(m_uiVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, pos)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, uv)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<void*>(offsetof(ImDrawVert, col)));
	glBindVertexArray(0);

	ImGui::StyleColorsDark();

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}


void COpenGLRenderer::clearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void COpenGLRenderer::prepareGui(const double dMousePositionX, const double dMousePositionY, const bool bMouseDown0, const bool bMouseDown1, const std::uint32_t uiWindowWidth, const std::uint32_t uiWindowHeight)
{
	m_pIO->DeltaTime = 1.0f / 60.0f;
	m_pIO->DisplaySize.x = uiWindowWidth;
	m_pIO->DisplaySize.y = uiWindowHeight;
	m_pIO->MousePos.x = dMousePositionX;
	m_pIO->MousePos.y = dMousePositionY;
	m_pIO->MouseDown[0] = bMouseDown0;
	m_pIO->MouseDown[1] = bMouseDown1;

	ImGui::NewFrame();
}


void COpenGLRenderer::renderGui()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* const pDrawData = ImGui::GetDrawData();
	renderGui(pDrawData);
}


void COpenGLRenderer::renderGui(ImDrawData* const pDrawData)
{
	const std::int32_t iFramebufferWidth = pDrawData->DisplaySize.x * m_pIO->DisplayFramebufferScale.x;
	const std::int32_t iFramebufferHeight = pDrawData->DisplaySize.y * m_pIO->DisplayFramebufferScale.y;
	if(iFramebufferWidth <= 0 or iFramebufferHeight <= 0)
	{
		return;
	}
	pDrawData->ScaleClipRects(m_pIO->DisplayFramebufferScale);

	GLint rgLastViewport[4];
	glGetIntegerv(GL_VIEWPORT, rgLastViewport);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	const glm::vec2 xDisplayPos = { pDrawData->DisplayPos.x, pDrawData->DisplayPos.y };
	const glm::vec2 xDisplaySize = { pDrawData->DisplaySize.x, pDrawData->DisplaySize.y };

	glViewport(0, 0, iFramebufferWidth, iFramebufferHeight);
	const glm::mat4 xOrtho = glm::ortho(xDisplayPos.x, xDisplayPos.x + xDisplaySize.x, xDisplayPos.y + xDisplaySize.y, xDisplayPos.y);

	glUseProgram(m_uiProgram);
		glUniform1i(glGetUniformLocation(m_uiProgram, "Texture"), 0);
		glUniformMatrix4fv(glGetUniformLocation(m_uiProgram, "ProjMtx"), 1, GL_FALSE, glm::value_ptr(xOrtho));

	glBindVertexArray(m_uiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiEBO);

	for(std::int32_t iCommandList = 0; iCommandList < pDrawData->CmdListsCount; ++iCommandList)
	{
		const ImDrawList* const cmd_list = pDrawData->CmdLists[iCommandList];
		const ImDrawIdx* pIndexBufferOffset = 0;

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for(std::int32_t iCommand = 0; iCommand < cmd_list->CmdBuffer.Size; ++iCommand)
		{
			const ImDrawCmd* const pcmd = &cmd_list->CmdBuffer[iCommand];
			const ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - xDisplayPos.x, pcmd->ClipRect.y - xDisplayPos.y, pcmd->ClipRect.z - xDisplayPos.x, pcmd->ClipRect.w - xDisplayPos.y);
			if (clip_rect.x < static_cast<float>(iFramebufferWidth) && clip_rect.y < static_cast<float>(iFramebufferHeight) && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
			{
				// Apply scissor/clipping rectangle
				glScissor((int)clip_rect.x, (int)(static_cast<float>(iFramebufferHeight) - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

				glBindTexture(GL_TEXTURE_2D, reinterpret_cast<GLuint>(pcmd->TextureId));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, pIndexBufferOffset);
			}
			pIndexBufferOffset += pcmd->ElemCount;
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	glViewport(rgLastViewport[0], rgLastViewport[1], rgLastViewport[2], rgLastViewport[3]);
}


static const char* errorSource(const GLenum eSource)
{
	switch(eSource)
	{
	case GL_DEBUG_SOURCE_API: return "GL_DEBUG_SOURCE_API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY: return "GL_DEBUG_SOURCE_THIRD_PARTY";
	case GL_DEBUG_SOURCE_APPLICATION: return "GL_DEBUG_SOURCE_APPLICATION";
	case GL_DEBUG_SOURCE_OTHER: return "GL_DEBUG_SOURCE_OTHER";
	// TODO: Don't return value, either throw exception or use mapping array
	default: return "";
	}
}

static const char* errorType(const GLenum eType)
{
	switch(eType)
	{
	case GL_DEBUG_TYPE_ERROR: return "GL_DEBUG_TYPE_ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY: return "GL_DEBUG_TYPE_PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE: return "GL_DEBUG_TYPE_PERFORMANCE";
	case GL_DEBUG_TYPE_MARKER: return "GL_DEBUG_TYPE_MARKER";
	case GL_DEBUG_TYPE_PUSH_GROUP: return "GL_DEBUG_TYPE_PUSH_GROUP";
	case GL_DEBUG_TYPE_POP_GROUP: return "GL_DEBUG_TYPE_POP_GROUP";
	case GL_DEBUG_TYPE_OTHER: return "GL_DEBUG_TYPE_OTHER";
	// TODO: Don't return value, either throw exception or use mapping array
	default: return "";
	}
}

#include <sstream>
static void errorCallback(const GLenum eSource, const GLenum eType, const GLuint uiId, const GLenum eSeverity, const GLsizei iLength, const GLchar* const szMessage, const void* const pUserParameter)
{
	std::stringstream xErrorString;
	xErrorString << errorSource(eSource) << ":" << errorType(eType) << "\n" << szMessage;
	switch(eType)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cerr << xErrorString.str() << std::endl;
		throw std::runtime_error(szMessage);
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: // fallthrough
	case GL_DEBUG_TYPE_PERFORMANCE: // fallthrough
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cerr << xErrorString.str() << std::endl;
		break;
	}
}
