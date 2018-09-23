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
		dump(arg, false);
		_log(args...);
	}

	template <typename... Args>
	void _log() {
		dump("", true);
	}

public:
	static Logger log;

	template <typename... Args>
	void operator()(const Args&... args) {
		std::unique_lock<std::mutex> lock{ this->mutex };
		_log(args...);
	}

	template <typename Iterator>
	void dumpContainer(Iterator begin, Iterator end, bool insertNewLines = true) {
		std::unique_lock<std::mutex> lock{ this->mutex };
		for (auto it = begin; it < end; it++) {
			dump(*it, insertNewLines);
		}
	}

	template <typename T>
	void dump(const T &arg, bool insertNewLines = true) {
		this->out << arg;
		if (insertNewLines) {
			this->out << "\n";
		}
	}
};
