#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() {}

TaskScheduler::~TaskScheduler() {}

TaskScheduler &TaskScheduler::GetInstance() {
	static TaskScheduler taskSchedulerInstance;
	return taskSchedulerInstance;
}

void TaskScheduler::AddTask(std::shared_ptr<ITask> task, bool wait) {
	

	// Not used yet, when to many tasks are executed the new tasks will get stored in a collection
	this->taskCollection.push_back(task);


	if (wait) {
		boost::thread th(&ITask::Start, task);
		th.join();

	} else {
		this->executeTaskAsync(task);
	}


}

void TaskScheduler::executeTaskAsync(std::shared_ptr<ITask> task) {
	boost::thread th(&ITask::Start, task);
	th.detach();

}