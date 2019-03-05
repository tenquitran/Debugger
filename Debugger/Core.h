#pragma once

#include "Debuggee.h"


namespace DebuggerApp
{
	class Core
	{
	public:
		// Parameters:
		// targetPath - full path to the target executable.
		// createTarget - true to create the target process, false to attach to the existing process;
		// targetPid - target process ID (valid if createTarget is false).
		Core(const CAtlString& targetPath, bool createTarget, DWORD targetPid = {});

		virtual ~Core();

	private:
		bool createTargetProcess();

		bool attachToTarget();

	private:
		void debugLoop(const LPDEBUG_EVENT pDbgEvent);

		//////////////////////////////////////////////////////////////////////////
		// Debugging event handlers.
		//////////////////////////////////////////////////////////////////////////

		DWORD onException(const LPDEBUG_EVENT pDbgEvent);

		DWORD onCreateProcess(const LPDEBUG_EVENT pDbgEvent);

		DWORD onCreateThread(const LPDEBUG_EVENT pDbgEvent);

		DWORD onExitProcess(const LPDEBUG_EVENT pDbgEvent);

		DWORD onExitThread(const LPDEBUG_EVENT pDbgEvent);

		DWORD onLoadDll(const LPDEBUG_EVENT pDbgEvent);

		DWORD onUnloadDll(const LPDEBUG_EVENT pDbgEvent);

		DWORD onOutputDebugString(const LPDEBUG_EVENT pDbgEvent);

		DWORD onRip(const LPDEBUG_EVENT pDbgEvent);

	private:
		Debuggee m_debuggee;
	};
}
