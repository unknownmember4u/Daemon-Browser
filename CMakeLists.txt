cmake_minimum_required(VERSION 3.10)
project(DaemonBrowserPoC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
cmake_policy(SET CMP0074 NEW)

set(CEF_ROOT "${CMAKE_SOURCE_DIR}/third_party/cef" CACHE PATH "CEF Root directory")

# Add the CEF directory to module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Find CEF
find_package(CEF REQUIRED)

# Remove -Werror which breaks on newer compilers
if(CEF_COMPILER_FLAGS)
    list(REMOVE_ITEM CEF_COMPILER_FLAGS "-Werror")
endif()
if(CEF_CXX_COMPILER_FLAGS)
    list(REMOVE_ITEM CEF_CXX_COMPILER_FLAGS "-Werror")
endif()
if(CEF_C_COMPILER_FLAGS)
    list(REMOVE_ITEM CEF_C_COMPILER_FLAGS "-Werror")
endif()

# Include CEF CMake definitions
add_subdirectory(${CEF_ROOT}/libcef_dll build_wrapper)

include_directories(${CEF_ROOT})

set(SOURCES
    src/main.cpp
    src/app.cpp
    src/request_handler.cpp
    src/browser_window.cpp
    src/security_state.cpp
    src/render_backend.cpp
    src/security_policy.cpp
    src/download_manager.cpp
    src/history_manager.cpp
    src/session_manager.cpp
    src/new_tab_page.cpp
    src/bookmark_manager.cpp
    src/bookmark_manager_page.cpp
    src/history_manager_page.cpp
    src/download_manager_page.cpp
    src/settings_page.cpp
)

add_executable(daemon_browser ${SOURCES})
target_link_libraries(daemon_browser libcef_dll_wrapper "${CEF_ROOT}/Release/libcef.so" ${CEF_STANDARD_LIBS})

# Set the compiler options for CEF on Linux
if(OS_LINUX)
    target_compile_definitions(daemon_browser PRIVATE ${CEF_COMPILER_DEFINES})
    target_compile_options(daemon_browser PRIVATE ${CEF_COMPILER_FLAGS})
    target_link_options(daemon_browser PRIVATE ${CEF_LINKER_FLAGS})
    
    # We must set the rpath so it finds libcef.so
    set_target_properties(daemon_browser PROPERTIES
        INSTALL_RPATH "${CEF_ROOT}/Release"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
    
    # Copy required runtime files to the output directory
    add_custom_command(TARGET daemon_browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CEF_ROOT}/Resources/locales" "$<TARGET_FILE_DIR:daemon_browser>/locales"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CEF_ROOT}/Resources/icudtl.dat"
        "${CEF_ROOT}/Resources/chrome_100_percent.pak"
        "${CEF_ROOT}/Resources/chrome_200_percent.pak"
        "${CEF_ROOT}/Resources/resources.pak"
        "${CEF_ROOT}/Release/libcef.so"
        "${CEF_ROOT}/Release/libEGL.so"
        "${CEF_ROOT}/Release/libGLESv2.so"
        "${CEF_ROOT}/Release/libvk_swiftshader.so"
        "${CEF_ROOT}/Release/libvulkan.so.1"
        "${CEF_ROOT}/Release/vk_swiftshader_icd.json"
        "${CEF_ROOT}/Release/v8_context_snapshot.bin"
        "$<TARGET_FILE_DIR:daemon_browser>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CEF_ROOT}/Release/chrome-sandbox"
        "$<TARGET_FILE_DIR:daemon_browser>/chrome-sandbox"
    )
endif()
