cmake_minimum_required(VERSION 3.13)

project(trgen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CONTROLLER_DIR controller)
set(VIEW_DIR view)
set(MODEL_DIR model)
set(OBSERVER_DIR observer)
set(STATISTICS_DIR ${VIEW_DIR}/statistics)
set(PAYLOAD_DIR ${MODEL_DIR}/payload)

include_directories(${CONTROLLER_DIR}
                    ${VIEW_DIR}
                    ${MODEL_DIR}
                    ${STATISTICS_DIR}
                    ${PAYLOAD_DIR}
                    ${OBSERVER_DIR}
                    
                    )

set(SOURCES 
            main.cpp
            ${MODEL_DIR}/core.cpp
            ${VIEW_DIR}/cli.cpp
            ${CONTROLLER_DIR}/controller.cpp
            ${STATISTICS_DIR}/pingStatistics.cpp
            ${PAYLOAD_DIR}/pinger.cpp
            ${PAYLOAD_DIR}/helper.cpp
            ${PAYLOAD_DIR}/ifacesList.cpp
            )

add_executable(${PROJECT_NAME} ${SOURCES})
