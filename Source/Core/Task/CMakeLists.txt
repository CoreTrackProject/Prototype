add_subdirectory(ProjectIO)

set(SRC
	${CMAKE_CURRENT_SOURCE_DIR}/ITask.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/TaskScheduler.h
	${CMAKE_CURRENT_SOURCE_DIR}/TaskScheduler.cpp
)

add_source(${SRC})
