cmake_minimum_required(VERSION 3.0)
project(QNplCad)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files



set(NPLRUNTIME_ROOT "${CMAKE_SOURCE_DIR}/deps/NPLRuntime")
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set(OUTPUT_BIN_DIR "${NPLRUNTIME_ROOT}/ParaWorld/bin64")
	set(QT_ROOT "D:\\Qt\\Qt5.6.2\\5.6\\msvc2015")
else()
	set(OUTPUT_BIN_DIR "${NPLRUNTIME_ROOT}/ParaWorld/bin32")
	set(QT_ROOT "D:\\Qt\\Qt5.6.2\\5.6\\msvc2015")
endif()

set (CMAKE_PREFIX_PATH ${QT_ROOT})

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Boost 1.61 REQUIRED) 

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient")
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient/Core")


file (GLOB Main_FILES "${PROJECT_SOURCE_DIR}/src/*.*")
SOURCE_GROUP("src" FILES ${Main_FILES})
list(APPEND QNplCad_SOURCE_FILES ${Main_FILES})



# Tell CMake to create the QNplCad executable
add_executable(QNplCad WIN32  ${QNplCad_SOURCE_FILES})


set_target_properties(QNplCad PROPERTIES OUTPUT_NAME "QNplCad")
set_target_properties(QNplCad PROPERTIES DEBUG_OUTPUT_NAME "QNplCad_d")

# Use the Widgets module from Qt 5.
target_link_libraries(QNplCad Qt5::Widgets)


ADD_CUSTOM_COMMAND(
	TARGET QNplCad
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QNplCad> ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy "${QT_ROOT}/bin/Qt5Cored.dll" "${OUTPUT_BIN_DIR}/Qt5Cored.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${QT_ROOT}/bin/Qt5Widgetsd.dll" "${OUTPUT_BIN_DIR}/Qt5Widgetsd.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${QT_ROOT}/bin/Qt5Guid.dll" "${OUTPUT_BIN_DIR}/Qt5Guid.dll"
)



