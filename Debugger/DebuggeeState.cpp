#include "stdafx.h"
#include "DebuggeeState.h"

//////////////////////////////////////////////////////////////////////////

using namespace DebuggerApp;

//////////////////////////////////////////////////////////////////////////


DebuggeeState::DebuggeeState(const CAtlString& targetPath, DWORD targetPid /*= {}*/)
	: m_targetPath(targetPath), m_targetPid(targetPid)
{
	if (m_targetPath.IsEmpty())
	{
		ATLASSERT(FALSE); throw EXCEPTION(L"Target path empty");
	}

	int pos = m_targetPath.ReverseFind('\\');
	if (-1 != pos)
	{
		m_currentDirectory = m_targetPath.Left(pos);
	}

	if (m_currentDirectory.IsEmpty())
	{
		ATLASSERT(FALSE); throw EXCEPTION(L"Failed to determine current directory");
	}

	if (m_targetPid > 0)
	{
		m_hProcess.Attach(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_targetPid));
		if (!m_hProcess)
		{
			ATLASSERT(FALSE); throw EXCEPTION(L"OpenProcess() for target failed: %u", GetLastError());
		}
	}
}

DebuggeeState::~DebuggeeState()
{
}

CAtlString DebuggeeState::getTargetPath() const
{
	return m_targetPath;
}

DWORD DebuggeeState::getTargetPID() const
{
	return m_targetPid;
}

void DebuggeeState::setTargetPID(DWORD pid)
{
	m_targetPid = pid;

	if (m_targetPid < 1)
	{
		std::wcerr << __FUNCTIONW__ << L": invalid process ID\n";
		return;
	}

	m_hProcess.Attach(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_targetPid));
	if (!m_hProcess)
	{
		std::wcerr << L"OpenProcess() for target failed: " << GetLastError() << '\n';
	}
}

CAtlString DebuggeeState::getCurrentDirectory() const
{
	return m_currentDirectory;
}

HANDLE DebuggeeState::getHandle() const
{
	return m_hProcess.m_h;
}
