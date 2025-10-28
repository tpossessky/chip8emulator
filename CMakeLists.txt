cmake_minimum_required(VERSION 3.24)

project(chip8 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_COMMAND: ${CMAKE_COMMAND}")
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")

add_executable(chip8
        main.c
        ChipCPU.c
        ChipCPU.h
        renderer.c
        renderer.h
)

# ------- Set up Homebrew paths ------- #
if(APPLE)
    # For Intel Macs
    set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
    # For Apple Silicon Macs, uncomment the line below instead:
    # set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})

    # Set PKG_CONFIG_PATH for Homebrew
    set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    # For Apple Silicon: set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

# ------- Use pkg-config to find SDL2 libraries ------- #
find_package(PkgConfig REQUIRED)

message("")
message(STATUS "FINDING SDL2")
pkg_check_modules(SDL2 REQUIRED sdl2)
message(STATUS "SDL2_FOUND: TRUE")
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_IMAGE")
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
message(STATUS "SDL2_IMAGE_FOUND: TRUE")
message(STATUS "SDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL2_IMAGE_LIBRARIES: ${SDL2_IMAGE_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_TTF")
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
message(STATUS "SDL2_TTF_FOUND: TRUE")
message(STATUS "SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")
message(STATUS "SDL2_TTF_LIBRARIES: ${SDL2_TTF_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_MIXER")
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
message(STATUS "SDL2_MIXER_FOUND: TRUE")
message(STATUS "SDL2_MIXER_INCLUDE_DIRS: ${SDL2_MIXER_INCLUDE_DIRS}")
message(STATUS "SDL2_MIXER_LIBRARIES: ${SDL2_MIXER_LIBRARIES}")
message("")

# ------- Include & Link ------- #
target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
)

# Add library directories BEFORE linking
target_link_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_IMAGE_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
        ${SDL2_MIXER_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
)

# Add compile flags
target_compile_options(${PROJECT_NAME} PRIVATE
        ${SDL2_CFLAGS_OTHER}
        ${SDL2_IMAGE_CFLAGS_OTHER}
        ${SDL2_TTF_CFLAGS_OTHER}
        ${SDL2_MIXER_CFLAGS_OTHER}
)

cmake_minimum_required(VERSION 3.24)

project(chip8 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_COMMAND: ${CMAKE_COMMAND}")
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")

add_executable(chip8emu
        main.c
        ChipCPU.c
        ChipCPU.h
        renderer.c
        renderer.h
)

# ------- Set up Homebrew paths ------- #
if(APPLE)
    # For Intel Macs
    set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
    # For Apple Silicon Macs, uncomment the line below instead:
    # set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})

    # Set PKG_CONFIG_PATH for Homebrew
    set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    # For Apple Silicon: set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

# ------- Use pkg-config to find SDL2 libraries ------- #
find_package(PkgConfig REQUIRED)

message("")
message(STATUS "FINDING SDL2")
pkg_check_modules(SDL2 REQUIRED sdl2)
message(STATUS "SDL2_FOUND: TRUE")
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_IMAGE")
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
message(STATUS "SDL2_IMAGE_FOUND: TRUE")
message(STATUS "SDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}")
message(STATUS "SDL2_IMAGE_LIBRARIES: ${SDL2_IMAGE_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_TTF")
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
message(STATUS "SDL2_TTF_FOUND: TRUE")
message(STATUS "SDL2_TTF_INCLUDE_DIRS: ${SDL2_TTF_INCLUDE_DIRS}")
message(STATUS "SDL2_TTF_LIBRARIES: ${SDL2_TTF_LIBRARIES}")

message("")
message(STATUS "FINDING SDL2_MIXER")
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
message(STATUS "SDL2_MIXER_FOUND: TRUE")
message(STATUS "SDL2_MIXER_INCLUDE_DIRS: ${SDL2_MIXER_INCLUDE_DIRS}")
message(STATUS "SDL2_MIXER_LIBRARIES: ${SDL2_MIXER_LIBRARIES}")
message("")

# ------- Include & Link ------- #
target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
)

# Add library directories BEFORE linking
target_link_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_IMAGE_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
        ${SDL2_MIXER_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
)

# Add compile flags
target_compile_options(${PROJECT_NAME} PRIVATE
        ${SDL2_CFLAGS_OTHER}
        ${SDL2_IMAGE_CFLAGS_OTHER}
        ${SDL2_TTF_CFLAGS_OTHER}
        ${SDL2_MIXER_CFLAGS_OTHER}
)

# Copy resources folder to bin directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
