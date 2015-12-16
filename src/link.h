#pragma once

void getWindowText(HWND handle, std::wstring *string)
{
    int text_len = ::GetWindowTextLength(handle);
    WCHAR *buffer = new WCHAR[text_len+1];
    ::GetWindowText(handle, buffer, text_len+1);
    string->assign(buffer);
    delete []buffer;
}

class Link : public CWindowImpl<Link, CStatic>
{
    std::wstring m_url;
public:
    void Attach(HWND static_control, const std::wstring& url)
    {
        CWindow s(static_control);
        RECT pos;
        s.GetWindowRect(&pos);
        CWindow p(s.GetParent());
        p.ScreenToClient(&pos);
        std::wstring tmp;
        getWindowText(static_control, &tmp);
        HFONT f = s.GetFont();
        s.ShowWindow(SW_HIDE);
        Create(p, pos, tmp.c_str(), WS_CHILD|WS_VISIBLE|SS_NOTIFY);       
        SetFont(f);
        m_url = url;
    }

private:
    BEGIN_MSG_MAP(Link)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
        MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor )
    END_MSG_MAP()

    LRESULT OnSetCursor(UINT, WPARAM, LPARAM, BOOL&)
    {
        static HCURSOR hHand = NULL;
        if (hHand == NULL)
            hHand = LoadCursor(0,  MAKEINTRESOURCE(IDC_HAND));
        if (hHand)
            ::SetCursor(hHand);
        return 1;
    };

    LRESULT OnCommand( UINT, WPARAM wParam, LPARAM, BOOL& ) 
    {
        int code = HIWORD( wParam );
        if ( code == STN_CLICKED )
        {
            std::wstring tmp(m_url);
            if (tmp.empty())
                getWindowText(m_hWnd, &tmp);
            std::wstring url(L"url.dll,FileProtocolHandler ");
            url.append(tmp);
            ShellExecute(NULL, L"open", L"rundll32.exe", url.c_str(), 0, SW_SHOWNORMAL);            
        }
        return 0;
    }

    LRESULT OnCtlColor( UINT, WPARAM wParam, LPARAM, BOOL& )
    {
        HDC hdc = (HDC)wParam;
        SetTextColor( hdc, RGB(0,0,255));
        SetBkMode( hdc, TRANSPARENT );
        return (LRESULT)GetStockObject( NULL_BRUSH );
    }
};
