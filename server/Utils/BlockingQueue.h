#pragma once

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <queue>

template <typename T>
class BlockingQueue {
public:
	BlockingQueue() = default;
	BlockingQueue(const BlockingQueue&) = delete;
	BlockingQueue(BlockingQueue&&) = delete;

	void push(const T& value) {
		auto lock = this->lock();
		conditionVariable.notify_one();
		queue.push(value);
	}

	void push(T&& value) {
		auto lock = this->lock();
		conditionVariable.notify_one();
		queue.push(std::move(value));
	}

	template< class Rep, class Period>
	bool popAndGet(T& result, const std::chrono::duration<Rep, Period> &timeout) {
		auto lock = this->lock();
		auto noTimeout = conditionVariable.wait_for(lock, timeout, [this] { return !this->empty(); });
		if (noTimeout) {
			result = std::move(queue.front());
			queue.pop();
		}
		return noTimeout;
	}

	bool empty() {
		return queue.empty();
	}

	void clear() {
		auto lock = this->lock();
		std::queue<T> empty;
		std::swap(queue, empty);
	}

private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable conditionVariable;

	std::unique_lock<std::mutex> lock() {
		return std::unique_lock<std::mutex> { this->mutex };
	}
};
