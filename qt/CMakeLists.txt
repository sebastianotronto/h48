cmake_minimum_required(VERSION 3.16)

project(nissyqt	VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appnissyqt
	main.cpp
	adapter.cpp adapter.h
	../cpp/nissy.h ../cpp/nissy.cpp
	../nissy.o
)

qt_add_qml_module(appnissyqt
	URI nissyqt
	VERSION 1.0
	QML_FILES Main.qml
)

target_link_libraries(appnissyqt
	PRIVATE Qt6::Quick
)
