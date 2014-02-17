#include "stdafx.h"
#include "resource.h"
#include "pcrecalc.h"

CAppModule _Module;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	_Module.Init(NULL, hInstance);
	int nRet = 0;
	{
		CPcreDlg dlgMain;
		nRet = dlgMain.DoModal();
	}
	_Module.Term();
	return nRet;
}
