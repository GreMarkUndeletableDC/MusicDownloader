#include "pch.h"

#include "CApp.h"
#include "CWindowMain.h"

#include "eck\AutoLink.h"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pszCmdLine,
    _In_ int nCmdShow)
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        EckDbgPrintFormatMessage(hr);
        eck::MessageDialog(
            eck::Format(L"CoInitialize failed! hr = %08X", hr).Data(),
            L"Error",
            MB_ICONERROR);
        return 0;
    }
    UINT uErr;
    eck::StartupStatus iRetInit;
    if ((iRetInit = eck::Initialize(hInstance, NULL, &uErr)) != eck::StartupStatus::Ok)
    {
        EckDbgPrintFormatMessage(uErr);
        eck::MessageDialog(
            eck::Format(L"Init failed!\nInitStatus = %d\nError code = %08X",
            (int)iRetInit, uErr).Data(),
            L"Error",
            MB_ICONERROR);
        return 0;
    }

    CApp::Init();

    App = new CApp{};
    const auto pWnd = new CWindowMain{};
    pWnd->Create(L"申必音乐下载器", WS_OVERLAPPEDWINDOW, 0, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0);
    pWnd->Show(nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (!eck::PreTranslateMessage(msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    delete pWnd;
    delete App;
    eck::ThreadUninitialize();
    eck::Uninitialize();
    CoUninitialize();
    return (int)0;// msg.wParam;
}