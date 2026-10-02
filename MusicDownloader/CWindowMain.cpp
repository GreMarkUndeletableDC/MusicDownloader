#include "pch.h"

#include "CWindowMain.h"

constexpr WCHAR ReqHeader[]
{
    LR"(User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36 Edg/127.0.0.0)"
    L"\r\n"
    LR"(Accept: application/json, text/plain, */*)"
    L"\r\n"
    LR"(Accept-Encoding: gzip, deflate)"
    L"\r\n"
    LR"(Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6)"
    L"\r\n"
    L"Cookie: appver=1.5.0.75771;\r\nReferer: http://music.163.com/\r\n"
};

constexpr std::wstring_view Source[]
{
    L"netease", L"tencent", L"tidal", L"spotify", L"ytmusic", L"qobuz", L"joox",
    L"deezer", L"migu", L"kugou", L"kuwo", L"ximalaya"
};

void CWindowMain::UpdateDpi(int iDpi)
{
    const int iDpiOld = m_iDpi;
    UpdateDpiInit(iDpi);

    HFONT hFont = eck::ReCreateFontForDpiChanged(m_hFont, iDpi, iDpiOld);
    eck::ApplyWindowFont(Handle, hFont);
    std::swap(m_hFont, hFont);
    DeleteObject(hFont);

    UpdateFixedUiSize();
}

void CWindowMain::UpdateFixedUiSize()
{
    m_Lyt.LoOnDpiChanged(m_iDpi);
    m_LBNSearch.SetItemHeight(m_Ds.cyItem);
    m_LBNTask.SetItemHeight(m_Ds.cyItem);
    m_LBNLog.SetItemHeight(m_Ds.cyItem);
}

void CWindowMain::ClearRes()
{
    DeleteObject(m_hFont);
    DeleteObject(m_hFontBig);
    if (m_pWndAbout)
    {
        if (m_pWndAbout->IsValid())
            m_pWndAbout->Destroy();
        delete m_pWndAbout;
    }
}

BOOL CWindowMain::OnCreate(HWND hWnd, CREATESTRUCT* lpCreateStruct)
{
    m_ptcUiThread = eck::PtcCurrent();
    m_ptcUiThread->UpdateDefaultColor();
    UpdateDpiInit(eck::GetDpi(hWnd));
    m_hFont = eck::EzFont(m_iDpi, L"微软雅黑", 9);
    m_hFontBig = eck::EzFont(m_iDpi, L"微软雅黑", 12, FW_BOLD);
    {
        m_BTAbout.Create(L"关于", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,
            0, 0, 0, m_Ds.cyBtn, hWnd, 0, NULL);
        m_LytLeft.LobAddObject(
            {
                .pObject = &m_BTAbout,
                .Margins = { 0, 0, 0, (float)m_Ds.Padding },
                .uFlags = eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT
            });

        m_ED.SetMultiLine(TRUE);
        m_ED.Create(NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0,
            0, 0, m_Ds.cxLeft, 0, hWnd, 0, NULL);
        m_ED.SetFrameType(eck::FrameType::Sunken);
        m_ED.SetText(L"By The River");
        m_LytLeft.LobAddObject(
            {
                .pObject = &m_ED,
                .Margins = { 0, 0, 0, (float)m_Ds.Padding },
                .uFlags = eck::LF_FIX_WIDTH | eck::LF_FILL_HEIGHT,
                .uWeight = 1
            });

        m_CBBSource.Create(nullptr, WS_CHILD | WS_VISIBLE, 0,
            0, 0, 0, m_Ds.cyBtn, hWnd, 0, NULL);
        m_CBBSource.GetListBox().SetItemCount(ARRAYSIZE(Source));
        m_CBBSource.GetListBox().SetCurrentSelection(0);
        m_LytLeft.LobAddObject(
            {
                .pObject = &m_CBBSource,
                .Margins = { 0, 0, 0, (float)m_Ds.Padding },
                .uFlags = eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT
            });

        {
            m_BTSearchId.Create(L"搜索ID", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,
                0, 0, 0, m_Ds.cyBtn, hWnd, 0, NULL);
            m_LytBtn.LobAddObject(
                {
                    .pObject = &m_BTSearchId,
                    .Margins = { 0, 0, 0, (float)m_Ds.Padding },
                    .uFlags = eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT,
                    .uWeight = 1
                });

            m_BTSearch.Create(L"搜索", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0,
                0, 0, 0, m_Ds.cyBtn, hWnd, 0, NULL);
            m_LytBtn.LobAddObject(
                {
                    .pObject = &m_BTSearch,
                    .Margins = { 0, 0, 0, (float)m_Ds.Padding },
                    .uFlags = eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT,
                    .uWeight = 1
                });
        }
        m_LytLeft.LobAddObject(
            {
                .pObject = &m_LytBtn,
                .uFlags = eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT
            });
    }
    m_Lyt.LobAddObject(
        {
            .pObject = &m_LytLeft,
            .uFlags = eck::LF_FIX_WIDTH | eck::LF_FILL_HEIGHT
        });

    m_LBNSearch.Create(NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
        0, 0, 0, 0, hWnd, 0);
    m_Lyt.LobAddObject(
        {
            .pObject = &m_LBNSearch,
            .Margins = { (float)m_Ds.Padding, 0, 0, 0 },
            .uFlags = eck::LF_FILL,
            .uWeight = 1
        });
    m_LBNSearch.SetItemHeight(m_Ds.cyItem);
    m_LBNSearch.SetMultiSel(TRUE);
    m_LBNSearch.SetExtendSel(TRUE);

    {
        m_LBNTask.Create(NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
            0, 0, 0, 0, hWnd, 0);
        m_LytRight.LobAddObject(
            {
                .pObject = &m_LBNTask,
                .uFlags = eck::LF_FILL,
                .uWeight = 3
            });
        m_LBNTask.SetItemHeight(m_Ds.cyItem);

        m_LBNLog.Create(NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
            0, 0, 0, 0, hWnd, 0);
        m_LytRight.LobAddObject(
            {
                .pObject = &m_LBNLog,
                .uFlags = eck::LF_FILL,
                .uWeight = 1
            });
        m_LBNLog.SetItemHeight(m_Ds.cyItem);
    }
    m_Lyt.LobAddObject(
        {
            .pObject = &m_LytRight,
            .uFlags = eck::LF_FILL,
            .uWeight = 1
        });

    m_Lyt.LoInitializeDpi(m_iDpi);

    eck::ApplyWindowFont(hWnd, m_hFont);
    return TRUE;
}

void CWindowMain::AddLog(LogLevel eLevel, eck::CStringW rsText,
    eck::CStringW rsDesc)
{
    m_ptcUiThread->Callback.EnQueueCallback(
        [=]() noexcept
        {
            SYSTEMTIME st;
            GetLocalTime(&st);
            eck::CStringW rsTime{};
            eck::FormatDateTime(rsTime, st);
            m_vLog.emplace_back(eLevel, rsText, rsDesc, std::move(rsTime));
            m_LBNLog.SetItemCount((int)m_vLog.size());
            m_LBNLog.EnsureVisible(m_LBNLog.GetItemCount() - 1);
            m_LBNLog.Redraw();
        });
}

void CWindowMain::ReplaceSpecialChar(PWSTR pszText)
{
    while (auto& ch = *pszText++)
    {
        if (ch == L'!' || ch == L'(' || ch == L')' || ch == L'\'' || ch == L'&')
            ch = L' ';
    }
}

eck::CoroTask<void> CWindowMain::TskSearch(SEARCH_TASK Tsk)
{
    auto Token{ co_await eck::CoroGetPromiseToken() };
    auto UiThread{ eck::CoroCaptureUiThread() };
    co_await eck::CoroResumeBackground();
    eck::CStringW rsUrl{}, rsNeteaseAlbum{};
    std::vector<ITEM> vTmp{};
    std::vector<std::wstring_view> vKeyWord{};
    eck::SplitString(
        Tsk.rsText.Data(), Tsk.rsText.Size(),
        EckArgString(L"\r\n"),
        0,
        [&](PWSTR psz, int cch) noexcept
        {
            *(psz + cch) = L'\0';
            vKeyWord.emplace_back(psz, cch);
        });
    eck::CHttpRequestAsync Req;
    Req.Header = ReqHeader;
    Req.AutoAddHeader = FALSE;
    for (const auto kw : vKeyWord)
    {
        Req.ClearResponse();
        PWSTR pszKeyWord{};
        eck::CStringW rsIdKeyWord{};
        if (Tsk.bFromId)// 查询网易云信息，ID转为可读文本
        {
            AddLog(LogLevel::Info, eck::Format(L"正在搜索ID：%s", kw.data()));
            rsUrl.Format(
                LR"(http://music.163.com/api/song/detail/?id=%s&ids=%%5B%s%%5D)",
                kw.data(), kw.data());
            auto TskReq{ Req.DoRequest(L"GET", rsUrl.Data(), rsUrl.Size()) };
            co_await TskReq;
            if (CheckHttpError(kw.data(), Req, TskReq, L"查询网易云信息失败"))
                continue;
            Json::CDocument Json{ Req.GetByteBuffer() };
            if (!Json.IsValid())
            {
                AddLog(LogLevel::Error,
                    eck::Format(L"搜索ID %s 失败：Json解析失败", kw.data()),
                    L"查询网易云信息失败");
                continue;
            }
            const int Code = Json["/code"].GetInt();
            if (Code != 200)
            {
                AddLog(LogLevel::Error,
                    eck::Format(L"搜索ID %s 失败：服务器返回 %d", kw.data(), Code),
                    L"查询网易云信息失败");
                continue;
            }
            auto vlSong = Json["/songs"];
            if (!vlSong.IsValid())
            {
                AddLog(LogLevel::Error,
                    eck::Format(L"搜索ID %s 失败：Json格式错误", kw.data()),
                    L"查询网易云信息失败");
                continue;
            }
            vlSong = vlSong[0];
            rsIdKeyWord.PushBack(vlSong["/name"].GetStringW());
            const auto vlArr = vlSong["/artists"];
            for (auto vl : vlArr.AsArray())
            {
                rsIdKeyWord.PushBackChar(L' ');
                rsIdKeyWord.PushBack(vl["/name"].GetStringW());
            }
            const auto vlAlbumName = vlSong["/album/name"];
            if (vlAlbumName.IsString())
                rsNeteaseAlbum = vlAlbumName.GetStringW();
            pszKeyWord = rsIdKeyWord.Data();
        }
        else
        {
            AddLog(LogLevel::Info, eck::Format(L"正在搜索：%s", kw.data()));
            pszKeyWord = (PWSTR)kw.data();
        }
        ReplaceSpecialChar(pszKeyWord);

        rsUrl.Format(LR"(https://music-api.gdstudio.xyz/api.php?)"
            LR"(types=search&source=%s&name=%s&count=10)",
            Source[(size_t)Tsk.eSource].data(), pszKeyWord);
        Req.ClearResponse();
        auto TskReq{ Req.DoRequest(L"GET", rsUrl.Data(), rsUrl.Size()) };
        co_await TskReq;
        if (CheckHttpError(pszKeyWord, Req, TskReq, L"请求搜索接口失败"))
            continue;
        const auto JsonW = eck::EcdUtf8ToWide(Req.GetByteBuffer());
        Json::CDocument Json{ Req.GetByteBuffer() };
        if (!Json.IsValid())
        {
            AddLog(LogLevel::Error,
                eck::Format(L"搜索 %s 失败：Json解析失败", pszKeyWord),
                L"查询网易云信息失败");
            continue;
        }
        BOOL bFirstInGroup = TRUE;
        for (auto vl : Json.GetRoot().AsArray())
        {
            auto& e = vTmp.emplace_back(
                vl["/name"].GetStringW(),
                eck::CStringW{},
                rsNeteaseAlbum.IsEmpty() ? vl["/album"].GetStringW() : rsNeteaseAlbum);
            e.rsId = vl["/url_id"].GetStringW();
            e.rsImgId = vl["/pic_id"].GetStringW();
            e.rsLrcId = vl["/lyric_id"].GetStringW();
            const auto arrArtist = vl["/artist"];
            BOOL bFirst = TRUE;
            for (auto vl : arrArtist.AsArray())
            {
                if (bFirst)
                    bFirst = FALSE;
                else
                    e.rsArtist.PushBackChar(L'、');
                e.rsArtist.PushBack(vl.GetStringW());
            }
            e.bFirstInGroup = bFirstInGroup;
            e.eSource = Tsk.eSource;
            if (bFirstInGroup)
                bFirstInGroup = FALSE;
            if (Tsk.bFromId && vTmp.size() >= 6)
                break;
        }
    }
    AddLog(LogLevel::Info, eck::Format(L"搜索完成，共找到 %d 条结果", (int)vTmp.size()));
    if (!vTmp.empty())
    {
        co_await UiThread;
        if (Token.GetPromise().IsCanceled())
            co_return;
        for (auto& e : vTmp)
            m_vSearchRes.emplace_back(std::move(e));
        m_LBNSearch.SetItemCount((int)m_vSearchRes.size());
        m_LBNSearch.Redraw();
    }
}

void CWindowMain::MarkDownloadFailed(std::shared_ptr<DOWNLOAD_TASK> pTsk)
{
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::Failed;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
}

eck::CoroTask<void> CWindowMain::TskDownload(ITEM Item,
    std::shared_ptr<DOWNLOAD_TASK> pTsk)
{
    auto Token{ co_await eck::CoroGetPromiseToken() };
    auto UiThread{ eck::CoroCaptureUiThread() };
    co_await eck::CoroResumeBackground();

    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::FetchUrl;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
    eck::CStringW rsUrl{};
    rsUrl.Format(LR"(https://music-api.gdstudio.xyz/api.php?)"
        LR"(types=url&source=%s&id=%s&br=%hu)",
        Source[(size_t)Item.eSource].data(),
        Item.rsId.Data(), pTsk->usQuality);
    eck::CHttpRequestAsync Req;
    Req.Header = ReqHeader;
    Req.AutoAddHeader = FALSE;
    auto TskReq{ Req.DoRequest(L"GET", rsUrl.Data(), rsUrl.Size()) };
    co_await TskReq;
    if (CheckHttpError(Item.rsTitle.Data(), Req, TskReq, L"请求歌曲链接失败"))
    {
        MarkDownloadFailed(pTsk);
        co_return;
    }
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::Downloading;
            pTsk->byProgress = 0;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
    Json::CDocument JsonReqUrl{ Req.GetByteBuffer() };
    if (!JsonReqUrl.IsValid())
    {
        AddLog(LogLevel::Error, eck::Format(L"下载歌曲 %s 失败：Json解析失败",
            Item.rsTitle.Data()));
        MarkDownloadFailed(pTsk);
        co_return;
    }

    const auto rsMusicUrl = JsonReqUrl["/url"].GetStringW();
    const auto cbMusicSize = (SIZE_T)JsonReqUrl["/size"].GetInt64();
    const auto posQ = rsMusicUrl.RFindChar(L'?');
    const auto posExt = rsMusicUrl.RFindChar(L'.', posQ);
    const auto cchExt = (posQ < 0 ? (rsMusicUrl.Size() - posExt) : int(posQ - posExt));

    eck::CStringW rsPath{ eck::GetRunningPath() };
    rsPath.PushBackChar(L'\\');
    const auto posFileName = rsPath.Size();
    rsPath.PushBackFormat(L"%s - %s",
        Item.rsTitle.Data(), Item.rsArtist.Data());
    eck::PazLegalize(rsPath.Data() + posFileName);
    if (posExt >= 0 && cchExt > 0)
        rsPath.PushBack(rsMusicUrl.Data() + posExt, cchExt);
    if (PathFileExistsW(rsPath.Data()))
    {
        eck::CStringW rs{ MAX_PATH };
        if (PathYetAnotherMakeUniqueName(rs.Data(), rsPath.Data(), NULL, NULL))
        {
            rs.ReCalculateLength();
            rsPath = std::move(rs);
        }
    }

    Req.WantFilePath(rsPath);
    TskReq = Req.DoRequest(L"GET", rsMusicUrl.Data(), rsMusicUrl.Size());
    TskReq.GetPromise().SetOnProgress([&](std::pair<SIZE_T, SIZE_T> Prog)
        {
            m_ptcUiThread->Callback.EnQueueCallback([=]
                {
                    SIZE_T cbTotal = Prog.second ? Prog.second : cbMusicSize;
                    if (cbTotal)
                        pTsk->byProgress = BYTE(Prog.first * 100 / cbTotal);
                    else
                        pTsk->byProgress = 100;
                    m_LBNTask.RedrawItem(pTsk->idxFlat);
                }, 0, TRUE, pTsk->Tag, TRUE);
        });
    Token.GetPromise().SetCanceller([](void* p)
        {
            ((decltype(TskReq)*)p)->TryCancel();
        }, &TskReq);
    co_await TskReq;
    Token.GetPromise().SetCanceller(nullptr, nullptr);
    if (CheckHttpError(Item.rsTitle.Data(), Req, TskReq, L"下载歌曲内容失败"))
    {
        MarkDownloadFailed(pTsk);
        co_return;
    }
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::DownloadingImg;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
    // 尝试补全元数据和图片
    rsUrl.Format(LR"(https://music-api.gdstudio.xyz/api.php?)"
        LR"(types=pic&source=%s&id=%s&size=500)",
        Source[(size_t)Item.eSource].data(), Item.rsImgId.Data());
    Req.WantByteBuffer();
    Req.ClearResponse();
    TskReq = Req.DoRequest(L"GET", rsUrl.Data(), rsUrl.Size());
    co_await TskReq;
    if (!CheckHttpError(Item.rsTitle.Data(), Req, TskReq, L"歌曲封面链接请求失败"))
    {
        eck::CStringW JsonW;
        eck::EcdUtf8ToWide(JsonW, (PCCH)Req.GetByteBuffer().Data(), (int)Req.GetByteBuffer().Size());
        Json::CDocument JsonPic{ Req.GetByteBuffer() };
        if (JsonPic.IsValid())
        {
            const auto rsPicUrl = JsonPic["/url"].GetStringW();
            Req.ClearResponse();
            TskReq = Req.DoRequest(L"GET", rsPicUrl.Data(), rsPicUrl.Size());
            co_await TskReq;
            CheckHttpError(Item.rsTitle.Data(), Req, TskReq, L"歌曲封面下载失败");
        }
    }
    const auto svContentType = eck::HeaderGetParam(
        Req.ResponseHeader.Data(), L"Content-Type");
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::FixMetadata;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
    Tag::CMediaFile File{ rsPath.Data(), STGM_READWRITE };
    File.DetectTag();
    if (File.GetTagType() & Tag::TAG_FLAC)
    {
        Tag::CFlac Tag{ File };
        Tag.ReadTag(0);
        auto& v = Tag.GetItemList();
        BOOL bTitle{}, bArtist{}, bAlbum{};
        for (const auto& e : v)
        {
            if (e.rsKey == "TITLE")
                bTitle = TRUE;
            else if (e.rsKey == "ARTIST")
                bArtist = TRUE;
            else if (e.rsKey == "ALBUM")
                bAlbum = TRUE;
        }
        if (!bTitle)
            v.emplace_back("TITLE", Item.rsTitle);
        if (!bArtist)
            v.emplace_back("ARTIST", Item.rsArtist);
        if (!bAlbum)
            v.emplace_back("ALBUM", Item.rsAlbum);

        auto& v2 = Tag.GetPictureList();
        if (v2.empty() && !Req.GetByteBuffer().IsEmpty())
        {
            auto& e = v2.emplace_back();
            e.rbData = std::move(Req.GetByteBuffer());
            e.eType = Tag::PictureType::CoverFront;

            eck::EcdWideToUtf8(
                e.rsMime,
                svContentType.data(), (int)svContentType.size());
        }
        Tag.WriteTag(0);
    }
    else
    {
        Tag::CID3v2 Tag{ File };
        Tag.ReadTag(0);
        auto& v = Tag.GetItemList();
        BOOL bTitle{}, bArtist{}, bAlbum{}, bImg{};
        for (const auto& e : v)
        {
            if (memcmp(e->Id, "TIT2", 4) == 0)
                bTitle = TRUE;
            else if (memcmp(e->Id, "TPE1", 4) == 0)
                bArtist = TRUE;
            else if (memcmp(e->Id, "TALB", 4) == 0)
                bAlbum = TRUE;
            else if (memcmp(e->Id, "APIC", 4) == 0)
                bImg = TRUE;
        }
        if (!bTitle)
        {
            auto p = std::make_unique<Tag::ID3v2::TEXTFRAME>("TIT2");
            p->vText.emplace_back(Item.rsTitle);
            p->eEncoding = Tag::ID3v2::TextEncoding::Utf16LE;
            v.emplace_back(std::move(p));
        }
        if (!bArtist)
        {
            auto p = std::make_unique<Tag::ID3v2::TEXTFRAME>("TPE1");
            p->vText.emplace_back(Item.rsArtist);
            p->eEncoding = Tag::ID3v2::TextEncoding::Utf16LE;
            v.emplace_back(std::move(p));
        }
        if (!bAlbum)
        {
            auto p = std::make_unique<Tag::ID3v2::TEXTFRAME>("TALB");
            p->vText.emplace_back(Item.rsAlbum);
            p->eEncoding = Tag::ID3v2::TextEncoding::Utf16LE;
            v.emplace_back(std::move(p));
        }
        if (!bImg && !Req.GetByteBuffer().IsEmpty())
        {
            auto p = std::make_unique<Tag::ID3v2::APIC>();
            p->rbData = std::move(Req.GetByteBuffer());
            p->eType = Tag::PictureType::CoverFront;
            eck::EcdWideToUtf8(
                p->rsMime,
                svContentType.data(), (int)svContentType.size());
            v.emplace_back(std::move(p));
        }
        Tag.WriteTag(Tag::MIF_CREATE_ID3V2_3);
    }
    // 下载歌词
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::DownloadingLrc;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
    rsPath.PazRemoveExtension();
    rsPath.PushBack(L".lrc");

    rsUrl.Format(LR"(https://music-api.gdstudio.xyz/api.php?)"
        LR"(types=lyric&source=%s&id=%s)",
        Source[(size_t)Item.eSource].data(), Item.rsLrcId.Data());
    Req.ClearResponse();
    TskReq = Req.DoRequest(L"GET", rsUrl.Data(), rsUrl.Size());
    co_await TskReq;
    if (!CheckHttpError(Item.rsTitle.Data(), Req, TskReq, L"下载歌词失败"))
    {
        Json::CDocument jLrc{ Req.GetByteBuffer() };
        if (jLrc.IsValid())
        {
            auto vl = jLrc["/lyric"];
            const auto pszLrc = vl.GetString();
            const auto cchLrc = vl.GetLength();
            vl = jLrc["/tlyric"];
            const auto pszTlyric = vl.GetString();
            const auto cchTlyric = vl.GetLength();
            constexpr size_t MinLrcLen = 5;
            if ((pszLrc || pszTlyric) &&
                (cchLrc > MinLrcLen || cchTlyric > MinLrcLen))
            {
                eck::CFile File{};
                File.Create(rsPath.Data(), FILE_OVERWRITE_IF, FILE_GENERIC_WRITE);
                File.Write(eck::BOM_UTF8, 3);
                if (pszLrc && cchLrc > MinLrcLen)
                    File.Write(pszLrc, (DWORD)cchLrc);
                if (pszTlyric && cchTlyric > MinLrcLen)
                    File.Write(pszTlyric, (DWORD)cchTlyric);
            }
        }
    }
    m_ptcUiThread->Callback.EnQueueCallback([=]
        {
            pTsk->eState = DTState::Completed;
            m_LBNTask.RedrawItem(pTsk->idxFlat);
        }, 0, TRUE, pTsk->Tag, TRUE);
}

BOOL CWindowMain::CheckHttpError(PCWSTR pszText,
    const eck::CHttpRequestAsync& Req,
    eck::CHttpRequestAsync::Task Tsk,
    const eck::CStringW& rsDesc)
{
    if (Req.ResponseCode != 200 && Req.ResponseCode != 0)
    {
        AddLog(LogLevel::Error, eck::Format(L"下载歌曲 %s 失败：服务器返回错误 %d",
            pszText, Req.ResponseCode), rsDesc);
        return TRUE;
    }
    if (FAILED(Tsk.GetReturnValue().value_or(S_OK)))
    {
        AddLog(LogLevel::Error, eck::Format(L"下载歌曲 %s 失败：网络请求失败，Hr = 0x%08X",
            pszText, Tsk.GetReturnValue().value()), rsDesc);
        return TRUE;
    }
    return FALSE;
}

LRESULT CWindowMain::OnLBNCustomDrawSearch(eck::NMCUSTOMDRAWEXT* p)
{
    switch (p->dwDrawStage)
    {
    case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT:
    {
        if (p->iStateId)
            DrawThemeBackground(m_LBNSearch.GetThemeHandle(), p->hdc,
                p->iPartId, p->iStateId, &p->rc, nullptr);
        SelectObject(p->hdc, m_hFontBig);
        RECT rc{ p->rc };
        rc.bottom = rc.top + m_Ds.cyTitle;
        eck::InflateRect(rc, -m_Ds.TextPadding, -m_Ds.TextPadding);

        SetTextColor(p->hdc, m_ptcUiThread->crDefText);
        DrawTextW(p->hdc, m_vSearchRes[p->dwItemSpec].rsTitle.Data(),
            m_vSearchRes[p->dwItemSpec].rsTitle.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP | DT_END_ELLIPSIS);

        rc.top = rc.bottom + m_Ds.TextPadding;
        rc.bottom = rc.top + m_Ds.cySubTitle;
        SelectObject(p->hdc, m_hFont);
        DrawTextW(p->hdc, m_vSearchRes[p->dwItemSpec].rsArtist.Data(),
            m_vSearchRes[p->dwItemSpec].rsArtist.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP | DT_END_ELLIPSIS);

        if (m_vSearchRes[p->dwItemSpec].bFirstInGroup)
        {
            SetDCBrushColor(p->hdc, eck::Colorref::CyanBlue);
            rc = p->rc;
            rc.bottom = rc.top + eck::DpiScale(2, m_iDpi);
            FillRect(p->hdc, &rc, GetStockBrush(DC_BRUSH));
        }
    }
    return CDRF_SKIPDEFAULT;
    }
    return 0;
}

LRESULT CWindowMain::OnLBNCustomDrawLog(eck::NMCUSTOMDRAWEXT* p)
{
    switch (p->dwDrawStage)
    {
    case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT:
    {
        if (p->iStateId)
            DrawThemeBackground(m_LBNSearch.GetThemeHandle(), p->hdc,
                p->iPartId, p->iStateId, &p->rc, nullptr);
        SetTextColor(p->hdc, m_ptcUiThread->crDefText);
        SelectObject(p->hdc, m_hFontBig);
        COLORREF cr{ CLR_INVALID };

        RECT rc{ p->rc };
        rc.bottom = rc.top + m_Ds.cyTitle;
        eck::InflateRect(rc, -m_Ds.TextPadding, -m_Ds.TextPadding);

        const auto& e = m_vLog[p->dwItemSpec];
        switch (e.eLevel)
        {
        case LogLevel::Error:
            cr = eck::Colorref::Red;
            break;
        case LogLevel::Warning:
            cr = eck::Colorref::Orange;
            break;
        }
        if (cr != CLR_INVALID)
            SetTextColor(p->hdc, cr);

        DrawTextW(p->hdc, e.rsText.Data(), e.rsText.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_END_ELLIPSIS);

        SelectObject(p->hdc, m_hFont);
        rc.top = rc.bottom + m_Ds.TextPadding;
        rc.bottom = rc.top + m_Ds.cySubTitle;
        DrawTextW(p->hdc, e.rsTime.Data(), e.rsTime.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_END_ELLIPSIS);
        eck::OffsetRect(rc, m_Ds.cxTime, 0);
        DrawTextW(p->hdc, e.rsDesc.Data(), e.rsDesc.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_END_ELLIPSIS);
    }
    return CDRF_SKIPDEFAULT;
    }
    return 0;
}

LRESULT CWindowMain::OnLBNCustomDrawDownload(eck::NMCUSTOMDRAWEXT* p)
{
    switch (p->dwDrawStage)
    {
    case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;
    case CDDS_ITEMPREPAINT:
    {
        if (p->iStateId)
            DrawThemeBackground(m_LBNSearch.GetThemeHandle(), p->hdc,
                p->iPartId, p->iStateId, &p->rc, nullptr);
        const auto& e = *m_vDownload[p->dwItemSpec];
        const auto bProgress = e.eState == DTState::Downloading;
        if (bProgress && e.byProgress)
        {
            const int cx = (p->rc.right - p->rc.left) * e.byProgress / 100;

            HDC hCDC = CreateCompatibleDC(p->hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(p->hdc, 1, 1);
            SelectObject(hCDC, hBitmap);

            constexpr RECT rcInt{ 0,0,1,1 };
            SetDCBrushColor(hCDC, 0xFFCC66);
            FillRect(hCDC, &rcInt, GetStockBrush(DC_BRUSH));

            BLENDFUNCTION bf{ .BlendOp = AC_SRC_OVER,.SourceConstantAlpha = 70 };
            AlphaBlend(p->hdc, p->rc.left, p->rc.top, cx, p->rc.bottom - p->rc.top,
                hCDC, 0, 0, 1, 1, bf);

            DeleteDC(hCDC);
            DeleteObject(hBitmap);
        }

        SetTextColor(p->hdc, m_ptcUiThread->crDefText);
        SelectObject(p->hdc, m_hFontBig);
        RECT rc{ p->rc };
        rc.bottom = rc.top + m_Ds.cyTitle;
        eck::InflateRect(rc, -m_Ds.TextPadding, -m_Ds.TextPadding);

        DrawTextW(p->hdc, e.rsText.Data(), e.rsText.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_END_ELLIPSIS);

        rc.top = rc.bottom + m_Ds.TextPadding;
        rc.bottom = rc.top + m_Ds.cySubTitle;
        SelectObject(p->hdc, m_hFont);
        DrawTextW(p->hdc, e.rsArtist.Data(), e.rsArtist.Size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_END_ELLIPSIS);

        rc.right = p->rc.right - m_Ds.TextPadding;
        rc.left = rc.right - m_Ds.cxState;
        if (e.eState == DTState::Failed)
            SetTextColor(p->hdc, eck::Colorref::Red);
        else if (e.eState == DTState::Completed)
            SetTextColor(p->hdc, eck::Colorref::Green);
        else if (e.eState == DTState::Canceled)
            SetTextColor(p->hdc, eck::Colorref::Gray);

        DrawTextW(p->hdc, StateTip[(int)e.eState].data(),
            (int)StateTip[(int)e.eState].size(),
            &rc, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX |
            DT_NOCLIP | DT_RIGHT);
    }
    return CDRF_SKIPDEFAULT;
    }
    return 0;
}

LRESULT CWindowMain::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (uMsg)
    {
    case WM_NOTIFY:
    {
        const auto pnm = (NMHDR*)lParam;
        if (pnm->hwndFrom == m_LBNSearch.Handle)
            switch (pnm->code)
            {
            case NM_CUSTOMDRAW:
                return OnLBNCustomDrawSearch((eck::NMCUSTOMDRAWEXT*)lParam);
            case NM_RCLICK:
            {
                const auto p = (eck::NMMOUSENOTIFY*)lParam;
                eck::CMenu Menu
                {
                    { L"下载 音质128",IDMI_SEARCH_DL_BEGIN },
                    { L"下载 音质192",IDMI_SEARCH_DL_BEGIN + 1 },
                    { L"下载 音质320",IDMI_SEARCH_DL_BEGIN + 2 },
                    { L"下载 音质740",IDMI_SEARCH_DL_BEGIN + 3 },
                    { L"下载 音质999",IDMI_SEARCH_DL_BEGIN + 4 },
                };
                constexpr static int Quality[]{ 128,192,320,740,999 };
                POINT ptMenu{ p->pt };
                ClientToScreen(pnm->hwndFrom, &ptMenu);
                const int nQuality = Menu.TrackPopupMenu(Handle, ptMenu.x, ptMenu.y,
                    TPM_RETURNCMD | TPM_RIGHTBUTTON) - IDMI_SEARCH_DL_BEGIN;
                if (nQuality < 0 || nQuality >= ARRAYSIZE(Quality))
                    return 0;
                const auto& vSel = m_LBNSearch.GetSelectionRange().GetList();
                int idxFlat = (int)m_vDownload.size();
                for (const auto& e : vSel)
                    for (int i = e.idxBegin; i <= e.idxEnd; ++i)
                    {
                        const auto& Item = m_vSearchRes[i];
                        auto pTsk = std::make_shared<DOWNLOAD_TASK>(
                            m_IdGen.Generate(), idxFlat++, Quality[nQuality]);
                        pTsk->rsText = Item.rsTitle;
                        pTsk->rsArtist = Item.rsArtist;
                        m_vDownload.emplace_back(pTsk);
                        pTsk->Task = TskDownload(Item, pTsk);
                    }
                m_LBNTask.SetItemCount((int)m_vDownload.size());
                m_LBNTask.Redraw();
            }
            return 0;
            }
        else if (pnm->hwndFrom == m_LBNLog.Handle)
            switch (pnm->code)
            {
            case NM_CUSTOMDRAW:
                return OnLBNCustomDrawLog((eck::NMCUSTOMDRAWEXT*)lParam);
            }
        else if (pnm->hwndFrom == m_LBNTask.Handle)
            switch (pnm->code)
            {
            case NM_CUSTOMDRAW:
                return OnLBNCustomDrawDownload((eck::NMCUSTOMDRAWEXT*)lParam);
            }
        else if (pnm->hwndFrom == m_CBBSource.Handle)
            switch (pnm->code)
            {
            case eck::NM_LBN_GETDISPINFO:
            {
                const auto p = (eck::NMLBNGETDISPINFO*)lParam;
                p->Item.pszText = Source[p->Item.idxItem].data();
                p->Item.cchText = (int)Source[p->Item.idxItem].size();
            }
            return TRUE;
            }
    }
    break;

    case WM_SIZE:
    {
        const auto d = (eck::TLytCoord)m_Ds.Padding;
        m_Lyt.Arrange(d, d, LOWORD(lParam) - d * 2, HIWORD(lParam) - d * 2);
    }
    return 0;

    case WM_CREATE:
        return HANDLE_WM_CREATE(Handle, wParam, lParam, OnCreate);

    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            if (lParam == (LPARAM)m_BTSearch.Handle ||
                lParam == (LPARAM)m_BTSearchId.Handle)
            {
                m_LBNSearch.SetItemCount(0);
                m_vSearchRes.clear();
                m_LBNSearch.Redraw();
                auto rsText = m_ED.GetText();
                if (rsText.IsEmpty())
                    return 0;
                TskSearch({
                    std::move(rsText),
                    (Src)m_CBBSource.GetListBox().GetCurrentSelection(),
                    lParam == (LPARAM)m_BTSearchId.Handle
                    });
            }
            else if (lParam == (LPARAM)m_BTAbout.Handle)
            {
                if (!m_pWndAbout)
                    m_pWndAbout = new CWindowAbout{};
                const auto pt = eck::CalculateCenterWindowPosition(
                    Handle, m_Ds.cxAbout, m_Ds.cyAbout);
                m_pWndAbout->Create(L"关于", WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 0,
                    pt.x, pt.y, m_Ds.cxAbout, m_Ds.cyAbout, Handle, NULL, NULL);
            }
            break;
        }
    }
    break;

    case WM_DESTROY:
        ClearRes();
        PostQuitMessage(0);
        return 0;

    case WM_DPICHANGED:
        UpdateDpi(HIWORD(wParam));
        eck::MsgOnDpiChanged(Handle, lParam);
        return 0;

    case WM_SETTINGCHANGE:
        eck::MsgOnSettingChangeMainWindow(Handle, wParam, lParam);
        break;
    }
    return CForm::OnMessage(uMsg, wParam, lParam);
}