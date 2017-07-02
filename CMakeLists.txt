cmake_minimum_required(VERSION 3.0)
project(QNplCad)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files


set(DEPS_ROOT "${CMAKE_SOURCE_DIR}/deps")

set(NPLRUNTIME_ROOT "${DEPS_ROOT}/NPLRuntime")
set(EIGEN_ROOT "${DEPS_ROOT}/Eigen")
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set(OUTPUT_BIN_DIR "${NPLRUNTIME_ROOT}/ParaWorld/bin64")
	set(QT_ROOT "D:\\Qt\\Qt5.6.2\\5.6\\msvc2015")
else()
	set(OUTPUT_BIN_DIR "${NPLRUNTIME_ROOT}/ParaWorld/bin32")
	set(QT_ROOT "D:\\Qt\\Qt5.6.2\\5.6\\msvc2015")
endif()

set (CMAKE_PREFIX_PATH ${QT_ROOT})

# Qt library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
# OpenGL & GLEW library
find_package(OpenGL REQUIRED)
# Boost library
find_package(Boost 1.61 REQUIRED) 

# include boost
include_directories("${Boost_INCLUDE_DIRS}")
# include glew-2.0.0
include_directories("${DEPS_ROOT}/glew-2.0.0/include")

# include paraengine
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient")
include_directories("${NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient/Core")
# include nplcad
include_directories("${CMAKE_SOURCE_DIR}/src")

#set project resources and icon resource
set(Project_Resources res/qnplcad.qrc res/gl/gl.qrc res/qt/qt.qrc)
set(Icon_Resource res/exe/qnplcad.rc)


#add resources to RCC
qt5_add_resources(Project_Resources_RCC ${Project_Resources})

# main
file (GLOB src_files "${PROJECT_SOURCE_DIR}/src/*.*")
SOURCE_GROUP("src" FILES ${src_files})
list(APPEND QNplCad_SOURCE_FILES ${src_files})

# controls
file (GLOB controls_files "${PROJECT_SOURCE_DIR}/src/controls/*.*")
SOURCE_GROUP("src\\controls" FILES ${controls_files})
list(APPEND QNplCad_SOURCE_FILES ${controls_files})

# renderer
file (GLOB renderer_files "${PROJECT_SOURCE_DIR}/src/renderer/*.*")
SOURCE_GROUP("src\\renderer" FILES ${renderer_files})
list(APPEND QNplCad_SOURCE_FILES ${renderer_files})

# documents
file (GLOB documents_files "${PROJECT_SOURCE_DIR}/src/documents/*.*")
SOURCE_GROUP("src\\documents" FILES ${documents_files})
list(APPEND QNplCad_SOURCE_FILES ${documents_files})

# shader
file (GLOB shader_files "${PROJECT_SOURCE_DIR}/res/gl/*.vert")
SOURCE_GROUP("shader" FILES ${shader_files})
list(APPEND QNplCad_SOURCE_FILES ${shader_files})
file (GLOB shader_files "${PROJECT_SOURCE_DIR}/res/gl/*.frag")
SOURCE_GROUP("shader" FILES ${shader_files})
list(APPEND QNplCad_SOURCE_FILES ${shader_files})


# Tell CMake to create the QNplCad executable
add_executable(QNplCad  ${Project_Resources_RCC} ${QNplCad_SOURCE_FILES} ${Icon_Resource})
qt5_use_modules(QNplCad Widgets OpenGL)
target_link_libraries(QNplCad ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

set_target_properties(QNplCad PROPERTIES OUTPUT_NAME "QNplCad")
set_target_properties(QNplCad PROPERTIES DEBUG_OUTPUT_NAME "QNplCad_d")

ADD_CUSTOM_COMMAND(
	TARGET QNplCad
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:QNplCad> ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> ${OUTPUT_BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::OpenGL> ${OUTPUT_BIN_DIR}
)



