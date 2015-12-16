#pragma once

#include "pcreHelper.h"
#include "link.h"

class CPcreDlg : public CDialogImpl<CPcreDlg>
{
    std::wstring locale;
    Pcre16 m_pcre;
    CEdit  m_regexp, m_text;
    CEdit  m_result;
    Link   m_link;
    CButton m_find;

public:
	enum { IDD = IDD_MAINDLG };
	BEGIN_MSG_MAP(CPcreDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnEndDlg)
		COMMAND_ID_HANDLER(IDCANCEL, OnEndDlg)
        COMMAND_HANDLER(IDC_EDIT_REGEXP, EN_CHANGE, OnEditChanged)
        COMMAND_HANDLER(IDC_EDIT_STRING, EN_CHANGE, OnEditChanged)
        COMMAND_ID_HANDLER(IDC_RADIO_FIND, OnEditChanged)
        COMMAND_ID_HANDLER(IDC_RADIO_FINDALL, OnEditChanged)
        REFLECT_NOTIFICATIONS()
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
        m_link.Attach(GetDlgItem(IDC_STATIC_LINK), L"https://github.com/gmax79/pcre_calculator");

        m_find.Attach(GetDlgItem(IDC_RADIO_FIND));
        m_find.SetCheck(BST_CHECKED);

        update();
        return TRUE;
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
            m_result.SetWindowText(L"Error in regular expression.");
            return;
        }

        if (m_find.GetCheck() == BST_CHECKED)
            m_pcre.find(text);
        else
            m_pcre.findAllMatches(text);

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
