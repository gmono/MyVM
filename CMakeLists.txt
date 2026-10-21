cmake_minimum_required(VERSION 3.20)

project(MyVM VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MYVM_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(MYVM_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            add_compile_options(-fno-pie)
            add_link_options(-no-pie)
        endif()
    else()
        message(FATAL_ERROR "MYVM_ENABLE_SANITIZERS requires GCC or Clang")
    endif()
endif()

include(FetchContent)
set(ANTLR_BUILD_CPP_TESTS OFF CACHE BOOL "" FORCE)
set(ANTLR_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(ANTLR_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(ANTLR4_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(antlr4_runtime
    GIT_REPOSITORY https://github.com/antlr/antlr4.git
    GIT_TAG 4.13.2
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR runtime/Cpp
)
FetchContent_MakeAvailable(antlr4_runtime)

set(MYVM_ANTLR_GENERATED
    ${CMAKE_CURRENT_SOURCE_DIR}/assembler/generated/MyVMAsmLexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/assembler/generated/MyVMAsmParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/assembler/generated/MyVMAsmVisitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/assembler/generated/MyVMAsmBaseVisitor.cpp
)
set_source_files_properties(${MYVM_ANTLR_GENERATED} PROPERTIES GENERATED TRUE)
add_library(myvm-assembler STATIC assembler/Assembler.cpp ${MYVM_ANTLR_GENERATED})
target_include_directories(myvm-assembler PUBLIC assembler PRIVATE assembler/generated)
target_link_libraries(myvm-assembler PUBLIC antlr4_static)
set_target_properties(myvm-assembler PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)

add_executable(myvm-asm assembler/main.cpp)
target_link_libraries(myvm-asm PRIVATE myvm-assembler)

add_library(myvm-translator STATIC translator/Translator.cpp)
target_include_directories(myvm-translator PUBLIC translator)
target_compile_features(myvm-translator PUBLIC cxx_std_23)

add_executable(myvm-translate translator/main.cpp)
target_link_libraries(myvm-translate PRIVATE myvm-translator)

option(MYVM_BUILD_X86 "Build the Zydis-backed x86-64 decoder" ON)
if(MYVM_BUILD_X86)
    FetchContent_Declare(zydis
        GIT_REPOSITORY https://github.com/zyantific/zydis.git
        GIT_TAG v4.1.1
        GIT_SHALLOW TRUE)
    set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ZYDIS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ZYDIS_FEATURE_ENCODER OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(zydis)
    add_library(myvm-x86 STATIC x86/X86Decoder.cpp)
    target_include_directories(myvm-x86 PUBLIC x86)
    target_link_libraries(myvm-x86 PUBLIC Zydis)
    target_compile_features(myvm-x86 PUBLIC cxx_std_23)
    add_executable(myvm-x86-decode x86/main.cpp)
    target_link_libraries(myvm-x86-decode PRIVATE myvm-x86)
endif()

option(MYVM_BUILD_ZIG_DEMO "Build the Zig freestanding to MyVM demonstration" OFF)
if(MYVM_BUILD_ZIG_DEMO)
    find_program(ZIG_EXECUTABLE zig REQUIRED)
    set(MYVM_ZIG_ASM ${CMAKE_CURRENT_BINARY_DIR}/myvm-zig-demo.s)
    set(MYVM_ZIG_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/myvm-zig-demo.myvm.asm)
    set(MYVM_ZIG_BINARY ${CMAKE_CURRENT_BINARY_DIR}/myvm-zig-demo.bin)
    add_custom_command(OUTPUT ${MYVM_ZIG_BINARY}
        COMMAND ${ZIG_EXECUTABLE} build-obj ${CMAKE_CURRENT_SOURCE_DIR}/zig/demo.zig
            -target x86_64-freestanding -O ReleaseSmall
            -femit-asm=${MYVM_ZIG_ASM} -fno-stack-check -fno-stack-protector -fstrip
        COMMAND $<TARGET_FILE:myvm-translate> --zig-intel ${MYVM_ZIG_ASM} ${MYVM_ZIG_SOURCE}
        COMMAND $<TARGET_FILE:myvm-asm> ${MYVM_ZIG_SOURCE} -o ${MYVM_ZIG_BINARY}
        DEPENDS zig/demo.zig myvm-translate myvm-asm
        COMMENT "Compiling Zig freestanding demo through the MyVM two-level assembler"
        VERBATIM)
    add_custom_target(myvm-zig-demo DEPENDS ${MYVM_ZIG_BINARY})
endif()

find_package(Java COMPONENTS Runtime QUIET)
if(Java_JAVA_EXECUTABLE)
    add_custom_command(
        OUTPUT ${MYVM_ANTLR_GENERATED}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr/antlr-4.13.2-complete.jar
            -Dlanguage=Cpp -visitor -no-listener -Xexact-output-dir
            -o ${CMAKE_CURRENT_SOURCE_DIR}/assembler/generated
            ${CMAKE_CURRENT_SOURCE_DIR}/assembler/MyVMAsm.g4
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/assembler/MyVMAsm.g4
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr/antlr-4.13.2-complete.jar
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating the ANTLR4 C++ assembler parser"
        VERBATIM
    )
    add_custom_target(generate-assembler-parser
        DEPENDS ${MYVM_ANTLR_GENERATED}
    )
    add_dependencies(myvm-assembler generate-assembler-parser)
endif()

add_library(vmlib
    vmlib/mvm.cpp
    vmlib/mvm_execute.cpp
)
add_library(MyVM::vmlib ALIAS vmlib)
target_compile_features(vmlib PUBLIC cxx_std_23)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(vmlib
        PRIVATE MYVM_BUILDING_LIBRARY
        INTERFACE MYVM_USING_SHARED_LIBRARY
    )
endif()
target_include_directories(vmlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vmlib>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(vmlib PUBLIC Threads::Threads)
set_target_properties(vmlib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_library(myvm-lab-core STATIC
    tools/myvm-lab/LabSession.cpp
    tools/myvm-lab/InstructionView.cpp
)
target_include_directories(myvm-lab-core PUBLIC tools/myvm-lab)
target_link_libraries(myvm-lab-core PUBLIC MyVM::vmlib myvm-assembler)
target_compile_features(myvm-lab-core PUBLIC cxx_std_23)

option(MYVM_BUILD_LAB "Build the Dear ImGui desktop teaching tool" OFF)
if(MYVM_BUILD_LAB)
    find_package(SDL2 CONFIG QUIET)
    if(TARGET SDL2::SDL2)
        set(MYVM_SDL2_TARGET SDL2::SDL2)
    elseif(TARGET SDL2::SDL2-static)
        set(MYVM_SDL2_TARGET SDL2::SDL2-static)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
        set(MYVM_SDL2_TARGET PkgConfig::SDL2)
    endif()
    find_package(OpenGL REQUIRED)

    FetchContent_Declare(dear_imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.9b
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(dear_imgui)

    add_library(myvm-imgui STATIC
        ${dear_imgui_SOURCE_DIR}/imgui.cpp
        ${dear_imgui_SOURCE_DIR}/imgui_draw.cpp
        ${dear_imgui_SOURCE_DIR}/imgui_tables.cpp
        ${dear_imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${dear_imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${dear_imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${dear_imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(myvm-imgui PUBLIC ${dear_imgui_SOURCE_DIR})
    target_link_libraries(myvm-imgui PUBLIC ${MYVM_SDL2_TARGET} OpenGL::GL)

    add_executable(myvm-lab
        tools/myvm-lab/main.cpp
        tools/myvm-lab/LabWindow.cpp
        tools/myvm-lab/EditorPanel.cpp
        tools/myvm-lab/StateViews.cpp
    )
    target_link_libraries(myvm-lab PRIVATE myvm-lab-core myvm-imgui)
    target_compile_features(myvm-lab PRIVATE cxx_std_23)
endif()

if(BUILD_TESTING)
    add_executable(myvm-smoke test/smoke.cpp)
    target_link_libraries(myvm-smoke PRIVATE MyVM::vmlib)
    add_test(NAME myvm-smoke COMMAND myvm-smoke)

    add_executable(myvm-lab-tests test/lab_model_tests.cpp)
    target_link_libraries(myvm-lab-tests PRIVATE myvm-lab-core)
    add_test(NAME myvm-lab-model COMMAND myvm-lab-tests)

    add_executable(myvm-assembler-tests test/assembler_tests.cpp)
    target_link_libraries(myvm-assembler-tests PRIVATE myvm-assembler)
    add_test(NAME myvm-assembler-tests COMMAND myvm-assembler-tests)

    add_executable(myvm-translator-tests test/translator_tests.cpp)
    target_link_libraries(myvm-translator-tests PRIVATE myvm-translator)
    add_test(NAME myvm-translator-tests COMMAND myvm-translator-tests)

    if(MYVM_BUILD_X86)
        add_executable(myvm-x86-decoder-tests test/x86_decoder_tests.cpp)
        target_link_libraries(myvm-x86-decoder-tests PRIVATE myvm-x86)
        add_test(NAME myvm-x86-decoder-tests COMMAND myvm-x86-decoder-tests)
    endif()

    add_test(NAME myvm-asm-cli
        COMMAND ${CMAKE_COMMAND}
            -DASSEMBLER=$<TARGET_FILE:myvm-asm>
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/assembler-cli.asm
            -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/assembler-cli-test.bin
            -P ${CMAKE_CURRENT_SOURCE_DIR}/test/assembler_cli_test.cmake
    )

    add_test(NAME myvm-translate-cli
        COMMAND ${CMAKE_COMMAND}
            -DTRANSLATOR=$<TARGET_FILE:myvm-translate>
            -DASSEMBLER=$<TARGET_FILE:myvm-asm>
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/translator-source.asm
            -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/translator-output.bin
            -P ${CMAKE_CURRENT_SOURCE_DIR}/test/translator_cli_test.cmake
    )
endif()

configure_package_config_file(
    cmake/MyVMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MyVMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyVM
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MyVMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS vmlib EXPORT MyVMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS myvm-asm myvm-translate RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES vmlib/mvm.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT MyVMTargets NAMESPACE MyVM:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyVM)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MyVMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MyVMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyVM
)
