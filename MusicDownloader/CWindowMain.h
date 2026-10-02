#pragma once
#include "CApp.h"
#include "CWindowAbout.h"

class CWindowMain : public eck::CForm
{
private:
    enum Src : BYTE
    {
        Netease,
        Tencent,
        Tidal,
        Spotify,
        YTMusic,
        Qobuz,
        Joox,
        Deezer,
        Migu,
        Kugou,
        Kuwo,
        Ximalaya
    };

    enum : UINT
    {
        IDMI = 100,
        IDMI_DOWNLOAD_DELETE,

        IDMI_SEARCH_DL_BEGIN,
    };

    enum class DTState : BYTE
    {
        Standby,
        FetchUrl,
        Downloading,
        DownloadingImg,
        FixMetadata,
        DownloadingLrc,
        Failed,
        Completed,
        Canceled
    };
    constexpr static std::wstring_view StateTip[]
    {
        L"就绪"sv,
        L"获取URL"sv,
        L"下载中"sv,
        L"下载封面"sv,
        L"修正元数据"sv,
        L"下载歌词"sv,
        L"下载失败"sv,
        L"下载完成"sv,
        L"已取消"sv
    };

    enum class LogLevel : BYTE
    {
        Info,
        Warning,
        Error
    };

    struct ITEM
    {
        eck::CStringW rsTitle;
        eck::CStringW rsArtist;
        eck::CStringW rsAlbum;
        eck::CStringW rsId;
        eck::CStringW rsImgId;
        eck::CStringW rsLrcId;
        BOOLEAN bFirstInGroup;
        Src eSource;
    };

    struct SEARCH_TASK
    {
        eck::CStringW rsText;	// 可能是多个，用CRLF分隔
        Src eSource;			// 
        BOOL bFromId;			// 是否从网易云ID搜索
    };

    struct LOG
    {
        LogLevel eLevel;
        eck::CStringW rsText;
        eck::CStringW rsDesc;
        eck::CStringW rsTime;
    };

    struct DOWNLOAD_TASK
    {
        ULONGLONG Tag;
        int idxFlat;
        USHORT usQuality;
        DTState eState;
        BYTE byProgress;
        eck::CoroTask<void> Task;
        eck::CStringW rsText;
        eck::CStringW rsArtist;
    };

    eck::ThreadContext* m_ptcUiThread{};

    eck::CTimeIdGenerator m_IdGen{};

    eck::CButton m_BTSearchId{};
    eck::CButton m_BTSearch{};
    eck::CButton m_BTAbout{};
    eck::CEditExt m_ED{};
    eck::CComboBoxNew m_CBBSource{};
    eck::CListBoxNew m_LBNSearch{};
    eck::CListBoxNew m_LBNTask{};
    eck::CListBoxNew m_LBNLog{};

    eck::CLinearLayoutH m_LytBtn{};
    eck::CLinearLayoutV m_LytLeft{};
    eck::CLinearLayoutV m_LytRight{};
    eck::CLinearLayoutH m_Lyt{};

    HFONT m_hFont{};
    HFONT m_hFontBig{};

    std::vector<ITEM> m_vSearchRes{};
    std::vector<LOG> m_vLog{};
    std::vector<std::shared_ptr<DOWNLOAD_TASK>> m_vDownload{};

    CWindowAbout* m_pWndAbout{};

    int m_iDpi = USER_DEFAULT_SCREEN_DPI;
    ECK_DS_BEGIN(DPIS)
        ECK_DS_ENTRY(cxLeft, 100)
        ECK_DS_ENTRY(cyBtn, 32)
        ECK_DS_ENTRY(Padding, 8)
        ECK_DS_ENTRY(cyTitle, 24)
        ECK_DS_ENTRY(cySubTitle, 18)
        ECK_DS_ENTRY(cyItem, 46)
        ECK_DS_ENTRY(TextPadding, 3)
        ECK_DS_ENTRY(cxTime, 120)
        ECK_DS_ENTRY(cxState, 50)
        ECK_DS_ENTRY(cxAbout, 500)
        ECK_DS_ENTRY(cyAbout, 500)
        ;
    ECK_DS_END_VAR(m_Ds)
private:
    EckInline void UpdateDpiInit(int iDpi)
    {
        m_iDpi = iDpi;
        eck::UpdateDpiSize(m_Ds, iDpi);
    }

    void UpdateDpi(int iDpi);

    void UpdateFixedUiSize();

    void ClearRes();

    BOOL OnCreate(HWND hWnd, CREATESTRUCT* lpCreateStruct);

    void AddLog(LogLevel eLevel, eck::CStringW rsText,
        eck::CStringW rsDesc = {});

    void ReplaceSpecialChar(PWSTR pszText);

    eck::CoroTask<void> TskSearch(SEARCH_TASK Tsk);

    void MarkDownloadFailed(std::shared_ptr<DOWNLOAD_TASK> pTsk);

    eck::CoroTask<void> TskDownload(ITEM Item,
        std::shared_ptr<DOWNLOAD_TASK> pTsk);

    BOOL CheckHttpError(PCWSTR pszText,
        const eck::CHttpRequestAsync& Req,
        eck::CHttpRequestAsync::Task Tsk,
        const eck::CStringW& rsDesc = {});

    LRESULT OnLBNCustomDrawSearch(eck::NMCUSTOMDRAWEXT* p);
    LRESULT OnLBNCustomDrawLog(eck::NMCUSTOMDRAWEXT* p);
    LRESULT OnLBNCustomDrawDownload(eck::NMCUSTOMDRAWEXT* p);
public:
    LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept override;
};