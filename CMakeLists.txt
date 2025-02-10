cmake_minimum_required(VERSION 3.5)

project(trgen)

set(CONTROLLER_DIR controller)
set(VIEW_DIR view)
set(MODEL_DIR model)
set(STATISTICS_DIR statistics)

include_directories(${CONTROLLER_DIR}
                    ${VIEW_DIR}
                    ${MODEL_DIR}
                    ${STATISTICS_DIR})

set(SOURCES 
            main.cpp
            ${MODEL_DIR}/core.cpp
            ${MODEL_DIR}/pinger.cpp
            ${VIEW_DIR}/cli.cpp
            ${CONTROLLER_DIR}/controller.cpp
            ${STATISTICS_DIR}/pingStatistics.cpp
            )
add_executable(${PROJECT_NAME} ${SOURCES})
