# Collect all .c files in the lsm6ds3-pid directory
file(GLOB LSM6DS3_PID_SRCS "../lsm6ds3-pid/*.c")

# Collect all .c files in the generated/messages directory and its subdirectories
file(GLOB_RECURSE GENERATED_MESSAGES_SRCS "../generated/messages/**/*.c")

# Collect all include directories in the generated/messages directory and its subdirectories
file(GLOB_RECURSE GENERATED_MESSAGES_INCLUDES "../generated/messages/**/*.h")

# Extract unique directories from include files
set(GENERATED_MESSAGES_INCLUDE_DIRS "")
foreach(file_path ${GENERATED_MESSAGES_INCLUDES})
    get_filename_component(dir_path ${file_path} DIRECTORY)
    list(APPEND GENERATED_MESSAGES_INCLUDE_DIRS ${dir_path})
endforeach()
list(REMOVE_DUPLICATES GENERATED_MESSAGES_INCLUDE_DIRS)

idf_component_register(SRCS "example_server.c" "wifi_cmds.c" "main.c" "wifi_cmds.c" ${LSM6DS3_PID_SRCS}
                       INCLUDE_DIRS "." "../lsm6ds3-pid" ${GENERATED_MESSAGES_INCLUDE_DIRS})