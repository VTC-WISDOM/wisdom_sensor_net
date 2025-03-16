set(WISDOM_PROJECT_PATH "~/WISDOM/wisdom_sensor_net")
get_filename_component(WISDOM_PROJECT_PATH "${WISDOM_PROJECT_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

set(WISDOM_PROJECT_PATH ${WISDOM_PROJECT_PATH} CACHE PATH "Root of Wisdom Repo" FORCE)
set(WISDOM_DRIVERS_PATH "${WISDOM_PROJECT_PATH}/drivers")
set(WISDOM_MODULES_PATH "${WISDOM_PROJECT_PATH}/modules")

# RFM69
message("wisdom_init: building rfm69_rp2040 library")
add_subdirectory(${WISDOM_DRIVERS_PATH}/rp2x_rfm69_rudp rp2x_rfm69_rudp)

# cellular
message("wisdom_init: building cell module")
add_subdirectory(${WISDOM_MODULES_PATH}/gateway gateway_module)

# scheduler
message("wisdom_init: building scheduler module")
add_subdirectory(${WISDOM_PROJECT_PATH}/modules/scheduler modules/scheduler)
