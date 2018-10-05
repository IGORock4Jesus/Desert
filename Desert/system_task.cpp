#include "system_task.h"
#include <Windows.h>


namespace Desert::System {


	const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.  
		LPCSTR szName; // Pointer to name (in user addr space).  
		DWORD dwThreadID; // Thread ID (-1=caller thread).  
		DWORD dwFlags; // Reserved for future use, must be zero.  
	} THREADNAME_INFO;
#pragma pack(pop)  
	void SetThreadName(DWORD dwThreadID, const char* threadName) {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
#pragma warning(pop)  
	}

	void SetThreadName(const char* threadName)
	{
		SetThreadName(GetCurrentThreadId(), threadName);
	}

	void SetThreadName(std::thread* thread, const char* threadName)
	{
		DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread->native_handle()));
		SetThreadName(threadId, threadName);
	}


	void Task::Process()
	{
		while (enabled)
		{
			OnPass();
		}
	}

	Task::Task()
		: _name{ "" }
	{
	}

	Task::Task(std::string name)
		: _name{ name }
	{
	}

	Task::~Task()
	{
		Stop();
	}

	void Task::Start()
	{
		if (enabled)return;
		enabled = true;
		thread = std::thread{ &Task::Process, this };
		if (_name != "")
			SetThreadName(&thread, _name.c_str());
	}

	void Task::Stop()
	{
		if (!enabled)return;
		enabled = false;

		if (thread.joinable())
			thread.join();
	}
}