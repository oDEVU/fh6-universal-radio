cmake_minimum_required(VERSION 3.20)
project(fh6_universal_radio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8 /MP /EHsc /wd4324)
  add_compile_options("$<$<CONFIG:Release>:/O2;/GL;/Gy;/Gw>")
  add_link_options("$<$<CONFIG:Release>:/LTCG;/OPT:REF;/OPT:ICF>")
elseif(MINGW)
  # MinGW-w64 cross-compilation flags
  add_compile_options(-Wall -Wextra -Wpedantic)
  # Disable stack protection to avoid Wine/Proton issues with stack canaries
  add_compile_options(-fno-stack-protector)
  # NOTE: Disabled LTO on MinGW due to stack issues with Proton/Wine
  # add_compile_options("$<$<CONFIG:Release>:-O3;-flto>")
  add_compile_options("$<$<CONFIG:Release>:-O3>")
  # add_link_options("$<$<CONFIG:Release>:-flto>")
  # MinGW uses -fexceptions instead of /EHsc for exception handling
  add_compile_options(-fexceptions)
endif()

# Common Windows definitions for both MSVC and MinGW
add_compile_definitions(_WIN32_WINNT=0x0A00 WIN32_LEAN_AND_MEAN NOMINMAX
                        _CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)

add_library(version SHARED
    src/proxy/dll_main.cpp
    src/bridge.cpp
    src/log.cpp
    src/config.cpp
    src/config_store.cpp
    src/ring_buffer.cpp
    src/audio_source_manager.cpp
    src/fmod/pe_image.cpp
    src/fmod/sig_scanner.cpp
    src/fmod/radio_discovery.cpp
    src/fmod/dsp_bridge.cpp
    src/fmod/dsp_control_loop.cpp
    src/fmod/metadata_injector.cpp
    src/http/http_server.cpp
    src/sources/local_file_source.cpp
    src/sources/youtube_music_source.cpp
)

target_include_directories(version PRIVATE
    include
    third_party/nlohmann
    third_party/toml11
    third_party/miniaudio
)

target_link_libraries(version PRIVATE ws2_32 bcrypt psapi)

set_target_properties(version PROPERTIES
    OUTPUT_NAME version
    PREFIX ""
)

# Copy MinGW runtime DLLs to build directory
if(MINGW)
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-search-dirs
    OUTPUT_VARIABLE mingw_search_dirs
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(mingw_runtime_search_roots)
  foreach(search_line libraries programs)
    string(REGEX MATCH "${search_line}: =([^\n]+)" _ "${mingw_search_dirs}")
    if(CMAKE_MATCH_1)
      string(REPLACE ":" ";" search_paths "${CMAKE_MATCH_1}")
      list(APPEND mingw_runtime_search_roots ${search_paths})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES mingw_runtime_search_roots)

  foreach(runtime_dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
    unset(runtime_dll_path CACHE)
    find_file(runtime_dll_path
      NAMES ${runtime_dll}
      PATHS ${mingw_runtime_search_roots}
      NO_DEFAULT_PATH
    )

    if(runtime_dll_path)
      add_custom_command(TARGET version POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${runtime_dll_path}"
                $<TARGET_FILE_DIR:version>
        COMMENT "Copying ${runtime_dll} next to version.dll"
      )
    else()
      message(WARNING "Could not locate ${runtime_dll} under ${mingw_runtime_search_roots}")
    endif()
  endforeach()
endif()
