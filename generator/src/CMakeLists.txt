cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(generator)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_executable(${PROJECT_NAME} main.cpp)

#setup do Toolkits folder
message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

# Devil
if (NOT EXISTS "${TOOLKITS_FOLDER}/devil/IL/il.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/devil.lib")
        message(ERROR ": DevIL not found")
    endif (NOT EXISTS "${TOOLKITS_FOLDER}/devil/IL/il.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/devil.lib")


include_directories(${TOOLKITS_FOLDER}/devil)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} 
									  ${TOOLKITS_FOLDER}/devil/devIL.lib )

if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll" AND EXISTS "${TOOLKITS_FOLDER}/devil/devil.dll")
	file(COPY ${TOOLKITS_FOLDER}/devil/devil.dll DESTINATION ${CMAKE_BINARY_DIR})
endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll" AND EXISTS "${TOOLKITS_FOLDER}/devil/devil.dll")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})