# Collect all .c files in the lsm6ds3-pid directory
file(GLOB LSM6DS3_PID_SRCS "../lsm6ds3-pid/*.c")
file(GLOB HAL_SOURCES "../HAL/*.cpp")

file(GLOB NANOPB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../nanopb/*.c)

file(GLOB_RECURSE COMM_SOURCES ../application/comm/*.cpp)
file(GLOB_RECURSE TELEMETRY_SOURCES ../application/telem/*.cpp)


# Collect all .c files in the generated/messages directory and its subdirectories
file(GLOB_RECURSE MESSAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../generated/messages/**/*.c)

idf_component_register(  
                         SRCS   
                         "example_server.c" 
                         "main.c" 
                         ${LSM6DS3_PID_SRCS} 
                         ${HAL_SOURCES} 
                         "app.cpp" 
                         ${MESSAGE_SOURCES}
                         ${COMM_SOURCES}
                         ${TELEMETRY_SOURCES}
                         ${NANOPB_SOURCES}
                    
                         INCLUDE_DIRS 
                         "." 
                         "../lsm6ds3-pid" 
                         "../devices" 
                         "../HAL" 
                         "../generated" 
                         "../nanopb"
                         "../application/comm"
                         "../application/telem")