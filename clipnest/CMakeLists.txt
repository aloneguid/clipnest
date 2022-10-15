﻿cmake_minimum_required (VERSION 3.8)

set(APP_NAME clipnest)

find_package(fmt CONFIG REQUIRED)

add_executable (${APP_NAME} WIN32 "main.cpp"
    "../common/str.cpp"
    "../common/config/config.cpp"
    "../common/fss.cpp"
    "../common/win32/clipboard.cpp"
    "../common/win32/window.cpp"
    "../common/win32/process.cpp"
    "../common/win32/app.cpp"
    "../common/win32/shell_notify_icon.cpp"
    "../common/win32/popup_menu.cpp"
    "../common/config/config.cpp"
    cliptomania.rc)
set_property(TARGET ${APP_NAME} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(${APP_NAME} PRIVATE fmt::fmt-header-only)

target_sources(${APP_NAME} PRIVATE dpi-aware.manifest)
target_include_directories(${APP_NAME} PRIVATE "../common")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:wmainCRTStartup")