#include "pch.h"
#include "CWindowAbout.h"

LRESULT CWindowAbout::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        const auto spRes = eck::GetResource(MAKEINTRESOURCEW(IDR_XWFXS), L"BIN");
        eck::GpCreateGdiBitmap(m_hbmBkg, spRes.data(), spRes.size());
        SetBackgroundImage(m_hbmBkg);
        SetBackgroundMode(eck::ImageMode::Stretch);
    }
    break;

    case WM_DESTROY:
        DeleteObject(m_hbmBkg);
        m_hbmBkg = nullptr;
        break;
    }
    return __super::OnMessage(uMsg, wParam, lParam);
}