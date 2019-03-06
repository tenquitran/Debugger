#pragma once


namespace DebuggerApp
{
	class Debuggee
	{
	public:
		// Parameters:
		// targetPath - full path to the target executable.
		// targetPid - target process ID.
		explicit Debuggee(const CAtlString& targetPath, DWORD targetPid = {});

		virtual ~Debuggee();

		// Set process-related information.
		void setProcessInfo(LPCREATE_PROCESS_DEBUG_INFO pProcessInfo);

		HANDLE getProcessHandle() const;

		CAtlString getTargetPath() const;

		DWORD getTargetPID() const;
		void setTargetPID(DWORD pid);

		CAtlString getCurrentDirectory() const;

		// Add info about the loaded DLL.
		// Parameters: pBase - base address;
		//             path - full path.
		void addDllInfo(LPVOID pBase, const CAtlString& path);

		// Find DLL path by its base address.
		// Returns: DLL path on success, empty string otherwise.
		CAtlString findDllPath(LPVOID pBase) const;

	private:
		// Full path to the target executable.
		const CAtlString m_targetPath;

		// Process ID of the target.
		DWORD m_targetPid = {};

		// Target process handle.
		HANDLE m_hProcess = { nullptr };

		CAtlString m_currentDirectory;

		// Loaded DLLs.
		// Key: base address; value: full path.
		std::map<LPVOID, CAtlString> m_dlls;
	};
}
