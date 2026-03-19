cmake_minimum_required(VERSION 3.10)
project(Marmot-C-API LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the C API shared library
add_library(marmot_c_api SHARED src/MarmotElement_C.cpp)

# Require the user to set MARMOT_INCLUDE_DIR and MARMOT_LIB_DIR
set(MARMOT_INCLUDE_DIR "" CACHE PATH "Path to Marmot include directory")
set(MARMOT_LIB_DIR "" CACHE PATH "Path to Marmot library directory")

if(MARMOT_INCLUDE_DIR STREQUAL "" OR MARMOT_LIB_DIR STREQUAL "")
    message(FATAL_ERROR "Both MARMOT_INCLUDE_DIR and MARMOT_LIB_DIR must be set per hand. Please configure them.")
endif()

if(MSVC)
    target_compile_definitions(marmot_c_api PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Add include directories for the C API
target_include_directories(marmot_c_api PUBLIC include)

# Include the user-provided Marmot headers
target_include_directories(marmot_c_api PRIVATE ${MARMOT_INCLUDE_DIR})

# Find and link the Marmot library strictly from the user-provided directory
find_library(MARMOT_LIB NAMES Marmot HINTS ${MARMOT_LIB_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(marmot_c_api PUBLIC ${MARMOT_LIB})

# Provide a standard export of headers if installed
install(TARGETS marmot_c_api
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

# --- Examples ---
# Build C example
add_executable(example_c examples/example.c)
target_link_libraries(example_c PRIVATE marmot_c_api)
set_target_properties(example_c PROPERTIES LINKER_LANGUAGE CXX)
