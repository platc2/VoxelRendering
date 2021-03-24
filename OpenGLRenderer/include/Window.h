#ifndef OPENGLRENDERER_VOXEL_WINDOW_H
#define OPENGLRENDERER_VOXEL_WINDOW_H

#include "OpenGLRenderer.h"

#include <vector>
#include <algorithm>

#include <functional>


namespace voxel
{

class Window
{
public:
	enum class EMouseButton
	{
		eLeft,
		eRight,
		eMiddle,
	};

	enum class EAction
	{
		ePressed,
		eReleased,
	};

	using TRenderCallbackSignature = std::function<void(void)>;
	using TSizeChangedCallbackSignature = std::function<void(const double, const double)>;
	using TMouseMovedCallbackSignature = std::function<void(const double, const double)>;
	using TMouseButtonCallbackSignature = std::function<void(const EMouseButton eButton, const EAction eAction)>;

	struct Callbacks
	{
		std::vector<TRenderCallbackSignature> vecRenderCallbacks;
		std::vector<TSizeChangedCallbackSignature> vecSizeChangedCallbacks;
		std::vector<TMouseMovedCallbackSignature> vecMouseMovedCallbacks;
		std::vector<TMouseButtonCallbackSignature> vecMouseButtonCallbacks;
	};

private:
	const Callbacks k_xCallbacks;

	COpenGLRenderer* const k_pRenderer;

	bool m_bMousePositionInitialized;

	double m_dMousePositionX;

	double m_dMousePositionY;

	bool m_bLeft = false;
	bool m_bRight = false;

	std::uint32_t m_uiWidth;
	std::uint32_t m_uiHeight;

public:
	Window(COpenGLRenderer* const pRenderer, const std::int32_t iWidth, const std::int32_t iHeight, const Callbacks& xCallbacks);

	void display();

	void setSize(const double dWidth, const double dHeight);

	void setMousePosition(const double dPositionX, const double dPositionY);

	void setMouseButtonState(const EMouseButton eButton, const EAction eState);
};

}


#endif
