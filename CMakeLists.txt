cmake_minimum_required(VERSION 3.5)

project(trgen)

set(CONTROLLER_DIR controller)
set(VIEW_DIR view)
set(MODEL_DIR model)

include_directories(${CONTROLLER_DIR} ${VIEW_DIR} ${MODEL_DIR})

set(SOURCES 
            main.cpp
            ${MODEL_DIR}/core.cpp
            ${VIEW_DIR}/cli.cpp
            ${CONTROLLER_DIR}/controller.cpp
            )
add_executable(${PROJECT_NAME} ${SOURCES})
