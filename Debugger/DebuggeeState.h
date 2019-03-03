#pragma once


namespace DebuggerApp
{
	class DebuggeeState
	{
	public:
		// Parameters:
		// targetPath - full path to the target executable.
		// targetPid - target process ID.
		explicit DebuggeeState(const CAtlString& targetPath, DWORD targetPid = {});

		virtual ~DebuggeeState();

		CAtlString getTargetPath() const;

		DWORD getTargetPID() const;

		CAtlString getCurrentDirectory() const;

	private:
		// Full path to the target executable.
		const CAtlString m_targetPath;

		// Process ID of the target.
		const DWORD m_targetPid;

		CAtlString m_currentDirectory;
	};
}
