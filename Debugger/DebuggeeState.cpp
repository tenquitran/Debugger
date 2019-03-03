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

CAtlString DebuggeeState::getCurrentDirectory() const
{
	return m_currentDirectory;
}
