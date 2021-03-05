# append all files with extension "ext" in the "dirs" directories to "ret"
# excludes all files starting with a '.' (dot)
macro (acg_append_files ret ext)
    foreach (_dir ${ARGN})
        file (GLOB _files "${_dir}/${ext}")
        foreach (_file ${_files})
            get_filename_component (_filename ${_file} NAME)
            if (_filename MATCHES "^[.]")
	            list (REMOVE_ITEM _files ${_file})
            endif ()
        endforeach ()
        list (APPEND ${ret} ${_files})
    endforeach ()
endmacro ()

# Locate root dir
SET(OPENMESH_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/src/OpenMesh")

# ----------------------------------------

SET(OPENMESH_CORE_DIR "${OPENMESH_ROOT_DIR}/Core")

SET(CORE_DIRS
    "${OPENMESH_CORE_DIR}"
    "${OPENMESH_CORE_DIR}/Geometry"
    "${OPENMESH_CORE_DIR}/IO"
    "${OPENMESH_CORE_DIR}/IO/exporter"
    "${OPENMESH_CORE_DIR}/IO/importer"
    "${OPENMESH_CORE_DIR}/IO/reader"
    "${OPENMESH_CORE_DIR}/IO/writer"
    "${OPENMESH_CORE_DIR}/Mesh"
    "${OPENMESH_CORE_DIR}/Mesh/gen"
    "${OPENMESH_CORE_DIR}/System"
    "${OPENMESH_CORE_DIR}/Utils"
)

# collect all header and source files
acg_append_files (CORE_HEADERS "*.hh" "${CORE_DIRS}")
acg_append_files (CORE_SOURCES "*.cc" "${CORE_DIRS}")

SET(OPENMESH_CORE_HEADERS "${CORE_HEADERS}")
SET(OPENMESH_CORE_SOURCES "${CORE_SOURCES}")

# ----------------------------------------

SET(OPENMESH_TOOL_DIR "${OPENMESH_ROOT_DIR}/Tools")

SET(TOOL_DIRS 
  "${OPENMESH_TOOL_DIR}"
  "${OPENMESH_TOOL_DIR}/Decimater"
  "${OPENMESH_TOOL_DIR}/Dualizer"
  "${OPENMESH_TOOL_DIR}/Smoother"
  "${OPENMESH_TOOL_DIR}/Subdivider/Adaptive/Composite"
  "${OPENMESH_TOOL_DIR}/Subdivider/Uniform/Composite"
  "${OPENMESH_TOOL_DIR}/Subdivider/Uniform"
  "${OPENMESH_TOOL_DIR}/Utils"
  "${OPENMESH_TOOL_DIR}/VDPM"
)

# collect all header and source files
acg_append_files (TOOL_HEADERS "*.hh" "${TOOL_DIRS}")
acg_append_files (TOOL_SOURCES "*.cc" "${TOOL_DIRS}")

SET(OPENMESH_TOOL_HEADERS "${TOOL_HEADERS}")
SET(OPENMESH_TOOL_SOURCES "${TOOL_SOURCES}")

# ----------------------------------------

# Handle the QUIETLY and REQUIRED arguments and set OPENMESH_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	OpenMesh DEFAULT_MSG
	OPENMESH_ROOT_DIR)

SET(
    OPENMESH_INCLUDE_DIRS
    "${OPENMESH_ROOT_DIR}/..")

MARK_AS_ADVANCED(
    OPENMESH_CORE_SOURCES
    OPENMESH_CORE_HEADERS
    OPENMESH_TOOL_HEADERS
    OPENMESH_TOOL_SOURCES
    OPENMESH_INCLUDE_DIRS)