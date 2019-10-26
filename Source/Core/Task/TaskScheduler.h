#pragma once

#include "ITask.hpp"

#include <vector>
#include <memory>

#include <boost/thread.hpp>

class TaskScheduler {
private:
	std::vector<std::shared_ptr<ITask>> taskCollection;

private:
	TaskScheduler();

public:
	~TaskScheduler();

	TaskScheduler(TaskScheduler const&)  = delete;
	void operator=(TaskScheduler const&) = delete;

	static TaskScheduler &GetInstance();

	void AddTask(std::shared_ptr<ITask> task, bool wait = false);


private:
	void executeTaskAsync(std::shared_ptr<ITask> task);

};