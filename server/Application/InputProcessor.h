#pragma once

#include <unordered_map>

template <typename ReturnType, typename ...Args>
struct InputProcessor {
	using InputHandler = ReturnType(*)(Args...);
	using InputEntries = std::unordered_map<std::string, InputHandler>;
	InputProcessor(InputEntries &inputEntries, InputHandler unknownCommandHandler) : inputEntries(inputEntries), unknownCommandHandler(unknownCommandHandler) {}

	ReturnType call(const std::string &command, Args... args) {
		const auto it = this->inputEntries.find(command);
		const auto handler = (it != this->inputEntries.end()) ? it->second : this->unknownCommandHandler;
		return handler(args...);
	}

	const InputEntries inputEntries;
	const InputHandler unknownCommandHandler;
};
