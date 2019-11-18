set(INC 
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_include(${INC})

set(SRC
	${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Core.h
	${CMAKE_CURRENT_SOURCE_DIR}/Core.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/WindowManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/roboto_embed.inc.h
	${CMAKE_CURRENT_SOURCE_DIR}/roboto_mono_embed.inc.h
)

add_source(${SRC})

add_subdirectory(BaseEditor)
add_subdirectory(Project)
add_subdirectory(Task)
add_subdirectory(Utils)
