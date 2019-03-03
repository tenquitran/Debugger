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

	const CAtlString targetPath = m_debuggee.getTargetPath();

	const CAtlString currDirectory = m_debuggee.getCurrentDirectory();

	const DWORD creationFlags = DEBUG_PROCESS;

	if (!CreateProcess(
		targetPath, nullptr, nullptr, nullptr, FALSE, creationFlags, nullptr,
		(currDirectory.IsEmpty() ? nullptr : currDirectory.GetString()), &si, &pi))
	{
		std::wcerr << L"CreateProcess() failed: " << GetLastError() << '\n';
		ATLASSERT(FALSE); return false;
	}

	m_debuggee.setTargetPID(pi.dwProcessId);

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
		ATLASSERT(FALSE);
		break;
	}

	return DBG_CONTINUE;
}

DWORD Core::onCreateProcess(const LPDEBUG_EVENT pDbgEvent)
{
	// Examine or change the registers of the process's initial thread with GetThreadContext() and SetThreadContext(); 
	// read from and write to the process's virtual memory with ReadProcessMemory() and WriteProcessMemory();
	// suspend and resume thread execution with SuspendThread() and ResumeThread().
	// Be sure to close the handle to the process image file with CloseHandle().

	// TODO: implement
	//ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onCreateThread(const LPDEBUG_EVENT pDbgEvent)
{
	// Examine or change the thread's registers with GetThreadContext() and SetThreadContext(); 
	// and suspend and resume thread execution with SuspendThread() and ResumeThread().

	// TODO: implement
	//ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onExitProcess(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the process exit code.

	DWORD exitCode = {};

	if (!GetExitCodeProcess(m_debuggee.getHandle(), &exitCode))
	{
		std::wcerr << L"GetExitCodeProcess() failed: " << GetLastError() << '\n';
		return DBG_CONTINUE;
	}

	std::wcout << L"Process exit with code " << exitCode << std::endl;
	return DBG_CONTINUE;
}

DWORD Core::onExitThread(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the thread's exit code.

	// TODO: implement
	//ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onLoadDll(const LPDEBUG_EVENT pDbgEvent)
{
	// Read the debugging information included in the newly loaded DLL. 
	// Be sure to close the handle to the loaded DLL with CloseHandle().

	// TODO: implement
	//ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onUnloadDll(const LPDEBUG_EVENT pDbgEvent)
{
	// Display a message that the DLL has been unloaded.

	// TODO: implement
	//ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onOutputDebugString(const LPDEBUG_EVENT pDbgEvent)
{
	// Display the output debugging string.

	// TODO: implement
	ATLASSERT(FALSE);
	return DBG_CONTINUE;
}

DWORD Core::onRip(const LPDEBUG_EVENT pDbgEvent)
{
	// TODO: implement
	ATLASSERT(FALSE);
	return DBG_CONTINUE;
}
