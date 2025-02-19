#cmake_minimum_required(VERSION 3.30)
#project(gbdk_snake C)
#
#set(CMAKE_C_STANDARD 90)
#
#add_executable(gbdk_snake src/main.c)

cmake_minimum_required(VERSION 3.10)
project(gbdk_snake C)

# -----------------------------------------------------------------------------
# 1) Configure GBDK path
#    If you've set GBDK_HOME in your environment, you can use $ENV{GBDK_HOME}.
#    Otherwise, you can set it manually here.
# -----------------------------------------------------------------------------

set(GBDK_HOME $ENV{GBDK_HOME})
if(NOT GBDK_HOME)
    # Fallback if the env var is not set
#    set(GBDK_HOME "$HOME/gbdk")
    set(GBDK_HOME "/Users/owencarey/gbdk")
endif()

# Explicitly tell CLion's indexer about the GBDK headers:
include_directories("${GBDK_HOME}/include")

# -----------------------------------------------------------------------------
# 2) Source files
# -----------------------------------------------------------------------------

set(SNAKE_SOURCES
        src/main.c
        src/snake.c
        # Add more .c files as needed
)

# -----------------------------------------------------------------------------
# 3) Custom command to compile the ROM using LCC
# -----------------------------------------------------------------------------

# Create a dummy executable for CLion's indexer
# This won't produce a real .gb file. It just lets CLion see includes.
add_executable(gbdk_snake_dummy ${SNAKE_SOURCES})

# Tell the host compiler to "pretend" it's SDCC building for Game Boy
#target_compile_definitions(gbdk_snake_dummy
#        PRIVATE
#        __PORT_gb=1
#        __SDCC=1
##        INT8="signed char"
##        UINT8="unsigned char"
##        INT16="short"
##        UINT16="unsigned short"
#)

# The -Wa-l, -Wl-m, -Wl-j flags generate additional map/listing data.
# You can omit them if you don’t need the extra debug info.

# add_custom_command(OUTPUT ...): We list the .gb file as the output of the command.

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/snake.gb
        COMMAND ${GBDK_HOME}/bin/lcc
        -Wa-l
        -Wl-m
        -Wl-j
        -o snake.gb
        ${SNAKE_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${SNAKE_SOURCES}
        COMMENT "Building snake.gb with GBDK-2020"
)

# -----------------------------------------------------------------------------
# 4) Custom target to tie everything together
# -----------------------------------------------------------------------------

# DEPENDS ${SNAKE_SOURCES}: Ensures that the .gb is rebuilt if any source files change.
# add_custom_target(snake ALL ...): Invokes the custom command so that make or cmake --build . will produce the snake.gb.

add_custom_target(
        snake ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/snake.gb
)

# -----------------------------------------------------------------------------
# 5) Install / Copy the final .gb if desired
# -----------------------------------------------------------------------------

#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/snake.gb DESTINATION .)
