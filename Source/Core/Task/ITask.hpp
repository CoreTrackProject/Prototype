#pragma once

#include <memory>

enum class TaskStatus {
	Created,
	Running,
	Stopped,
	Paused,
	Errored,
	Finished
};

class ITask {

	public:
		ITask() {}
		virtual ~ITask() {}

		virtual void Start() = 0;
		virtual void Stop()  = 0;
		virtual void Pause() = 0;
		virtual TaskStatus GetStatus() = 0;
		virtual std::shared_ptr<void> GetResult() = 0;


};
