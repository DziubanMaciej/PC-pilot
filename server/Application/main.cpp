#pragma once

#include "Application/Application.h"
#include "Communication/SocketContext.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/ArgumentParser.h"

#include <memory>

int main(int argc, char **argv) {
	std::unique_ptr<SocketContext> context = SocketContext::create();
	std::unique_ptr<InputSimulator> inputSimulator = InputSimulator::create();

	ArgumentParser argumentParser{ argc, argv };
	auto port = argumentParser.getArgumentValue<uint16_t>({ "-p", "--port" }, 9999u);

	Application application;
	application.run(*context, *inputSimulator, port);
}
