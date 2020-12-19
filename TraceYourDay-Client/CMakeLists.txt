cmake_minimum_required(VERSION 3.17)
project(TraceYourDay C)

set(CMAKE_C_STANDARD 99)

link_libraries(X11)
link_libraries(pthread)

add_executable(TraceYourDay
        src/main.c
        src/record.c
        src/utils.c
        src/window_manager.c
        src/x11_utils.c
        src/dayrecord.c

        headers/utils.h
        headers/x11_utils.h
        headers/record.h
        headers/path.h
        headers/size.h
        headers/ipc_utils.h src/ipc_utils.c headers/strings.h src/signal_utils.c headers/signal_utils.h)
