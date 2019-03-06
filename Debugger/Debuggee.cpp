#include "stdafx.h"
#include "Debuggee.h"

//////////////////////////////////////////////////////////////////////////

using namespace DebuggerApp;

//////////////////////////////////////////////////////////////////////////


Debuggee::Debuggee(const CAtlString& targetPath, DWORD targetPid /*= {}*/)
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

Debuggee::~Debuggee()
{
}

CAtlString Debuggee::getTargetPath() const
{
	return m_targetPath;
}

DWORD Debuggee::getTargetPID() const
{
	return m_targetPid;
}

void Debuggee::setTargetPID(DWORD pid)
{
	m_targetPid = pid;

	if (m_targetPid < 1)
	{
		std::wcerr << __FUNCTIONW__ << L": invalid process ID\n";
		return;
	}
}

CAtlString Debuggee::getCurrentDirectory() const
{
	return m_currentDirectory;
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
