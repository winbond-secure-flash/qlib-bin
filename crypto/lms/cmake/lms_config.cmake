cmake_minimum_required(VERSION 3.12)

#############################################################################################################
#                                              LMS VARIABLES                                               #
#############################################################################################################
# LMS root directory
get_filename_component(LMS_ROOT
                       "${CMAKE_CURRENT_LIST_DIR}/../"
                       ABSOLUTE)
string(REPLACE "/" "\\" LMS_ROOT__WIN "${LMS_ROOT}")

if (NOT DEFINED MAIN_PROJECT)
    set(MAIN_PROJECT LMS)
    message("MAIN_PROJECT = ${MAIN_PROJECT}")
endif()

if (NOT DEFINED MAIN_PROJECT_ROOT)
    set(MAIN_PROJECT_ROOT ${LMS_ROOT})
    message("MAIN_PROJECT_ROOT = ${MAIN_PROJECT_ROOT}")
endif()

if (NOT DEFINED TARGET)
    set(TARGET None)
endif()

if (NOT DEFINED ARTIFACT_TYPE)
    set(ARTIFACT_TYPE None)
endif()

if (${TARGET} STREQUAL "release")
    set(CMAKE_BUILD_TYPE Release)
endif()

#############################################################################################################
#                                               LMS SOURCES                                                #
#############################################################################################################
file(GLOB_RECURSE LMS_SRC__C            "${LMS_ROOT}/src/*.c")
file(GLOB_RECURSE LMS_SRC__H            "${LMS_ROOT}/src/*.h")
set(LMS_SRC__H                          "${LMS_SRC__H}"
                                        "${LMS_ROOT}/inc/lms_api.h"
)
set(LMS_SRC__INCLUDES                   "${LMS_ROOT}/src"
                                        "${LMS_ROOT}/inc"
)
source_group("lms/src"                  FILES
                                        ${LMS_SRC__C}
                                        ${LMS_SRC__H}
)

#############################################################################################################
#                                             LMS TEST SOURCES                                              #
#############################################################################################################
if (${TARGET} STREQUAL "debug")
    set(LMS_TEST__C                         "${LMS_ROOT}/tests/compilation_tests/main.c")
    set(LMS_TEST__H                         "${LMS_ROOT}/tests/compilation_tests/load_lms_functions.h")
    set(LMS_TEST__INCLUDES                  "${LMS_ROOT}/tests/compilation_tests")
    source_group("lms/test"                 FILES
                                            ${LMS_TEST_SRC__C}
                                            ${LMS_TEST_SRC__H}
    )
    if (${ARTIFACT_TYPE} STREQUAL "executable")
        set(LMS_TEST__C                     
                                            ${LMS_TEST__C}
                                            "${LMS_ROOT}/tests/compilation_tests/load_lms_functions_using_code.c"
        )
        set(LMS_TEST__H                     
                                            ${LMS_TEST__H}
                                            "${LMS_ROOT}/tests/compilation_tests/load_lms_functions_using_code.h"
        )
    elseif(${ARTIFACT_TYPE} STREQUAL "library" OR ${ARTIFACT_TYPE} STREQUAL "shared_library")
        set(LMS_TEST__C                     
                                            ${LMS_TEST__C}
                                            "${LMS_ROOT}/tests/compilation_tests/load_lms_functions_using_dll.c"
        )
        set(LMS_TEST__H                     
                                            ${LMS_TEST__H}
                                            "${LMS_ROOT}/tests/compilation_tests/load_lms_functions_using_dll.h"
        )
    endif()
endif()

#############################################################################################################
#                                           LMS PLATFORM SOURCES                                            #
#############################################################################################################

set(LMS_PLATFORM__C
                                        "${LMS_ROOT}/winbond_stubs/winbond_sha256.c"
                                        "${LMS_ROOT}/winbond_stubs/winbond_nonce.c"
                                        "${LMS_ROOT}/winbond_stubs/winbond_random.c"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage.c"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage_in_file.c"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage_in_ram.c"
)
set(LMS_PLATFORM__H
                                        "${LMS_ROOT}/imports/sha256.h"
                                        "${LMS_ROOT}/imports/nonce.h"
                                        "${LMS_ROOT}/imports/random.h"
                                        "${LMS_ROOT}/imports/key_storage.h"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage.h"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage_in_file.h"
                                        "${LMS_ROOT}/winbond_stubs/winbond_key_storage_in_ram.h"
)
# LMS_BUILD_FROM_SOURCE_TREE set to ON when building for testing during development phase
# Default is OFF (release deliverables layout).
if(NOT DEFINED LMS_BUILD_FROM_SOURCE_TREE)
    set(LMS_BUILD_FROM_SOURCE_TREE OFF)
endif()

if(LMS_BUILD_FROM_SOURCE_TREE)
    set(LMS_COMMON_BOARD_API_DIR    "${LMS_ROOT}/../common_board_api")
    set(LMS_PLATFORM_SHA256_C       "${LMS_ROOT}/../win_sal/platform_sha256.c")
else()
    set(LMS_COMMON_BOARD_API_DIR    "${LMS_ROOT}/winbond_stubs")
    set(LMS_PLATFORM_SHA256_C       "${LMS_ROOT}/winbond_stubs/platform_sha256.c")
endif()

set(LMS_PLATFORM__INCLUDES          "${LMS_ROOT}/imports"
                                    "${LMS_ROOT}/winbond_stubs"
                                    "${LMS_COMMON_BOARD_API_DIR}"
)
if (${TARGET} STREQUAL "release")
    set(LMS_PLATFORM__C
                                        ${LMS_PLATFORM__C}
                                        "${LMS_PLATFORM_SHA256_C}"
    )
    set(LMS_PLATFORM__H
                                        ${LMS_PLATFORM__H}
                                        "${LMS_COMMON_BOARD_API_DIR}/common_platform_sha256.h"
    )
else ()
    set(LMS_PLATFORM__C
                                        ${LMS_PLATFORM__C}
                                        "${LMS_PLATFORM_SHA256_C}"
                                        "${LMS_ROOT}/tests/compilation_tests/test_winbond_key_storage.c"
    )
    set(LMS_PLATFORM__H
                                        ${LMS_PLATFORM__H}
                                        "${LMS_COMMON_BOARD_API_DIR}/common_platform_sha256.h"
                                        "${LMS_ROOT}/tests/compilation_tests/test_winbond_key_storage.h"
    )
endif ()

source_group("lms/platform"     FILES   ${LMS_PLATFORM__C} ${LMS_PLATFORM__H})


file(GLOB_RECURSE LMS_MD__ALL           "${LMS_ROOT}/*.md")

#############################################################################################################
#                                            LMS FINAL SOURCES                                              #
#############################################################################################################
set (LMS_SOURCES__ALL_C
                                        ${LMS_SRC__C}
                                        ${LMS_PLATFORM__C}
                                        ${LMS_TEST__C}
)
set (LMS_SOURCES__ALL_H
                                        ${LMS_SRC__H}
                                        ${LMS_PLATFORM__H}
                                        ${LMS_TEST__H}
)
set (LMS_SOURCES__ALL
                                        ${LMS_SOURCES__ALL_H}
                                        ${LMS_SOURCES__ALL_C}
)
set (LMS_SOURCES__general               ${LMS_SOURCES__ALL})

#############################################################################################################
#                                             LMS FINAL INCLUDES                                            #
#############################################################################################################
set (LMS__INCLUDES                      
                                        ${LMS_SRC__INCLUDES}
                                        ${LMS_PLATFORM__INCLUDES}
                                        ${LMS_TEST__INCLUDES}
)

#############################################################################################################
#                                               LMS DEFINES                                                 #
#############################################################################################################
set (LMS_DEFINES__RUNTIME                           USE_LMS_EXT)
set (LMS_DEFINES__RUNTIME__MUTUAL_EXCLUSIVE                             )
set (LMS_DEFINES__RUNTIME__FORCE                                        )
set (LMS_DEFINES__RUNTIME__ALWAYS                                       )
set (LMS_DEFINES__RUNTIME__NEVER                                        )
set (LMS_DEFINES__DEFAULT                       ${LMS_DEFINES__RUNTIME})

# All LMS defines
set (LMS_DEFINES_ALL                # ${LMS_DEFINES__DEBUG}
                                    # ${LMS_DEFINES__RELEASE_OPTIONS}
                                    # ${LMS_DEFINES__CHIP_FEATURES}
                                    ${LMS_DEFINES__RUNTIME}
                                    ${LMS_DEFINES__INTERNAL}
                                    # ${LMS_DEFINES__RUNTIME__ALWAYS}
)
