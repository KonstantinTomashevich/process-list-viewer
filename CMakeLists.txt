project (ProcessListViewer)
cmake_minimum_required (VERSION 2.8)
set (CMAKE_C_STANDARD 11)
add_executable (ProcessListViewer main.c array.h dwordhashmap.h dwordhashmap.c
    programdata.h programdata.c mainwindow.h mainwindow.c processinfowindow.h processinfowindow.c)

execute_process (COMMAND "windres" "-i" "resources.rc" "resources.o" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
target_link_libraries (${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/resources.o libcomctl32.a)

