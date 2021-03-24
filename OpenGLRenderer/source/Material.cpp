#include "Material.h"

#include <vector>
#include <stdexcept>
#ifdef _MSC_VER
	#include <ciso646>
#endif

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

using namespace voxel;


std::uint32_t CMaterial::s_uiLastUsed = 0;


CMaterial CMaterial::fromShader(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource)
{
	return CMaterial(strVertexShaderSource, strFragmentShaderSource);
}


CMaterial CMaterial::fromShader(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource, const std::string& strGeometryShaderSource)
{
	return CMaterial(strVertexShaderSource, strFragmentShaderSource, strGeometryShaderSource);
}


CMaterial::~CMaterial()
{
	glDeleteProgram(m_uiProgramHandle);
}


CMaterial::CMaterial(CMaterial&& xOther)
	: m_uiProgramHandle(xOther.m_uiProgramHandle)
{
	xOther.m_uiProgramHandle = 0;
}


void CMaterial::use() const
{
	if(m_uiProgramHandle not_eq s_uiLastUsed)
	{
		glUseProgram(m_uiProgramHandle);
	}
}


void CMaterial::setParameter(const std::string& strName, const float fValue) const
{
	const GLuint uiLocation = glGetUniformLocation(m_uiProgramHandle, strName.c_str());
	use();
	glUniform1f(uiLocation, fValue);
}


void CMaterial::setParameter(const std::string& strName, const glm::vec3& xValue) const
{
	const GLuint uiLocation = glGetUniformLocation(m_uiProgramHandle, strName.c_str());
	use();
	glUniform3fv(uiLocation, 1, glm::value_ptr(xValue));
}


void CMaterial::setParameter(const std::string& strName, const glm::mat4& xValue) const
{
	const GLuint uiLocation = glGetUniformLocation(m_uiProgramHandle, strName.c_str());
	use();
	glUniformMatrix4fv(uiLocation, 1, GL_FALSE, glm::value_ptr(xValue));
}


CMaterial::CMaterial(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource)
	: m_uiProgramHandle(0)
{
	const GLuint uiVertexShader = createShader(GL_VERTEX_SHADER, strVertexShaderSource);
	const GLuint uiFragmentShader = createShader(GL_FRAGMENT_SHADER, strFragmentShaderSource);

	const GLuint uiProgramHandle = glCreateProgram();
	glAttachShader(uiProgramHandle, uiVertexShader);
	glAttachShader(uiProgramHandle, uiFragmentShader);
	glLinkProgram(uiProgramHandle);
	GLint iStatus;
	glGetProgramiv(uiProgramHandle, GL_LINK_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetProgramiv(uiProgramHandle, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetProgramInfoLog(uiProgramHandle, iInfoLogLength, nullptr, vecInfoLog.data());
		const std::string strInfoLog = std::string(std::begin(vecInfoLog), std::end(vecInfoLog));
		throw std::runtime_error("Failed to link program: " + strInfoLog);
	}

	m_uiProgramHandle = uiProgramHandle;
}


CMaterial::CMaterial(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource, const std::string& strGeometryShaderSource)
	: m_uiProgramHandle(0)
{
	const GLuint uiVertexShader = createShader(GL_VERTEX_SHADER, strVertexShaderSource);
	const GLuint uiFragmentShader = createShader(GL_FRAGMENT_SHADER, strFragmentShaderSource);
	const GLuint uiGeometryShader = createShader(GL_GEOMETRY_SHADER, strGeometryShaderSource);

	const GLuint uiProgramHandle = glCreateProgram();
	glAttachShader(uiProgramHandle, uiVertexShader);
	glAttachShader(uiProgramHandle, uiFragmentShader);
	glAttachShader(uiProgramHandle, uiGeometryShader);
	glLinkProgram(uiProgramHandle);
	GLint iStatus;
	glGetProgramiv(uiProgramHandle, GL_LINK_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetProgramiv(uiProgramHandle, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetProgramInfoLog(uiProgramHandle, iInfoLogLength, nullptr, vecInfoLog.data());
		const std::string strInfoLog = std::string(std::begin(vecInfoLog), std::end(vecInfoLog));
		throw std::runtime_error("Failed to link program: " + strInfoLog);
	}

	m_uiProgramHandle = uiProgramHandle;
}


std::uint32_t CMaterial::createShader(const std::int32_t iShaderType, const std::string& strShaderSource)
{
	const GLuint uiShader = glCreateShader(iShaderType);
	const char* const szSource = strShaderSource.c_str();
	glShaderSource(uiShader, 1, &szSource, nullptr);
	glCompileShader(uiShader);
	GLint iStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetShaderInfoLog(uiShader, iInfoLogLength, nullptr, vecInfoLog.data());
		const std::string strInfoLog = std::string(std::begin(vecInfoLog), std::end(vecInfoLog));
		throw std::runtime_error("Failed to compile shader: " + strInfoLog);
	}

	return uiShader;
}
