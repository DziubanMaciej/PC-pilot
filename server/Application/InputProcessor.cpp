#include "Application/InputProcessor.h"

#include "Application/Application.h"
#include "Utils/Logger.h"

#include <string>

const InputProcessor::InputEntries InputProcessor::inputEntries = {
	{ "", InputProcessor::emptyMessage },
	{ "help", InputProcessor::printHelp },
	{ "status", InputProcessor::printStatus },
	{ "address", InputProcessor::printAddress },
	{ "exit", InputProcessor::exit }
};

void InputProcessor::call(Application &application, const std::string &command) {
	auto it = InputProcessor::inputEntries.find(command);
	if (it == InputProcessor::inputEntries.end()) {
		Logger::log("Unknown command provided. Type \"help\" to see available commands.\n");
		return;
	}

	it->second(application);
}

void InputProcessor::emptyMessage(Application & application) {
}

void InputProcessor::printHelp(Application &application) {
	std::vector<std::string> tokens;
	tokens.push_back("Available commands:");

	for (auto& entry : InputProcessor::inputEntries) {
		if (!entry.first.empty()) {
			std::string line = "     - ";
			line.append(entry.first);
			tokens.push_back(line);
		}
	}

	Logger::log.dumpContainer(tokens.begin(), tokens.end());
}

void InputProcessor::printStatus(Application &application) {
	Logger::log(application.getStatus());
}

void InputProcessor::printAddress(Application & application) {
	Logger::log(application.getAddress());
}

void InputProcessor::exit(Application &application) {
	application.exit();
}
