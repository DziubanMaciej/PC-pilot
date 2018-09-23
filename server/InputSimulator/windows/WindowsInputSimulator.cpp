#include "InputSimulator/windows/WindowsInputSimulator.h"

#include <Windows.h>
#include "Winuser.h"

#include <algorithm>
#include <exception>
#include <thread>

std::unique_ptr<InputSimulator> InputSimulator::create() {
	return std::make_unique<WindowsInputSimulator>();
}

bool WindowsInputSimulator::sendInput(tagINPUT &input) {
  auto inputsProcessed = ::SendInput(1, &input, sizeof(input));
  return inputsProcessed != 0;
}

bool WindowsInputSimulator::sendMouseButtonEvent(
    MouseButtonEventType mouseButtonEventType) {
  DWORD dwFlags = 0;
  switch (mouseButtonEventType) {
  case MouseButtonEventType::LeftPress:

    dwFlags = MOUSEEVENTF_LEFTDOWN;
    break;
  case MouseButtonEventType::LeftRelease:
    dwFlags = MOUSEEVENTF_LEFTUP;
    break;
  case MouseButtonEventType::MiddlePress:
    dwFlags = MOUSEEVENTF_MIDDLEDOWN;
    break;
  case MouseButtonEventType::MiddleRelease:
    dwFlags = MOUSEEVENTF_MIDDLEUP;
    break;
  case MouseButtonEventType::RightPress:
    dwFlags = MOUSEEVENTF_RIGHTDOWN;
    break;
  case MouseButtonEventType::RightRelease:
    dwFlags = MOUSEEVENTF_MIDDLEUP;
    break;
  default:
    throw std::exception("Unknown MouseButtonEventType");
  }

  INPUT input;
  input.type = 0;
  input.mi.dx = 0;        // ignored
  input.mi.dy = 0;        // ignored
  input.mi.mouseData = 0; // ignored
  input.mi.dwFlags = dwFlags;
  input.mi.time = 0; // system will provide timestamp
  input.mi.dwExtraInfo = 0;
  return sendInput(input);
}

void WindowsInputSimulator::sleepMs(unsigned int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool WindowsInputSimulator::click() {
  bool retVal = sendMouseButtonEvent(MouseButtonEventType::LeftPress);
  if (retVal) {
    retVal = sendMouseButtonEvent(MouseButtonEventType::LeftRelease);
  }
  return retVal;
}

bool WindowsInputSimulator::moveCursorTo(int x, int y) {
  return SetCursorPos(x, y) != 0;
}

bool WindowsInputSimulator::moveCursorBy(int x, int y) {
  POINT current;
  bool retVal = ::GetCursorPos(&current) != 0;
  if (retVal) {
    retVal = moveCursorTo(current.x + x, current.y + y);
  }
  return retVal;
}

bool WindowsInputSimulator::pressButtonUnicode(char16_t unicode) {
  INPUT input;
  input.type = 1;
  input.ki.wVk = 0;
  input.ki.wScan = static_cast<WORD>(unicode);
  input.ki.dwFlags = KEYEVENTF_UNICODE;
  input.ki.time = 0;
  input.ki.dwExtraInfo = 0;
  return sendInput(input);
}
