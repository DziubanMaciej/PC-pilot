#pragma once

#include <unordered_map>

class Application;

class InputProcessor {
	using InputHandler = void(*)(Application&);
	using InputEntries = std::unordered_map<std::string, InputHandler>;

public:
	static void call(Application &application, const std::string &command);
	static void printHelp(Application &application);

private:
	static void emptyMessage(Application &application);
	static void printStatus(Application &application);
	static void exit(Application &application);

	static const InputEntries inputEntries;
};
