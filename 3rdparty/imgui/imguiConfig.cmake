# Locate root dir
SET(IMGUI_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

FILE(GLOB IMGUI_SOURCES
    "${IMGUI_ROOT_DIR}/imgui.cpp"
    "${IMGUI_ROOT_DIR}/imgui_demo.cpp"
    "${IMGUI_ROOT_DIR}/imgui_draw.cpp"
    "${IMGUI_ROOT_DIR}/imgui_widgets.cpp"
    "${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl2.cpp"
    "${IMGUI_ROOT_DIR}/backends/imgui_impl_glut.cpp")
    
FILE(GLOB IMGUI_HEADERS
    "${IMGUI_ROOT_DIR}/imconfig.h"
    "${IMGUI_ROOT_DIR}/imgui.h"
    "${IMGUI_ROOT_DIR}/imgui_internal.h"
    "${IMGUI_ROOT_DIR}/imstb_rectpack.h"
    "${IMGUI_ROOT_DIR}/imstb_textedit.h"
    "${IMGUI_ROOT_DIR}/imstb_truetype.h"
    "${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl2.h"
    "${IMGUI_ROOT_DIR}/backends/imgui_impl_glut.h")

# Handle the QUIETLY and REQUIRED arguments and set IMGUI_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	imgui DEFAULT_MSG
	IMGUI_ROOT_DIR
	IMGUI_SOURCES
	IMGUI_HEADERS)

SET(IMGUI_INCLUDE_DIRS ${IMGUI_ROOT_DIR})

MARK_AS_ADVANCED(IMGUI_SOURCES IMGUI_HEADERS IMGUI_INCLUDE_DIRS)

# To use imgui sources directly, enable scripts below
IF (${IMGUI_EMBED_SOURCE})
    find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty")
    list(APPEND SRCS ${IMGUI_SOURCES})
    include_directories(${IMGUI_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SRCS})
ENDIF (${IMGUI_EMBED_SOURCE})