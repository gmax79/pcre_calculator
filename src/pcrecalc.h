#pragma once

#include "pcreHelper.h"

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
public:
    void Attach(HWND static_control)
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
            std::wstring tmp;
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

template <WORD t_wDlgTemplateID>
class CAboutDlg : public CSimpleDialog<t_wDlgTemplateID, TRUE>
{
    Link m_link;
public:
    BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_RANGE_HANDLER(IDOK, IDNO, OnCloseCmd)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

private:
    LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        ExecuteDlgInit(t_wDlgTemplateID);
        m_link.Attach(GetDlgItem(IDC_STATIC_LINK));
        CenterWindow(GetParent());
        return 0;
    }
    LRESULT OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(m_hWnd, wID);
        return 0;
    }
};

class CPcreDlg : public CDialogImpl<CPcreDlg>
{
    std::wstring locale;
    Pcre16 m_pcre;
    CEdit  m_regexp, m_text;
    CEdit  m_result;
public:
	enum { IDD = IDD_MAINDLG };
	BEGIN_MSG_MAP(CPcreDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(ID_LANGUAGE_ENGLISH, OnEnInterface)
        COMMAND_ID_HANDLER(ID_LANGUAGE_RUSSIAN, OnRuInterface)
		COMMAND_ID_HANDLER(IDOK, OnEndDlg)
		COMMAND_ID_HANDLER(IDCANCEL, OnEndDlg)
        COMMAND_HANDLER(IDC_EDIT_REGEXP, EN_CHANGE, OnEditChanged)
        COMMAND_HANDLER(IDC_EDIT_STRING, EN_CHANGE, OnEditChanged)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        CenterWindow();
        HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	    SetIcon(hIcon, TRUE);
	    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	    SetIcon(hIconSmall, FALSE);

        m_regexp.Attach(GetDlgItem(IDC_EDIT_REGEXP));
        m_text.Attach(GetDlgItem(IDC_EDIT_STRING));
        m_result.Attach(GetDlgItem(IDC_EDIT_RESULT));

        WCHAR buffer[16];
        DWORD lcid = MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT);
        GetLocaleInfoW(lcid, LOCALE_SISO3166CTRYNAME, buffer, 15);
        locale.assign(buffer);
        std::transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
        if (locale == L"ru")
            setRussianInterface();
        else
            setEnglishInterface();        
        update();
        return TRUE;
    }

	LRESULT OnAppAbout(WORD, WORD, HWND, BOOL&)
    {
        if (locale == L"ru")
        {
           CAboutDlg<IDD_ABOUTBOX_RU> dlg;
           dlg.DoModal();
           return 0;
        }

        CAboutDlg<IDD_ABOUTBOX> dlg;
	    dlg.DoModal();
        return 0;
    }

	LRESULT OnEndDlg(WORD, WORD wID, HWND, BOOL&)
    {
        EndDialog(wID);
        return 0;
    }
    
    LRESULT OnEditChanged(WORD, WORD, HWND, BOOL&)
    {
        update();
        return 0;
    }

    LRESULT OnEnInterface(WORD, WORD wID, HWND, BOOL&)
    {
        setEnglishInterface();
        return 0;
    }

    LRESULT OnRuInterface(WORD, WORD wID, HWND, BOOL&)
    {
        setRussianInterface();
        return 0;
    }

    void setEnglishInterface()
    {
        SetMenu(LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU_EN)));
        ::SetWindowText(GetDlgItem(IDC_STATIC_REGEXP), L"Regular Expression:");
        ::SetWindowText(GetDlgItem(IDC_STATIC_STRING), L"Test String:");
        ::SetWindowText(GetDlgItem(IDC_STATIC_RESULT), L"Result:");
        locale = L"en";
    }

    void setRussianInterface()
    {
        SetMenu(LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU_RU)));
        ::SetWindowText(GetDlgItem(IDC_STATIC_REGEXP), L"Регулярное Выражение:");
        ::SetWindowText(GetDlgItem(IDC_STATIC_STRING), L"Исходный текст:");
        ::SetWindowText(GetDlgItem(IDC_STATIC_RESULT), L"Результат:");
        locale = L"ru";
    }

    void update()
    {
        std::wstring regexp, text;
        getWindowText(m_regexp, &regexp);
        getWindowText(m_text, &text);
        if (regexp.empty() || text.empty())
        {
            m_result.SetWindowText(L"");
            return;
        }
        if (!m_pcre.setRegExp(regexp, true))
        {
            m_result.SetWindowText(L"Error in regexp.");
            return;
        }

        m_pcre.find(text);
        int n = m_pcre.getSize();
        if (n == 0)
        {
            m_result.SetWindowText(L"No matches");
        }
        else
        {
            std::wstring result;
            for (int i=0; i<n; ++i)
            {
                std::wstring tmp;
                m_pcre.getString(i, &tmp);                
                
                WCHAR buffer[16];
                wsprintf(buffer, L"%d:[", i);
                result.append(buffer);
                result.append(tmp);
                result.append(L"]\r\n");
            }
            m_result.SetWindowText(result.c_str());
        }
    }
};
