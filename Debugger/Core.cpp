#include "stdafx.h"
#include "Core.h"

//////////////////////////////////////////////////////////////////////////

using namespace DebuggerApp;

//////////////////////////////////////////////////////////////////////////


Core::Core(const CAtlString& targetPath, bool createTarget, DWORD targetPid /*= {}*/)
	: m_debuggee(targetPath, targetPid)
{
	if (!createTarget && targetPid < 1)
	{
		ATLASSERT(FALSE); throw EXCEPTION(L"Invalid target process ID");
	}

	bool res = createTarget ? createTargetProcess() : attachToTarget();

	if (!res)
	{
		ATLASSERT(FALSE); throw EXCEPTION_FMT(L"%s failed (%s)", (createTarget ? L"Creation" : L"Attachment"), (LPCTSTR)targetPath);
	}
}

Core::~Core()
{
}

bool Core::createTargetProcess()
{
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);
	// TODO: you can specify the standard input, output and error handles, etc.

	// After the CreateProcess call, PROCESS_INFORMATION will contain the process and thread IDs and handles.
	PROCESS_INFORMATION pi = {};

	const CAtlString targetPath = m_debuggee.getImagePath();

	const CAtlString currDirectory = m_debuggee.getCurrentDirectory();

	const DWORD creationFlags = DEBUG_PROCESS;

	if (!CreateProcess(
			targetPath, nullptr, nullptr, nullptr, FALSE, creationFlags, nullptr,
			(currDirectory.IsEmpty() ? nullptr : currDirectory.GetString()), &si, &pi))
	{
		std::wcerr << L"CreateProcess() failed: " << GetLastError() << '\n';
		ATLASSERT(FALSE); return false;
	}

	m_debuggee.setPID(pi.dwProcessId);

	// Enter the debugger main loop.

	DEBUG_EVENT dbgEvent = {};
	debugLoop(&dbgEvent);

	return true;
}

bool Core::attachToTarget()
{
	// TODO: implement
	ATLASSERT(FALSE);
	return false;
}

void Core::debugLoop(const LPDEBUG_EVENT pDbgEvent)
{
	DWORD continueStatus = DBG_CONTINUE;    // continue execution

	for (;;)
	{
		// Wait for a debugging event to occur.
		WaitForDebugEvent(pDbgEvent, INFINITE);

		// Process the debugging event code. 
		switch (pDbgEvent->dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			continueStatus = onException(pDbgEvent);
			break;
		case CREATE_PROCESS_DEBUG_EVENT:
			continueStatus = onCreateProcess(pDbgEvent);
			break;
		case CREATE_THREAD_DEBUG_EVENT:
			continueStatus = onCreateThread(pDbgEvent);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			continueStatus = onExitProcess(pDbgEvent);

			if (!DebugActiveProcessStop(pDbgEvent->dwProcessId))
			{
				std::wcerr << L"DebugActiveProcessStop() failed: " << GetLastError() << '\n';
				ATLASSERT(FALSE);
			}
			return;
		case EXIT_THREAD_DEBUG_EVENT:
			continueStatus = onExitThread(pDbgEvent);
			break;
		case LOAD_DLL_DEBUG_EVENT:
			continueStatus = onLoadDll(pDbgEvent);
			break;
		case UNLOAD_DLL_DEBUG_EVENT:
			continueStatus = onUnloadDll(pDbgEvent);
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			continueStatus = onOutputDebugString(pDbgEvent);
			break;
		case RIP_EVENT:
			continueStatus = onRip(pDbgEvent);
			break;
		}

		// Resume the thread execution.
		ContinueDebugEvent(pDbgEvent->dwProcessId, pDbgEvent->dwThreadId, continueStatus);
	}
}

DWORD Core::onException(const LPDEBUG_EVENT pDbgEvent)
{
	// Process the exception code. 
	// When handling exceptions, remember to set the continuation status parameter (dwContinueStatus).
	// This value is used by ContinueDebugEvent().
	switch (pDbgEvent->u.Exception.ExceptionRecord.ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		// First chance: Pass this on to the system. 
		// Last chance: Display an appropriate error.

		// TODO: implement
		ATLASSERT(FALSE);
		break;
	case EXCEPTION_BREAKPOINT:
		// First chance: Display the current instruction and register values.

		// TODO: implement
		//ATLASSERT(FALSE);
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		// First chance: Pass this on to the system. 
		// Last chance: Display an appropriate error.

		// TODO: implement
		ATLASSERT(FALSE);
		break;
	case EXCEPTION_SINGLE_STEP:
		// First chance: Update the display of the current instruction and register values.

		// TODO: implement
		ATLASSERT(FALSE);
		break;
	case DBG_CONTROL_C:
		// First chance: Pass this on to the system. 
		// Last chance: Display an appropriate error.

		// TODO: implement
		ATLASSERT(FALSE);
		break;
	default:
		// Handle other exceptions.

		// TODO: implement
		//ATLASSERT(FALSE);

		// Returning DBG_CONTINUE here can make the debuggee unresponsive.
		return DBG_EXCEPTION_NOT_HANDLED;
	}

	return DBG_CONTINUE;
}

DWORD Core::onCreateProcess(const LPDEBUG_EVENT pDbgEvent)
{
	// Examine or change the registers of the process's initial thread with GetThreadContext() and SetThreadContext(); 
	// read from and write to the process's virtual memory with ReadProcessMemory() and WriteProcessMemory();
	// suspend and resume thread execution with SuspendThread() and ResumeThread().
	// Be sure to close the handle to the process image file with CloseHandle().

	LPCREATE_PROCESS_DEBUG_INFO pProcess = &pDbgEvent->u.CreateProcessInfo;

	m_debuggee.setProcessInfo(pProcess);

	std::wcout << L"Process " << pDbgEvent->dwProcessId << L" created" << std::endl;
	return DBG_CONTINUE;
}

DWORD Core::onCreateThread(const LPDEBUG_EVENT pDbgEvent)
{
	// Examine or change the thread's registers with GetThreadContext() and SetThreadContext(); 
	// and suspend and resume thread execution with SuspendThread() and ResumeThread().

	std::wcout << L"Thread " << pDbgEvent->dwThreadId << L" created" << std::endl;

	m_debuggee.addThreadInfo(pDbgEvent->u.CreateThread.hThread);

	return DBG_CONTINUE;
}

DWORD Core::onExitProcess(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the process exit code.

    std::wcout << L"Process " << pDbgEvent->dwProcessId << L" exit: code " << pDbgEvent->u.ExitProcess.dwExitCode << std::endl;

	//CloseHandle(m_debuggee.getProcessHandle());

	return DBG_CONTINUE;
}

DWORD Core::onExitThread(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the thread exit code.

    std::wcout << L"Thread " << pDbgEvent->dwThreadId << L" exit: code " << pDbgEvent->u.ExitThread.dwExitCode << std::endl;
	return DBG_CONTINUE;
}

DWORD Core::onLoadDll(const LPDEBUG_EVENT pDbgEvent)
{
	// Read the debugging information included in the newly loaded DLL. 
	// Be sure to close the handle to the loaded DLL with CloseHandle().

	LPLOAD_DLL_DEBUG_INFO pLoadDllInfo = &pDbgEvent->u.LoadDll;

	HANDLE hDll = pLoadDllInfo->hFile;

	if (!hDll)
	{
		return DBG_CONTINUE;
	}

	CAtlString dllPath;

	DWORD res = GetFinalPathNameByHandle(hDll, dllPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH, 0);
	dllPath.ReleaseBuffer();

	if (!res)
	{
		std::wcerr << __FUNCTIONW__ << L": GetFinalPathNameByHandle() failed: " << GetLastError() << '\n';
	}
	else
	{
		dllPath.TrimLeft(L"\\?");

		ATLASSERT(pLoadDllInfo->lpBaseOfDll);
		m_debuggee.addDllInfo(pLoadDllInfo->lpBaseOfDll, dllPath);

		std::wcout << "DLL loaded: " << dllPath.GetString() << std::endl;
	}

	if (!CloseHandle(hDll))
	{
		std::wcerr << __FUNCTIONW__ << L": CloseHandle() failed: " << GetLastError() << '\n';
	}

	return DBG_CONTINUE;
}

DWORD Core::onUnloadDll(const LPDEBUG_EVENT pDbgEvent)
{
	// Display a message that the DLL has been unloaded.

	LPVOID pBaseAddress = pDbgEvent->u.UnloadDll.lpBaseOfDll;
	ATLASSERT(pBaseAddress);

	CAtlString dllPath = m_debuggee.findDllPath(pBaseAddress);

	if (dllPath.IsEmpty())
	{
		std::wcerr << L"No DLL found at the base address " << pBaseAddress << '\n';
	}
	else
	{
		std::wcout << "DLL unloaded: " << dllPath.GetString() << std::endl;
	}

	return DBG_CONTINUE;
}

DWORD Core::onOutputDebugString(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the output debugging string.

	LPOUTPUT_DEBUG_STRING_INFO pDbgStr = &pDbgEvent->u.DebugString;

	SIZE_T cbRead = {};

	if (pDbgStr->fUnicode)
	{
		std::unique_ptr<WCHAR[]> spBuff = std::make_unique<WCHAR[]>(pDbgStr->nDebugStringLength + 1);

		if (   !ReadProcessMemory(m_debuggee.getProcessHandle(), pDbgStr->lpDebugStringData, spBuff.get(), 
								  pDbgStr->nDebugStringLength * sizeof(WCHAR), &cbRead)
			|| 0 == cbRead)
		{
			std::wcerr << __FUNCTIONW__ << L": ReadProcessMemory() failed: " << GetLastError() << '\n';
		}
		else
		{
			std::wcout << spBuff.get() << std::endl;
		}
	} 
	else
	{
		std::unique_ptr<CHAR[]> spBuff = std::make_unique<CHAR[]>(pDbgStr->nDebugStringLength + 1);

		if (   !ReadProcessMemory(m_debuggee.getProcessHandle(), pDbgStr->lpDebugStringData, spBuff.get(), 
								  pDbgStr->nDebugStringLength * sizeof(CHAR), &cbRead)
			|| 0 == cbRead)
		{
			std::wcerr << __FUNCTIONW__ << L": ReadProcessMemory() failed: " << GetLastError() << '\n';
		}
		else
		{
			std::cout << spBuff.get() << std::endl;
		}
	}

	return DBG_CONTINUE;
}

DWORD Core::onRip(const LPDEBUG_EVENT pDbgEvent)
{
	LPRIP_INFO pRip = &pDbgEvent->u.RipInfo;

	CAtlString info;

	switch (pRip->dwType)
	{
	case 0:
		info = L"no additional info";
		break;
	case SLE_ERROR:
		info = L"invalid data were passed to the function that failed. This caused the application to fail.";
		break;
	case SLE_MINORERROR:
		info = L"indicates that invalid data were passed to the function, but the error probably will not cause the application to fail.";
		break;
	case SLE_WARNING:
		info = L"potentially invalid data were passed to the function, but the function completed processing.";
		break;
	default:
		info = L"unknown code";
		ATLASSERT(FALSE); break;
	}

	std::wcout << L"RIP: debuggee died outside of the system debugger\'s control. Error code: " << pRip->dwError 
		       << L"Additional info: " << info.GetString() << '\n';

	return DBG_CONTINUE;
}
