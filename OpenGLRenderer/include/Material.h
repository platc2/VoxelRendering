#ifndef OPENGLRENDERER_MATERIAL_H
#define OPENGLRENDERER_MATERIAL_H

#include <string>
#include <cstdint>

#include <glm/matrix.hpp>


namespace voxel
{

class CMaterial final
{
private:
	static std::uint32_t s_uiLastUsed;

	std::uint32_t m_uiProgramHandle;

public:
	static CMaterial fromShader(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource);

	static CMaterial fromShader(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource, const std::string& strGeometryShaderSource);

	virtual ~CMaterial();

	CMaterial(CMaterial&& xOther);

	void use() const;

	void setParameter(const std::string& strName, const float fValue) const;

	void setParameter(const std::string& strName, const glm::vec3& xValue) const;

	void setParameter(const std::string& strName, const glm::mat4& xValue) const;

private:
	CMaterial(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource);

	CMaterial(const std::string& strVertexShaderSource, const std::string& strFragmentShaderSource, const std::string& strGeometryShaderSource);

	static std::uint32_t createShader(const std::int32_t iShaderType, const std::string& strShaderSource);
};

}


#endif
