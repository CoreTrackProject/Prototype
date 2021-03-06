cmake_minimum_required(VERSION 3.5)

set(BUILD_TARGET "CoreTrack")

set(BUILD_VERSION_MAJOR 0)
set(BUILD_VERSION_MINOR 1)
set(BUILD_VERSION_PATCH 0)

project(${BUILD_TARGET})


# We have to use C++14 since imgui-node-editor uses C++14
# (other component uses C++11)
set(CMAKE_CXX_STANDARD          14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)


find_package(OpenCV REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)
find_package(Boost REQUIRED filesystem log_setup log serialization )
find_package(GLEW REQUIRED)
find_package(Ceres CONFIG REQUIRED) # for libmv

find_library(XXHASH_LIBRARY NAMES xxhash)
include(CMake/Utils.cmake)

find_path( AVCODEC_INCLUDE_DIR libavcodec/avcodec.h )
find_library( AVCODEC_LIBRARY avcodec )
find_library( AVDEVICE_LIBRARY avdevice )
find_library( AVFILTER_LIBRARY avfilter )
find_library( AVFORMAT_LIBRARY avformat )
find_library( AVRESAMPLE_LIBRARY avresample )
find_library( AVUTIL_LIBRARY avutil )
find_library( AVUTIL_SW_RESAMPLE swresample )
find_library( AVUTIL_SW_SCALE swscale )

set_property(GLOBAL PROPERTY source_list)
set_property(GLOBAL PROPERTY include_list)


set(INC
	${OPENGL_INCLUDE_DIR}

	#ImGui Node editor
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/BlueprintUtilities/Include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/BlueprintUtilities/Source
	#${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/gl3w/Include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/stb_image
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/Application/Source/GLFW

	#ImGui Guizmo and Sequencer
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo
	
)
add_include(${INC})

set(SRC
   
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/imgui/imgui.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/imgui/imgui_widgets.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/imgui/imgui_draw.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/imgui/imgui_demo.cpp

   	# Borrow some codes in Examples/Common/BlueprintUtilities
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/BlueprintUtilities/Source/ax/Drawing.cpp
  	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/BlueprintUtilities/Source/ax/Builders.cpp
  	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/BlueprintUtilities/Source/ax/Widgets.cpp


   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/stb_image/stb_image.h
   	#${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/ThirdParty/gl3w/Source/gl3w.c
   	#${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/Examples/Common/Application/Source/GLFW/imgui_impl_glfw_gl3.cpp
   
   	# imgui-node-editor
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Include/imgui_node_editor.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/crude_json.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/crude_json.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_bezier_math.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_bezier_math.inl
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_canvas.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_canvas.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_extra_math.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_extra_math.inl
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_node_editor_api.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_node_editor_internal.h
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_node_editor_internal.inl
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Source/imgui_node_editor.cpp
   	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui-node-editor/NodeEditor/Support/imgui_node_editor.natvis


	# imguizmo
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImCurveEdit.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImCurveEdit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImGradient.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImGradient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImGuizmo.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImGuizmo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImSequencer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/ImSequencer.cpp
	
)

add_source(${SRC})
add_subdirectory(Source)


add_include(${Boost_INCLUDE_DIRS})

add_include("./ThirdParty/libmv")
add_include(${AVCODEC_INCLUDE_DIR})

if (MSVC)
	add_compile_options(-bigobj)
endif(MSVC)


get_property(local_include_list GLOBAL PROPERTY include_list)
include_directories(${local_include_list})

get_property(local_src_list GLOBAL PROPERTY source_list)

add_executable(
   ${BUILD_TARGET}
   ${local_src_list}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${local_src_list})

add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

add_subdirectory("./ThirdParty/libmv")

target_link_libraries(
    ${BUILD_TARGET}
    ${OPENGL_LIBRARIES}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    glfw
    unofficial::nativefiledialog::nfd
    ${XXHASH_LIBRARY}
    GLEW::GLEW

	ceres
    libmv

    # FFMPEG related libs

    ${AVFORMAT_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${AVUTIL_SW_RESAMPLE}
    ${AVUTIL_SW_SCALE}
    ${AVDEVICE_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${AVRESAMPLE_LIBRARY}


)

# Install the built executable into (prefix)/bin
install(TARGETS ${BUILD_TARGET} DESTINATION bin)

# [VisualStudio]
if (WIN32)

  # Set `CoreTrack` as a startup project for VS IDE
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${BUILD_TARGET})

  # For easier debugging in VS IDE(cmake 3.8.0 or later required)
  # Set working directory to $nnview git repo root.
  if (CMAKE_VERSION VERSION_GREATER 3.8.0)
	set_target_properties(${BUILD_TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endif()



# Increase warning level for clang.
# https://stackoverflow.com/questions/13638408/override-compile-flags-for-single-files
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Assume C++ sources
  set_source_files_properties(${NNVIEW_SOURCES} PROPERTIES COMPILE_FLAGS "-Weverything -Wno-system-headers -Werror -Wno-padded -Wno-c++98-compat-pedantic -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro")
ENDIF ()
