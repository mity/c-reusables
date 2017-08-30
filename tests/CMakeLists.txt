
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole -static-libgcc")
    endif()
elseif(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /SUBSYSTEM:CONSOLE")
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
endif()


add_executable(test-base64 acutest.h test-base64.c ../encode/base64.h ../encode/base64.c)
target_include_directories(test-base64 PRIVATE ../encode)

add_executable(test-hex acutest.h test-hex.c ../encode/hex.h ../encode/hex.c)
target_include_directories(test-hex PRIVATE ../encode)

add_executable(test-fnv1a acutest.h test-fnv1a.c ../hash/fnv1a.h ../hash/fnv1a.c)
target_include_directories(test-fnv1a PRIVATE ../hash)

add_executable(test-cmdline acutest.h test-cmdline.c ../misc/cmdline.h ../misc/cmdline.c)
target_include_directories(test-cmdline PRIVATE ../misc)

add_executable(test-defs acutest.h test-defs.c ../misc/defs.h)
target_include_directories(test-defs PRIVATE ../misc)

add_executable(test-malloca acutest.h test-malloca.c ../misc/malloca.h)
target_include_directories(test-malloca PRIVATE ../misc)
