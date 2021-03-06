# Locate STB dir
SET(STB_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Look for the header file.
SET(STB_INCLUDE_DIR "${STB_ROOT_DIR}")

# Handle the QUIETLY and REQUIRED arguments and set STB_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	stb DEFAULT_MSG
	STB_ROOT_DIR
	STB_INCLUDE_DIR)

# Copy the results to the output variables.
IF (STB_FOUND)
	SET(STB_INCLUDE_DIRS ${STB_INCLUDE_DIR})
ELSE (STB_FOUND)
	SET(STB_INCLUDE_DIRS)
ENDIF (STB_FOUND)

#message("STB_INCLUDE_DIRS = ${STB_INCLUDE_DIRS}")

MARK_AS_ADVANCED(STB_INCLUDE_DIRS)