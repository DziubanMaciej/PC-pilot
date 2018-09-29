#pragma once
#include "InputSimulator/InputSimulator.h"

struct tagINPUT;

class WindowsInputSimulator : public InputSimulator {
public:
	void sleepMs(unsigned int ms) override;
	bool sendMouseButtonEvent(MouseButtonEventType mouseButtonEventType) override;
	bool click() override;
	bool moveCursorTo(int x, int y) override;
	bool moveCursorBy(int x, int y) override;
	bool pressButtonUnicode(char16_t unicode) override;
	bool pressButtonEnter() override;

protected:
	bool sendInput(tagINPUT& input);
};
