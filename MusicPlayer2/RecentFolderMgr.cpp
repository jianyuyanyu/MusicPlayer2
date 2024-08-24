﻿#include "stdafx.h"
#include "RecentFolderMgr.h"
#include "MusicPlayer2.h"


CRecentFolderMgr CRecentFolderMgr::m_instance;

CRecentFolderMgr::~CRecentFolderMgr()
{
}

CRecentFolderMgr& CRecentFolderMgr::Instance()
{
    return m_instance;
}

bool CRecentFolderMgr::SetSortMode(FolderSortMode sort_mode)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    if (m_sort_mode != sort_mode)
    {
        m_sort_mode = sort_mode;
        SortPath();
        return true;
    }
    return false;
}

CRecentFolderMgr::FolderSortMode CRecentFolderMgr::GetSortMode() const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    return m_sort_mode;
}

void CRecentFolderMgr::EmplaceRecentFolder(const std::wstring& path, int track, int position, SortMode sort_mode, int track_num, int totla_time, bool contain_sub_folder)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path == path_info.path; });

    PathInfo path_info;
    path_info.path = path;
    path_info.track = track;
    path_info.position = position;
    path_info.sort_mode = sort_mode;
    path_info.track_num = track_num;
    path_info.total_time = totla_time;
    path_info.contain_sub_folder = contain_sub_folder;
    path_info.last_played_time = CCommon::GetCurTimeElapse();

    //如果当前路径已经在最近路径中，则更新已经存在的路径
    if (iter != m_recent_path.end())
    {
        if (track_num > 0)
        {
            path_info.add_time = iter->add_time;
            *iter = path_info;
        }
        else
        {
            m_recent_path.erase(iter);
        }
    }
    //当前路径不在最近路径中，则添加
    else
    {
        if (track_num > 0)      //仅当路径中有文件时才保存
        {
            path_info.add_time = CCommon::GetCurTimeElapse();
            m_recent_path.push_back(path_info);
        }
    }

    //更新文件夹顺序
    SortPath();
}

PathInfo CRecentFolderMgr::GetCurrentListInfo() const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    if (!m_recent_path.empty())
    {
        //查找播放时间最近的路径
        const PathInfo* latest_path{ &m_recent_path.front() };
        for (const auto& path_info : m_recent_path)
        {
            if (path_info.last_played_time > latest_path->last_played_time)
                latest_path = &path_info;
        }
        return *latest_path;
    }
    return PathInfo();
}

bool CRecentFolderMgr::RemoveItem(const std::wstring& path)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path_info.path == path; });
    if (iter != m_recent_path.end())
    {
        m_recent_path.erase(iter);        // 删除选中的路径
        return true;
    }
    return false;
}

int CRecentFolderMgr::RemoveItemIf(std::function<bool(const PathInfo&)> fun_condition)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    int removedCount{};
    auto iter = m_recent_path.begin();
    while (iter != m_recent_path.end())
    {
        if (fun_condition(*iter))
        {
            iter = m_recent_path.erase(iter);
            ++removedCount;
        }
        else
            ++iter;
    }
    return removedCount;
}

bool CRecentFolderMgr::SetContainSubFolder(const wstring& path)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path_info.path == path; });
    if (iter != m_recent_path.end())
    {
        m_recent_path.erase(iter);        // 删除选中的路径
        return true;
    }
    return false;
}

bool CRecentFolderMgr::ResetLastPlayedTime(const std::wstring& path)
{
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path_info.path == path; });
    if (iter != m_recent_path.end())
    {
        iter->last_played_time = 0;
        return true;
    }
    return false;
}

int CRecentFolderMgr::GetItemSize() const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    return static_cast<int>(m_recent_path.size());
}

PathInfo CRecentFolderMgr::GetItem(const wstring& path) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    auto iter = std::find_if(m_recent_path.begin(), m_recent_path.end(),
        [&](const PathInfo& path_info) { return path_info.path == path; });
    if (iter != m_recent_path.end())
        return *iter;
    return PathInfo();
}

void CRecentFolderMgr::IteratePathInfo(std::function<void(const PathInfo&)> func) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    for (const auto& path_info : m_recent_path)
    {
        func(path_info);
    }
}

PathInfo CRecentFolderMgr::GetItem(int index) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    if (index >= 0 && index < static_cast<int>(m_recent_path.size()))
    {
        return m_recent_path[index];
    }
    return PathInfo();
}

void CRecentFolderMgr::GetItem(int index, std::function<void(const PathInfo&)> func) const
{
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    if (index >= 0 && index < static_cast<int>(m_recent_path.size()))
    {
         func(m_recent_path[index]);
    }
}

bool CRecentFolderMgr::LoadData()
{
    // 打开文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(), CFile::modeRead);
    if (!bRet)      //文件不存在
    {
        return false;
    }
    std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
    //为了保持和以前版本的数据兼容，先读取前8个字节，以判断是否是以前版本
    char buff[8]{};
    file.Read(buff, 8);
    file.SeekToBegin();

    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    unsigned int size{};
    CString temp;
    int sort_mode;
    unsigned int version{};
    try
    {
        ar >> size;		//读取映射容器的长度
        if (buff[4] == '\xff' && buff[5] == '\xfe')     //如果第4个字节和第5个字节是FFFE，则说明数据文件是以前版本，此时不读取version
            version = 0;
        else
            ar >> version;  //读取数据文件的版本
        for (unsigned int i{}; i < size; i++)
        {
            PathInfo path_info;
            ar >> temp;
            path_info.path = temp;
            ar >> path_info.track;
            ar >> path_info.position;
            ar >> sort_mode;
            path_info.sort_mode = static_cast<SortMode>(sort_mode);
            if (version >= 2 && version <= 3)   // 在版本4变量descending不再独立存储，功能整合到sort_mode内，这里做读取兼容
            {
                BYTE descending;
                ar >> descending;
                switch (sort_mode)
                {
                case 0: path_info.sort_mode = descending ? SM_D_FILE : SM_U_FILE; break;
                case 1: path_info.sort_mode = descending ? SM_D_PATH : SM_U_PATH; break;
                case 2: path_info.sort_mode = descending ? SM_D_TITLE : SM_U_TITLE; break;
                case 3: path_info.sort_mode = descending ? SM_D_ARTIST : SM_U_ARTIST; break;
                case 4: path_info.sort_mode = descending ? SM_D_ALBUM : SM_U_ALBUM; break;
                case 5: path_info.sort_mode = descending ? SM_D_TRACK : SM_U_TRACK; break;
                case 6: path_info.sort_mode = descending ? SM_D_TIME : SM_U_TIME; break;
                default: break;
                }
            }
            ar >> path_info.track_num;
            ar >> path_info.total_time;
            if (version >= 1)
            {
                BYTE contain_sub_folder;
                ar >> contain_sub_folder;
                path_info.contain_sub_folder = (contain_sub_folder != 0);
            }
            if (version >= 3)
                ar >> path_info.last_played_time;
            if (version >= 5)
                ar >> path_info.add_time;
            if (path_info.add_time == 0)
                path_info.add_time = path_info.last_played_time;     //没有读取到添加时间，则将上次播放时间作为添加时间

            if (path_info.path.empty() || path_info.path.size() < 2) continue;		//如果路径为空或路径太短，就忽略它
            if (path_info.path.back() != L'/' && path_info.path.back() != L'\\')	//如果读取到的路径末尾没有斜杠，则在末尾加上一个
                path_info.path.push_back(L'\\');
            m_recent_path.push_back(std::move(path_info));
        }
    }
    catch (CArchiveException* exception)
    {
        //捕获序列化时出现的异常
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_path_dat_path, exception->m_cause });
        theApp.WriteLog(info);
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();
    return true;
}

void CRecentFolderMgr::SaveData() const
{
    // 打开或者新建文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_path_dat_path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)      //打开文件失败
    {
        return;
    }
    std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写数据
    const unsigned int version{ 5u };
    ar << static_cast<unsigned int>(m_recent_path.size());		//写入m_recent_path容器的大小
    ar << version;     //写入文件的版本
    for (const auto& path_info : m_recent_path)
    {
        ar << CString(path_info.path.c_str())
            << path_info.track
            << path_info.position
            << static_cast<int>(path_info.sort_mode)
            << path_info.track_num
            << path_info.total_time
            << static_cast<BYTE>(path_info.contain_sub_folder)
            << path_info.last_played_time
            << path_info.add_time
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();
}


CRecentFolderMgr::CRecentFolderMgr()
{
}

void CRecentFolderMgr::SortPath()
{
    if (m_recent_path.size() > 1)
    {
        switch (m_sort_mode)
        {
        case CRecentFolderMgr::FolderSortMode::SM_RECENT_PLAYED:
            std::stable_sort(m_recent_path.begin(), m_recent_path.end(), [](const PathInfo& a, const PathInfo& b) { return a.last_played_time > b.last_played_time; });
            break;
        case CRecentFolderMgr::FolderSortMode::SM_RECENT_ADDED:
            std::stable_sort(m_recent_path.begin(), m_recent_path.end(), [](const PathInfo& a, const PathInfo& b) { return a.add_time > b.add_time; });
            break;
        case CRecentFolderMgr::FolderSortMode::SM_PATH:
            std::stable_sort(m_recent_path.begin(), m_recent_path.end(), [](const PathInfo& a, const PathInfo& b) { return a.path < b.path; });
            break;
        default:
            break;
        }
    }
}
