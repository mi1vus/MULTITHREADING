cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
project(HTML_server)			# Название проекта

set(SOURCE_EXE HTTPServer.cpp)		# Установка переменной со списком исходников для исполняемого файла

set(SOURCE_LIB http_parser.c)			# Тоже самое, но для библиотеки

SET(GCC_COVERAGE_LINK_FLAGS    "-pthread")
#add_definitions(${GCC_COVERAGE_LINK_FLAGS})

add_library(parser STATIC ${SOURCE_LIB})	# Создание статической библиотеки с именем foo

add_executable(final ${SOURCE_EXE})	# Создает исполняемый файл с именем main

target_link_libraries(final parser ${GCC_COVERAGE_LINK_FLAGS})		# Линковка программы с библиотекой
