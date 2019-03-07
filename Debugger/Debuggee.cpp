#include "stdafx.h"
#include "Debuggee.h"

//////////////////////////////////////////////////////////////////////////

using namespace DebuggerApp;

//////////////////////////////////////////////////////////////////////////


Debuggee::Debuggee(const CAtlString& imagePath, DWORD pid /*= {}*/)
	: m_imagePath(imagePath), m_processId(pid)
{
	if (m_imagePath.IsEmpty())
	{
		ATLASSERT(FALSE); throw EXCEPTION(L"Target path empty");
	}

	int pos = m_imagePath.ReverseFind('\\');
	if (-1 != pos)
	{
		m_currentDirectory = m_imagePath.Left(pos);
	}

	if (m_currentDirectory.IsEmpty())
	{
		ATLASSERT(FALSE); throw EXCEPTION(L"Failed to determine current directory");
	}
}

Debuggee::~Debuggee()
{
}

void Debuggee::setProcessInfo(LPCREATE_PROCESS_DEBUG_INFO pProcessInfo)
{
	// Store process handle.
	if (!pProcessInfo->hProcess)
	{
		std::wcerr << L"Process handle is NULL\n";
		ATLASSERT(FALSE);
	}
	else
	{
		m_hProcess = pProcessInfo->hProcess;
	}

	// Store initial thread ID and handle.
	if (!pProcessInfo->hThread)
	{
		std::wcerr << L"Initial thread handle is NULL\n";
		ATLASSERT(FALSE);
	}
	else
	{
		addThreadInfo(pProcessInfo->hThread);
	}
}

HANDLE Debuggee::getProcessHandle() const
{
	return m_hProcess;
}

CAtlString Debuggee::getImagePath() const
{
	return m_imagePath;
}

DWORD Debuggee::getPID() const
{
	return m_processId;
}

void Debuggee::setPID(DWORD pid)
{
	m_processId = pid;

	if (m_processId < 1)
	{
		std::wcerr << __FUNCTIONW__ << L": invalid process ID\n";
		return;
	}
}

CAtlString Debuggee::getCurrentDirectory() const
{
	return m_currentDirectory;
}

void Debuggee::addThreadInfo(HANDLE hThread)
{
	DWORD tid = GetThreadId(hThread);

	if (0 == tid)
	{
		std::wcerr << __FUNCTIONW__ << L": GetThreadId() failed: " << GetLastError() << '\n';
		ATLASSERT(FALSE);
	}

	m_threads.insert(std::make_pair(tid, hThread));
}

void Debuggee::addDllInfo(LPVOID pBase, const CAtlString& path)
{
	m_dlls.insert(std::make_pair(pBase, path));
}

CAtlString Debuggee::findDllPath(LPVOID pBase) const
{
	std::map<LPVOID, CAtlString>::const_iterator itr = m_dlls.find(pBase);

	if (m_dlls.cend() == itr)
	{
		return L"";
	}

	return itr->second;
}
