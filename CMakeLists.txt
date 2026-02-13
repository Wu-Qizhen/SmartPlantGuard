cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME SmartPlantGuard)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -u _printf_float # Enable printf
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    # System module
    Modules/System/Src/system_config.c
    Modules/System/Src/system_state.c
    Modules/System/Src/error_handler.c
    # Sensor module
    Modules/Sensor/Src/sensor_manager.c
    Modules/Sensor/Src/dht11.c
    Modules/Sensor/Src/soil_moisture.c
    Modules/Sensor/Src/light_sensor.c
    # Actuator module
    Modules/Actuator/Src/actuator_manager.c
    Modules/Actuator/Src/relay_driver.c
    Modules/Actuator/Src/pwm_driver.c
    # Controller module
    Modules/Controller/Src/controller_core.c
    Modules/Controller/Src/hysteresis_logic.c
    Modules/Controller/Src/storage_flash.c
    # Communication module
    Modules/Communication/Src/protocol.c
    Modules/Communication/Src/communication_manager.c
    Modules/Communication/Src/bluetooth_hc05.c
    Modules/Communication/Src/wifi_esp8266.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    # System module
    Modules/System/Inc
    # Sensor module
    Modules/Sensor/Inc
    # Actuator module
    Modules/Actuator/Inc
    # Controller module
    Modules/Controller/Inc
    # Communication module
    Modules/Communication/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
