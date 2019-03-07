#pragma once


namespace DebuggerApp
{
	class Debuggee
	{
	public:
		// Parameters: imagePath - full path to the executable image;
		//             pid - process ID.
		explicit Debuggee(const CAtlString& imagePath, DWORD pid = {});

		virtual ~Debuggee();

		// Set process-related information.
		void setProcessInfo(LPCREATE_PROCESS_DEBUG_INFO pProcessInfo);

		HANDLE getProcessHandle() const;

		CAtlString getImagePath() const;

		DWORD getPID() const;
		void setPID(DWORD pid);

		CAtlString getCurrentDirectory() const;

		// Add info about thread.
		// Parameters: hThread - thread handle.
		void addThreadInfo(HANDLE hThread);

		// Add info about loaded DLL.
		// Parameters: pBase - base address;
		//             path - full path.
		void addDllInfo(LPVOID pBase, const CAtlString& path);

		// Find DLL path by its base address.
		// Returns: DLL path on success, empty string otherwise.
		CAtlString findDllPath(LPVOID pBase) const;

	private:
		// Full path to the target executable.
		const CAtlString m_imagePath;

		// Process ID.
		DWORD m_processId = {};

		// Target process handle.
		HANDLE m_hProcess = { nullptr };

		CAtlString m_currentDirectory;

		// Threads.
		// Key: thread ID; value: thread handle.
		std::map<DWORD, HANDLE> m_threads;

		// Loaded DLLs.
		// Key: base address; value: full path.
		std::map<LPVOID, CAtlString> m_dlls;
	};
}
