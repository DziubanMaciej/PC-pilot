add_sources (
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/ConnectionlessSocket.h
	${CMAKE_CURRENT_SOURCE_DIR}/InetAddress.h
	${CMAKE_CURRENT_SOURCE_DIR}/SocketContext.h
	${CMAKE_CURRENT_SOURCE_DIR}/Messages.h
)
add_os_specific_subdirectory()
