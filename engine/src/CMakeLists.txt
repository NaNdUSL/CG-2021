cmake_minimum_required(VERSION 3.5)

# Project Name
PROJECT(engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(${PROJECT_NAME} main.cpp)


# OpenGL Inclusion
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)



############ Include de Headers e libraries #################

#setup do Toolkits folder
message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

#setup do XML folder
message(STATUS "TinyXml2_DIR set to: " ${TINYXML2_FOLDER})
set(TINYXML2_FOLDER "" CACHE PATH "Path to TinyXml2 folder")

# verificar a existência dos files necessários
# Glut
if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
	message(ERROR ": GLUT not found")
endif (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")	

# Glew
if (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
	message(ERROR ": GLEW not found")
endif (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")	

# TinyXml2
if (NOT EXISTS "${TINYXML2_FOLDER}/tinyxml2.h" OR NOT EXISTS "${TINYXML2_FOLDER}/tinyxml2.cpp")
	message(ERROR ": TinyXml2 not found")
endif (NOT EXISTS "${TINYXML2_FOLDER}/tinyxml2.h" OR NOT EXISTS "${TINYXML2_FOLDER}/tinyxml2.cpp")	


# Devil
if (NOT EXISTS "${TOOLKITS_FOLDER}/devil/IL/il.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/devil.lib")
        message(ERROR ": DevIL not found")
    endif (NOT EXISTS "${TOOLKITS_FOLDER}/devil/IL/il.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/devil/devil.lib")


include_directories(${TOOLKITS_FOLDER}/glut ${TOOLKITS_FOLDER}/glew ${TOOLKITS_FOLDER}/devil ${TINYXML2_FOLDER})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} 
									  ${TOOLKITS_FOLDER}/glut/glut32.lib 
									  ${TOOLKITS_FOLDER}/glew/glew32.lib
									  ${TOOLKITS_FOLDER}/devil/devIL.lib )

if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll" AND EXISTS "${TOOLKITS_FOLDER}/devil/devil.dll")
	file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${TOOLKITS_FOLDER}/devil/devil.dll DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll" AND EXISTS "${TOOLKITS_FOLDER}/devil/devil.dll")	

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
