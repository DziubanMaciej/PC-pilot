#pragma once

#include "Utils/InstanceLimited.h"
#include <iostream>
#include <mutex>

class Logger {
	Logger(std::ostream& out) : out(out) {}
	std::ostream& out;
	std::mutex mutex;

	template <typename T, typename... Args>
	void _log(const T& arg, const Args&... args) {
		this->out << arg;
		_log(args...);
	}

	template <typename... Args>
	void _log() {
		this->out << '\n';
	}

public:
	static Logger log;

	template <typename... Args>
	Logger& operator()(const Args&... args) {
		std::unique_lock<std::mutex> lock{ this->mutex };
		_log(args...);
		return *this;
	}
};
