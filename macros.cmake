macro(add_sources)
	set_property(GLOBAL APPEND PROPERTY ${SOURCE_FILES_PROPERTY} ${ARGN})
endmacro()

macro(get_sources var)
	get_property(${var} GLOBAL PROPERTY ${SOURCE_FILES_PROPERTY})
endmacro()
	
macro(add_subdirectories)
	foreach(subdir ${ARGN})
		add_subdirectory(${subdir})
	endforeach()
endmacro()