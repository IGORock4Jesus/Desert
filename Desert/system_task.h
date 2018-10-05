#pragma once

#include <thread>
#include <string>


namespace Desert::System
{
	class Task {
		std::thread thread;
		bool enabled;
		std::string _name;


		void Process();

	protected:
		virtual void OnPass() {}

	public:
		Task();
		Task(std::string name);
		~Task();
		void Start();
		void Stop();
	};
}