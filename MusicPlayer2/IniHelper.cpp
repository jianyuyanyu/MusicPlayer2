﻿#include "stdafx.h"
#include "IniHelper.h"


CIniHelper::CIniHelper(const wstring& file_path)
{
    m_file_path = file_path;
    ifstream file_stream{ file_path };
    if (!file_stream.is_open())
        return;
    // 获取文件大小
    file_stream.seekg(0, std::ios::end);
    size_t file_size = static_cast<size_t>(file_stream.tellg());
    file_stream.seekg(0, std::ios::beg);
    // 读取文件内容
    string ini_str;
    ini_str.resize(file_size + 1);
    file_stream.read(&ini_str[0], file_size);
    // 检查并添加末尾的空行
    if (!ini_str.empty() && ini_str.back() != L'\n')
        ini_str.push_back(L'\n');
    // 转换成Unicode
    m_ini_str = CCommon::StrToUnicode(ini_str, CodeType::AUTO);
}

CIniHelper::CIniHelper(UINT id, CodeType code_type)
{
    m_ini_str = CCommon::GetTextResource(id, code_type);
}


CIniHelper::~CIniHelper()
{
}

void CIniHelper::SetSaveAsUTF8(bool utf8)
{
    m_save_as_utf8 = utf8;
}

void CIniHelper::WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str)
{
    wstring write_str{ str };
    if (!write_str.empty() && (write_str[0] == L' ' || write_str.back() == L' '))		//如果字符串前后含有空格，则在字符串前后添加引号
    {
        write_str = L'\"' + write_str;
        write_str.push_back(L'\"');
    }
    _WriteString(AppName, KeyName, write_str);
}

wstring CIniHelper::GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str) const
{
    wstring rtn{ _GetString(AppName, KeyName, default_str) };
    //如果读取的字符串前后有引号，则删除它
    if (!rtn.empty() && rtn.front() == L'\"')
        rtn = rtn.substr(1);
    if (!rtn.empty() && rtn.back() == L'\"')
        rtn.pop_back();
    return rtn;
}

void CIniHelper::WriteInt(const wchar_t* AppName, const wchar_t* KeyName, int value)
{
    wchar_t buff[16]{};
    _itow_s(value, buff, 10);
    _WriteString(AppName, KeyName, wstring(buff));
}

int CIniHelper::GetInt(const wchar_t* AppName, const wchar_t* KeyName, int default_value) const
{
    wchar_t default_str_buff[16]{};
    _itow_s(default_value, default_str_buff, 10);
    wstring rtn{ _GetString(AppName, KeyName, default_str_buff) };
    if (rtn == L"true")
        return 1;
    else if (rtn == L"false")
        return 0;
    else
        return _ttoi(rtn.c_str());
}

void CIniHelper::WriteDouble(const wchar_t* AppName, const wchar_t* KeyName, double value)
{
    _WriteString(AppName, KeyName, std::to_wstring(value));
}

double CIniHelper::GetDouble(const wchar_t* AppName, const wchar_t* KeyName, double default_value) const
{
    wstring rtn{ _GetString(AppName, KeyName, std::to_wstring(default_value).c_str()) };
    return _wtof(rtn.c_str());
}

void CIniHelper::WriteBool(const wchar_t* AppName, const wchar_t* KeyName, bool value)
{
    if (value)
        _WriteString(AppName, KeyName, wstring(L"true"));
    else
        _WriteString(AppName, KeyName, wstring(L"false"));
}

bool CIniHelper::GetBool(const wchar_t* AppName, const wchar_t* KeyName, bool default_value) const
{
    wstring rtn{ _GetString(AppName, KeyName, (default_value ? L"true" : L"false")) };
    if (rtn == L"true")
        return true;
    else if (rtn == L"false")
        return false;
    else
        return (_ttoi(rtn.c_str()) != 0);
}

void CIniHelper::WriteIntArray(const wchar_t* AppName, const wchar_t* KeyName, const int* values, int size)
{
    CString str, tmp;
    for (int i{}; i < size; i++)
    {
        tmp.Format(_T("%d,"), values[i]);
        str += tmp;
    }
    _WriteString(AppName, KeyName, wstring(str));
}

void CIniHelper::GetIntArray(const wchar_t* AppName, const wchar_t* KeyName, int* values, int size, int default_value) const
{
    CString default_str;
    default_str.Format(_T("%d"), default_value);
    wstring str;
    str = _GetString(AppName, KeyName, default_str);
    size_t index{}, index0{};
    for (int i{}; i < size; i++)
    {
        index0 = index;
        if (index0 < 256 && str[index0] == L',')
            index0++;
        index = str.find(L',', index + 1);
        if (index0 == index)
        {
            if (i != 0)
                values[i] = values[i - 1];		//如果后面已经没有数据，则填充为前一个数据
            else
                values[i] = default_value;
        }
        else
        {
            wstring tmp = str.substr(index0, index - index0);
            values[i] = _wtoi(tmp.c_str());
        }
    }
}

void CIniHelper::WriteBoolArray(const wchar_t* AppName, const wchar_t* KeyName, const bool* values, int size)
{
    int value{};
    for (int i{}; i < size; i++)
    {
        if (values[i])
            value |= (1 << i);
    }
    return WriteInt(AppName, KeyName, value);
}

void CIniHelper::GetBoolArray(const wchar_t* AppName, const wchar_t* KeyName, bool* values, int size, bool default_value) const
{
    int value = GetInt(AppName, KeyName, 0);
    for (int i{}; i < size; i++)
    {
        values[i] = ((value >> i) % 2 != 0);
    }
}

void CIniHelper::WriteStringList(const wchar_t* AppName, const wchar_t* KeyName, const vector<wstring>& values)
{
    wstring str_write{ CCommon::MergeStringList(values) };
    _WriteString(AppName, KeyName, str_write);
}

void CIniHelper::GetStringList(const wchar_t* AppName, const wchar_t* KeyName, vector<wstring>& values, const vector<wstring>& default_values) const
{
    wstring default_str{ CCommon::MergeStringList(default_values) };
    wstring str_value{ _GetString(AppName, KeyName, default_str.c_str()) };
    CCommon::SplitStringList(values, str_value);
}

CVariant CIniHelper::GetValue(const wchar_t* AppName, const wchar_t* KeyName, CVariant default_value) const
{
    wstring str_value = GetString(AppName, KeyName, default_value.ToString());
    return CVariant(str_value);
}

void CIniHelper::WriteValue(const wchar_t* AppName, const wchar_t* KeyName, CVariant value)
{
    WriteString(AppName, KeyName, value.ToString().GetString());
}


vector<wstring> CIniHelper::GetAllAppName(const wstring& prefix) const
{
    vector<wstring> list;
    size_t pos{};
    while ((pos = m_ini_str.find(L"\n[" + prefix, pos)) != wstring::npos)
    {
        size_t end = m_ini_str.find(L']', pos + 1);
        if (end != wstring::npos)
        {
            wstring tmp(m_ini_str.begin() + pos + prefix.size() + 2, m_ini_str.begin() + end);
            list.push_back(std::move(tmp));
            pos = end + 1;
        }
    }
    return list;
}

void CIniHelper::GetAllKeyValues(const wstring& AppName, std::map<wstring,wstring>& map) const
{
    wstring app_str{ L"[" };
    app_str.append(AppName).append(L"]");
    size_t app_pos{}, app_end_pos{};
    app_pos = m_ini_str.find(app_str);
    if (app_pos == wstring::npos)
        return;
    app_end_pos = m_ini_str.find(L"\n[", app_pos + 2);
    if (app_end_pos != wstring::npos)
        app_end_pos++;
    app_str = m_ini_str.substr(app_pos, app_end_pos - app_pos);
    vector<wstring> line;
    CCommon::StringSplit(app_str, L'\n', line);
    for (wstring str : line)
    {
        // CCommon::StringSplit会跳过空字符串，str一定非空
        if (str[0] == L';' || str[0] == L'#')   // 跳过注释行（只支持行首注释）
            continue;
        size_t pos = str.find_first_of(L'=');
        if (pos == wstring::npos)
            continue;
        wstring key{ str.substr(0, pos) };
        wstring value{ str.substr(pos + 1) };
        CCommon::StringNormalize(key);
        CCommon::StringNormalize(value);
        if (!key.empty() && !value.empty())
        {
            if (value.front() == L'\"' && value.back() == L'\"')
                value = value.substr(1, value.size() - 2);
            UnEscapeString(value);
            map[key] = value;
        }
    }
}

bool CIniHelper::Save()
{
    if (m_file_path.empty())
        return false;
    ofstream file_stream{ m_file_path };
    if (file_stream.fail())
        return false;
    string ini_str{ CCommon::UnicodeToStr(m_ini_str, m_save_as_utf8 ? CodeType::UTF8 : CodeType::ANSI) };
    file_stream << ini_str;
    return true;
}

void CIniHelper::UnEscapeString(wstring& str)
{
    bool escape{ false };
    wstring result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); i++)
    {
        wchar_t ch = str[i];
        if (escape)
        {
            switch (ch)
            {
            case L'n': result += L'\n'; break;
            case L'r': result += L'\r'; break;
            case L't': result += L'\t'; break;
            case L'"': result += L'"'; break;
            case L';': result += L';'; break;
            case L'#': result += L'#'; break;
            case L'\\': result += L'\\'; break;
            default:result += '\\'; result += ch; break;
            }
            escape = false;
        }
        else if (ch == L'\\')
            escape = true;
        else if (i > 0 && ch == '\"' && str[i - 1] == '\"')     //两个连续的引号只保留一个引号
            continue;
        else
            result += ch;
    }
    str.swap(result);
}

void CIniHelper::_WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str)
{
    wstring app_str{ L"[" };
    app_str.append(AppName).append(L"]");
    size_t app_pos{}, app_end_pos, key_pos;
    app_pos = m_ini_str.find(app_str);
    if (app_pos == wstring::npos)		//找不到AppName，则在最后面添加
    {
        if (!m_ini_str.empty() && m_ini_str.back() != L'\n')
            m_ini_str += L"\n";
        app_pos = m_ini_str.size();
        m_ini_str += app_str;
        m_ini_str += L"\n";
    }
    app_end_pos = m_ini_str.find(L"\n[", app_pos + 2);
    if (app_end_pos != wstring::npos)
        app_end_pos++;

    key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L' ', app_pos);		//查找“\nkey_name ”
    if (key_pos >= app_end_pos)		//如果找不到“\nkey_name ”，则查找“\nkey_name=”
        key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L'=', app_pos);
    if (key_pos >= app_end_pos)				//找不到KeyName，则插入一个
    {
        //wchar_t buff[256];
        //swprintf_s(buff, L"%s = %s\n", KeyName, str.c_str());
        std::wstring str_temp = KeyName;
        str_temp += L" = ";
        str_temp += str;
        str_temp += L"\n";
        if (app_end_pos == wstring::npos)
            m_ini_str += str_temp;
        else
            m_ini_str.insert(app_end_pos, str_temp);
    }
    else	//找到了KeyName，将等号到换行符之间的文本替换
    {
        size_t str_pos;
        str_pos = m_ini_str.find(L'=', key_pos + 2);
        size_t line_end_pos = m_ini_str.find(L'\n', key_pos + 2);
        if (str_pos > line_end_pos)	//所在行没有等号，则插入一个等号
        {
            m_ini_str.insert(key_pos + wcslen(KeyName) + 1, L" =");
            str_pos = key_pos + wcslen(KeyName) + 2;
        }
        else
        {
            str_pos++;
        }
        size_t str_end_pos;
        str_end_pos = m_ini_str.find(L"\n", str_pos);
        m_ini_str.replace(str_pos, str_end_pos - str_pos, L" " + str);
    }
}

wstring CIniHelper::_GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str) const
{
    wstring app_str{ L"[" };
    app_str.append(AppName).append(L"]");
    size_t app_pos{}, app_end_pos, key_pos;
    app_pos = m_ini_str.find(app_str);
    if (app_pos == wstring::npos)		//找不到AppName，返回默认字符串
        return default_str;

    app_end_pos = m_ini_str.find(L"\n[", app_pos + 2);
    if (app_end_pos != wstring::npos)
        app_end_pos++;

    key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L' ', app_pos);		//查找“\nkey_name ”
    if (key_pos >= app_end_pos)		//如果找不到“\nkey_name ”，则查找“\nkey_name=”
        key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L'=', app_pos);
    if (key_pos >= app_end_pos)				//找不到KeyName，返回默认字符串
    {
        return default_str;
    }
    else	//找到了KeyName，获取等号到换行符之间的文本
    {
        size_t str_pos;
        str_pos = m_ini_str.find(L'=', key_pos + 2);
        size_t line_end_pos = m_ini_str.find(L'\n', key_pos + 2);
        if (str_pos > line_end_pos)	//所在行没有等号，返回默认字符串
        {
            return default_str;
        }
        else
        {
            str_pos++;
        }
        size_t str_end_pos;
        str_end_pos = m_ini_str.find(L"\n", str_pos);
        //获取文本
        wstring return_str{ m_ini_str.substr(str_pos, str_end_pos - str_pos) };
        //如果前后有空格，则将其删除
        CCommon::StringNormalize(return_str);
        return return_str;
    }
}
