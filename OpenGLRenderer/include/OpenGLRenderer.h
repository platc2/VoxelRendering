#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <cstdint>

#include <imgui.h>


namespace voxel
{

class Texture;

class COpenGLRenderer final
{
private:
	ImGuiIO* m_pIO;

	std::uint32_t m_uiProgram;
	std::uint32_t m_uiVBO;
	std::uint32_t m_uiEBO;
	std::uint32_t m_uiVAO;

	Texture* m_xTexture;

public:
	COpenGLRenderer();

	virtual ~COpenGLRenderer();

	void initialize();

	void clearScreen();

	void prepareGui(const double dMousePositionX, const double dMousePositionY, const bool bMouseDown0, const bool bMouseDown1, const std::uint32_t uiWindowWidth, const std::uint32_t uiWindowHeight);

	void renderGui();

private:
	void renderGui(ImDrawData* const pDrawData);
};

}


#endif
