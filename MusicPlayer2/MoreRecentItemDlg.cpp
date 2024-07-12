﻿// AddToPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MoreRecentItemDlg.h"
#include "FilePathHelper.h"
#include "MediaLibPlaylistMgr.h"


// CMoreRecentItemDlg 对话框

IMPLEMENT_DYNAMIC(CMoreRecentItemDlg, CBaseDialog)

CMoreRecentItemDlg::CMoreRecentItemDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_ADD_TO_PLAYLIST_DIALOG, pParent)
{

}

CMoreRecentItemDlg::~CMoreRecentItemDlg()
{
}

const CRecentFolderAndPlaylist::Item* CMoreRecentItemDlg::GetSelectedItem() const
{
    auto& data_list{ m_searched ? m_search_result : CRecentFolderAndPlaylist::Instance().GetItemList() };
    if (m_selected_item >= 0 && m_selected_item < static_cast<int>(data_list.size()))
        return &data_list.at(m_selected_item);
    return nullptr;
}

CString CMoreRecentItemDlg::GetDialogName() const
{
    return _T("MoreRecentItemDlg");
}

bool CMoreRecentItemDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_MORE_RECENT_ITEM");
    SetWindowTextW(temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CMoreRecentItemDlg::ShowList()
{
    m_list_ctrl.DeleteAllItems();
    auto& data_list{ m_searched ? m_search_result : CRecentFolderAndPlaylist::Instance().GetItemList() };
    for (const auto& item : data_list)
    {
        m_list_ctrl.AddString(item.GetName().c_str());
        //设置图标
        IconMgr::IconType icon_type{};
        if (item.IsFolder())
            icon_type = IconMgr::IT_Folder;
        else if (item.IsPlaylist())
            icon_type = IconMgr::IT_Playlist;
        else if (item.medialib_info != nullptr)
            icon_type = CMediaLibPlaylistMgr::GetIcon(item.medialib_info->medialib_type);
        m_list_ctrl.SetItemIcon(m_list_ctrl.GetItemCount() - 1, theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IS_OutlinedDark));
    }
}

void CMoreRecentItemDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CMoreRecentItemDlg, CBaseDialog)
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMoreRecentItemDlg::OnNMDblclkList1)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CMoreRecentItemDlg::OnEnChangeSearchEdit)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CMoreRecentItemDlg::OnSearchEditBtnClicked)
END_MESSAGE_MAP()


// CMoreRecentItemDlg 消息处理程序


BOOL CMoreRecentItemDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    SetIcon(IconMgr::IconType::IT_Media_Lib, FALSE);     // 设置小图标
    m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT").c_str(), TRUE);

    //初始化列表
    m_list_ctrl.SetRowHeight(theApp.DPI(24), theApp.DPI(18));
    ShowList();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CMoreRecentItemDlg::OnOK()
{
    m_selected_item = m_list_ctrl.GetCurSel();

    CBaseDialog::OnOK();
}


void CMoreRecentItemDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_selected_item = pNMItemActivate->iItem;

    CBaseDialog::OnOK();

    *pResult = 0;
}


void CMoreRecentItemDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CBaseDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    m_search_edit.GetWindowText(str);
    QuickSearch(wstring(str));
    m_searched = !str.IsEmpty();
    ShowList();
}


afx_msg LRESULT CMoreRecentItemDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    CSearchEditCtrl* pEdit = (CSearchEditCtrl*)wParam;
    if (pEdit == &m_search_edit)
    {
        if (m_searched)
        {
            //清除搜索结果
            m_searched = false;
            m_search_edit.SetWindowText(_T(""));
            ShowList();
        }
    }
    return 0;
}

void CMoreRecentItemDlg::QuickSearch(const wstring& key_word)
{
    m_search_result.clear();
    for (const auto& item : CRecentFolderAndPlaylist::Instance().GetItemList())
    {
        std::wstring str = item.GetName();
        if (CCommon::StringFindNoCase(str, key_word) != wstring::npos)
        {
            m_search_result.push_back(item);
        }
    }
}