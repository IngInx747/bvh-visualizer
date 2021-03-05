project(viewer)

file(GLOB SRCS
    "*.h"
    "*.hh"
    "*.hpp"
    "*.c"
    "*.cc"
    "*.cpp")
    
add_executable(${PROJECT_NAME} ${SRCS})
include_directories("${CMAKE_SOURCE_DIR}/3rdparty")

# ---------- Precompiled libraries ----------

# OpenGL and GLUT
if (MSVC)
    find_package(freeglut REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/freeglut")
    include_directories("${FREEGLUT_INCLUDE_DIRS}")
    target_link_libraries(${PROJECT_NAME} "${FREEGLUT_LIBRARIES}")
else (MSVC)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif (MSVC)

# ---------- Header-only libraries ----------

# GLM
find_package(glm REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/glm")
include_directories(${GLM_INCLUDE_DIRS})

# STB
find_package(stb REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/stb")
include_directories(${STB_INCLUDE_DIRS})

# Eigen
find_package(Eigen REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/Eigen")
include_directories(${EIGEN_INCLUDE_DIRS})

# ---------- To-build libraries ----------

# ImGUI
find_package(imgui REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/imgui")
include_directories(${IMGUI_INCLUDE_DIRS})
add_dependencies(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} imgui)

# OpenMesh
find_package(OpenMesh REQUIRED HINTS "${CMAKE_SOURCE_DIR}/3rdparty/OpenMesh")
include_directories(${OPENMESH_INCLUDE_DIRS})
add_dependencies(${PROJECT_NAME} OpenMeshCore)
add_dependencies(${PROJECT_NAME} OpenMeshTool)
target_link_libraries(${PROJECT_NAME} OpenMeshCore)
target_link_libraries(${PROJECT_NAME} OpenMeshTool)

if (WIN32)
    add_definitions(
        -D_USE_MATH_DEFINES
        -DNOMINMAX
        -D_CRT_SECURE_NO_WARNINGS)
endif ()
