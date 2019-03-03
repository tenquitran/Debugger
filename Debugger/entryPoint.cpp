#include "stdafx.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////////

using namespace DebuggerApp;

//////////////////////////////////////////////////////////////////////////


int _tmain(int argc, _TCHAR* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	// Redirect output from std::wcout and std::wcerr to log file.
	std::wofstream out("log.txt");
	std::wcout.rdbuf(out.rdbuf());
	std::wcerr.rdbuf(out.rdbuf());

	// TODO: temp
	Core dbgCore(L"C:\\Windows\\System32\\notepad.exe", true);

	return 0;
}
