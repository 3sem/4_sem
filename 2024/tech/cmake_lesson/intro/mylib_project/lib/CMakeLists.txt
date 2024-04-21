cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
									# Если версия установленой программы
									# старее указаной, произoйдёт аварийный выход.

project(lib)				# Название проекта

set(SOURCE_LIB lib.cpp)		# Установка переменной со списком исходников

add_library(lib STATIC ${SOURCE_LIB})
