#pragma once

#include <string>
#include <iostream>
#include <exception>

class ApplicationError : public std::logic_error {
public:
	ApplicationError(const std::string& message) : std::logic_error(message) {}

	static void exception(const std::string &message) {
		throw ApplicationError(message);
	}

	static void unrecoverable(const std::string &message, int errorCode) {
		unrecoverable(appendErrorCode(message, errorCode));
	}

	static void unrecoverable(const std::string &message) {
		std::cerr << message << std::endl;
		abort();
	}

private:
	static std::string appendErrorCode(const std::string &message, int errorCode) {
		return message + " (errorCode: " + std::to_string(errorCode) + ")";
	}
};
