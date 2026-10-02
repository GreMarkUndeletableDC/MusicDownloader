#pragma once
#include "CApp.h"

class CWindowAbout : public eck::CForm
{
private:
    HBITMAP m_hbmBkg{};
public:
    LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;
};