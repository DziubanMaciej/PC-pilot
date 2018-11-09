# PC Pilot

## Overview

PC Pilot is client-server application to pass commands from an Android device to a PC and simulate it as keyboard and mouse movement made by user.

## Project structure

### Server
Server is written entirely in C++, project can be generated using CMake and compiled with c++14 toolset on Windows. Linux is currently not supported, though application is prepared for multi-architecture implementation.

### Client
Client has been implemented entirely in Kotlin and works on Android devices starting with KitKat release (API level 19). Project files can be generated using Gradle. To compile successfully ANDROID_HOME environment variable has to point to location where Android SDK with required toolset is installed.

### Code generation
Code-gen directory contains Python scripts using Mako library to generate C++ and Kotlin source files for both client and server. It serves avoiding duplication of knowledge such as timeout durations, default port numbers or message encodings. Files are generated directly into their appropriate locations in client and server directories.

## Networking
Applications use UDP protocol to communicate. Concept of connection is achieved using periodically sent 'KeepAlive' messages by both sides. Only one client can be connected to the server. All 'ConnectionRequest' when the server is connected are ignored. Typical communication scheme is as follows:
- Connection
	- Server broadcasts 'Advertise' message periodically
	- Client sends 'ConnectionRequest' message
	- Server stops advertising, starts sending 'KeepAlive' messages to the client periodically
	- Client starts sending 'KeepAlive' messages to the server upon receiving first 'KeepAlive' from the server
- Client sends command messages such as 'LeftPress' or 'KeyPressBackSpace'. Server sends no ACK messages.
- Disconnection
	- Client sends 'DisconnectMessage'
	- Server sends 'DisconnectMessage'
	- Server starts broadcasting 'Advertise' message periodically

