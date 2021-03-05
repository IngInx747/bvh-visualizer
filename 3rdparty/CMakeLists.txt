# CMakeLists of libraries to build as static libs.
# The libs are used by multiple executables and other libs.

# ImGUI
find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/imgui")
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
include_directories(${IMGUI_INCLUDE_DIRS})

# OpenGL and GLUT used by ImGUI
if (MSVC)
    find_package(freeglut REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/freeglut")
    include_directories("${FREEGLUT_INCLUDE_DIRS}")
    target_link_libraries(imgui "${FREEGLUT_LIBRARIES}")
else (MSVC)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
    target_link_libraries(imgui ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif (MSVC)

# OpenMesh
find_package(OpenMesh REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/OpenMesh")
add_library(OpenMeshCore STATIC ${OPENMESH_CORE_SOURCES} ${OPENMESH_CORE_HEADERS})
add_library(OpenMeshTool STATIC ${OPENMESH_TOOL_SOURCES} ${OPENMESH_TOOL_HEADERS})
include_directories(${OPENMESH_INCLUDE_DIRS})

if (WIN32)
    add_definitions(
        -D_USE_MATH_DEFINES
        -DNOMINMAX
        -D_CRT_SECURE_NO_WARNINGS)
endif ()
