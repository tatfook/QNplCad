cmake_minimum_required(VERSION 3.0)
# https://github.com/euler0/mini-cmake-qt
project(QNplCad)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.

set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files

set(QT_ROOT "D:\\Qt\\Qt5.6.2\\5.6\\msvc2015")

set (CMAKE_PREFIX_PATH ${QT_ROOT})

set(NPLRUNTIME_ROOT "${CMAKE_SOURCE_DIR}/deps/NPLRuntime")
set(OUTPUT_BIN_DIR "${NPLRUNTIME_ROOT}/ParaWorld/bin32")

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Boost 1.61 REQUIRED) 

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient")
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient/Core")


set(SOURCE_FILES
    src/main.cc
    src/mainwindow.cc
    src/MyApp.cpp
)



# Tell CMake to create the helloworld executable
add_executable(QNplCad WIN32  ${SOURCE_FILES} ${META_FILES_TO_INCLUDE})

# set_target_properties(QNplCad
#     PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${NPLRUNTIME_ROOT}/ParaWorld/"
#     LIBRARY_OUTPUT_DIRECTORY "${NPLRUNTIME_ROOT}/ParaWorld/"
#     RUNTIME_OUTPUT_DIRECTORY "${NPLRUNTIME_ROOT}/ParaWorld/"
# )
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


