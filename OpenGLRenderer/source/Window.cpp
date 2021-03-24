#include "Window.h"

using namespace voxel;


Window::Window(COpenGLRenderer* const pRenderer, const std::int32_t iWidth, const std::int32_t iHeight, const Callbacks& xCallbacks) : 
	k_xCallbacks{xCallbacks},
	k_pRenderer{pRenderer},
	m_bMousePositionInitialized{false},
	m_dMousePositionX{0},
	m_dMousePositionY{0},
	m_uiWidth{static_cast<std::uint32_t>(iWidth)},
	m_uiHeight{static_cast<std::uint32_t>(iHeight)}
{
}


void Window::display()
{
	// prepare DearImGui
	k_pRenderer->prepareGui(m_dMousePositionX, m_dMousePositionY, m_bLeft, m_bRight, m_uiWidth, m_uiHeight);

	std::for_each(std::cbegin(k_xCallbacks.vecRenderCallbacks), std::cend(k_xCallbacks.vecRenderCallbacks), [](const TRenderCallbackSignature fn) { fn(); });

	k_pRenderer->renderGui();
}


void Window::setSize(const double dWidth, const double dHeight)
{
	m_uiWidth = dWidth;
	m_uiHeight = dHeight;

	std::for_each(std::cbegin(k_xCallbacks.vecSizeChangedCallbacks), std::cend(k_xCallbacks.vecSizeChangedCallbacks),
		[&](const TSizeChangedCallbackSignature fn) { fn(dWidth, dHeight); });
}


void Window::setMousePosition(const double dPositionX, const double dPositionY)
{
	const double dMouseDeltaX = dPositionX - m_dMousePositionX;
	const double dMouseDeltaY = dPositionY - m_dMousePositionY;
	m_dMousePositionX = dPositionX;
	m_dMousePositionY = dPositionY;
	std::for_each(std::cbegin(k_xCallbacks.vecMouseMovedCallbacks), std::cend(k_xCallbacks.vecMouseMovedCallbacks),
		[dPositionX, dPositionY](const TMouseMovedCallbackSignature fn) { fn(dPositionX, dPositionY); });
}


void Window::setMouseButtonState(const EMouseButton eButton, const EAction eState)
{
	if(eButton == EMouseButton::eLeft) { m_bLeft = eState == EAction::ePressed; }
	if(eButton == EMouseButton::eRight) { m_bRight = eState == EAction::ePressed; }
	std::for_each(std::cbegin(k_xCallbacks.vecMouseButtonCallbacks), std::cend(k_xCallbacks.vecMouseButtonCallbacks),
		[eButton, eState](const TMouseButtonCallbackSignature fn) { fn(eButton, eState); });
}
