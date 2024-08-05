cmake_minimum_required(VERSION 2.8)

project(segyhdr)
# Specify the source files and header files
set(SOURCES
    source/segy.cpp  
    source/sgyhdr.cpp     
)

set(HEADERS
   include/segy.h
   include/utils.h
)

include_directories(include)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# Add your executable
add_executable(segyhdr ${SOURCES} ${HEADERS})

# Link the Boost program_options library
target_link_libraries(segyhdr ${Boost_LIBRARIES})