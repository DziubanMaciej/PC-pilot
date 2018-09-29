#pragma once

#include "Utils/InstanceLimited.h"
#include <memory>

class InputSimulator : public OneInstanced<InputSimulator> {
protected:
	InputSimulator() = default;
	InputSimulator(InputSimulator&) = delete;
public:
	static std::unique_ptr<InputSimulator> create();
	enum class MouseButtonEventType { LeftPress, LeftRelease, MiddlePress, MiddleRelease, RightPress, RightRelease };

	// general
	virtual void sleepMs(unsigned int ms) = 0;

	// mouse
	virtual bool sendMouseButtonEvent(MouseButtonEventType mouseButtonEventType) = 0;
	virtual bool click() = 0;
	virtual bool moveCursorTo(int x, int y) = 0;
	virtual bool moveCursorBy(int x, int y) = 0;

	// keyboard
	virtual bool pressButtonUnicode(char16_t unicode) = 0;
	virtual bool pressButtonEnter() = 0;
};
