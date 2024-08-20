set(WISDOM_PROJECT_PATH "~/pico/wisdom_sensor_net")
get_filename_component(WISDOM_PROJECT_PATH "${WISDOM_PROJECT_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
set(WISDOM_PROJECT_PATH ${WISDOM_PROJECT_PATH} CACHE PATH "Root of Wisdom Repo" FORCE)

# Radio
message("wisdom_init: initializing pcf8523 driver")
add_subdirectory(${WISDOM_PROJECT_PATH}/drivers/pcf8523_rp2040 drivers/pcf8523_rp2040)

# Load local config
message("wisdom_init: loading local wisdom_config.cmake file")
include(wisdom_config.cmake)
