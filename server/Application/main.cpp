#pragma once

#include "Application/Application.h"
#include "Communication/SocketContext.h"
#include "InputSimulator/InputSimulator.h"

#include <memory>

int main() {
	std::unique_ptr<SocketContext> context = SocketContext::create();
	std::unique_ptr<InputSimulator> inputSimulator = InputSimulator::create();

	Application application;
	application.run(*context, *inputSimulator);
}
