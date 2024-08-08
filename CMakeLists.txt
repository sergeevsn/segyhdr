cmake_minimum_required(VERSION 3.1)

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

find_package(Boost REQUIRED COMPONENTS program_options)

#set(Boost_INCLUDE_DIRS C:/Program Files/boost_1_82_0)
#set(Boost_LIBRARY_DIRS  C:/Program Files/boost_1_82_0/stage/lib)
#set(Boost_LIBRARIES boost_program_options)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


# Add your executable
add_executable(segyhdr ${SOURCES} ${HEADERS})

# Link the Boost program_options library
target_link_libraries(segyhdr ${Boost_LIBRARIES})
