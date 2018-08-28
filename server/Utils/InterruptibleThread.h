#pragma once

#include <thread>
#include <functional>
#include <memory>

template <typename... Args>
class InterruptibleThread {
public:
	void start(Args&... args) {
		this->interrupted = false;
		this->thread = std::make_unique<std::thread>(
			threadFunctionWrapper,
			std::reference_wrapper<InterruptibleThread>{ *this },
			std::reference_wrapper<Args>(args)...);
	}

	static void threadFunctionWrapper(InterruptibleThread &thr, Args&... args) {
		thr.threadFunction(args...);
	}

	void join() {
		this->thread->join();
		this->thread.reset(nullptr);
	}

	void interrupt() {
		this->interrupted = true;
	}

	bool isInterrupted() const {
		return this->interrupted;
	}

protected:
	virtual void threadFunction(Args&... args) = 0;

private:
	bool interrupted = false;
	std::unique_ptr<std::thread> thread;
};

template <typename... Args>
class InterruptibleThreadAdapter : public InterruptibleThread<Args...> {
protected:
	void threadFunction(Args&... args) override {
		onStart(args...);
		while (!this->isInterrupted()) {
			onUpdate(args...);
		}
		onEnd(args...);
	}

	virtual void onStart(Args&...) {}
	virtual void onUpdate(Args&...) {}
	virtual void onEnd(Args&...) {}
};
