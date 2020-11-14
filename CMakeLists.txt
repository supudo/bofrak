CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

SET(PROJECT_NAME "Bofrak")
PROJECT(${PROJECT_NAME})

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)

# Apple CMake file

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Os")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -S -emit-llvm")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-trace")

ADD_DEFINITIONS(-DBofrak_Debug_Timings)
ADD_DEFINITIONS(-DCMAKE_CXX_CPPCHECK)

# Warnings
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-extern-c-compat")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-constant-out-of-range-compare")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-multichar")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")

ADD_DEFINITIONS(-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM="${CMAKE_SOURCE_DIR}/utilities/gl/glincludes.hpp")

IF((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
ENDIF()

IF(APPLE)
  SET(MACOSX_BUNDLE true)
  SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})
  SET(MACOSX_BUNDLE_INFO_PLIST, "${CMAKE_SOURCE_DIR}/resources/Info.plist")
ENDIF(APPLE)

# Libraries
IF(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  MARK_AS_ADVANCED(COCOA_LIBRARY)
ELSE(APPLE)
  MESSAGE(FATAL_ERROR "Cocoa Libraries not found!")
ENDIF(APPLE)

FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
MARK_AS_ADVANCED(OpenGL_LIBRARY)

# Boost
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# GLM
INCLUDE(cmake/FindGLM.cmake)
IF(GLM_FOUND)
  INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})
ELSE(GLM_FOUND)
  MESSAGE(FATAL_ERROR "GLM not found!")
ENDIF(GLM_FOUND)

# SDL2
FIND_PACKAGE(SDL2)
IF(SDL2_FOUND)
  INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
ELSE(SDL2_FOUND)
  MESSAGE(FATAL_ERROR "SDL2 not found!")
ENDIF(SDL2_FOUND)

# GLFW3
FIND_PACKAGE(GLFW3 REQUIRED)
IF(GLFW3_FOUND)
  INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIRS})
ELSE(GLFW3_FOUND)
  MESSAGE(FATAL_ERROR "GLFW3 not found!")
ENDIF(GLFW3_FOUND)

INCLUDE_DIRECTORIES(.)

ADD_DEFINITIONS(-DDef_Bofrak_OpenGL_4x)
ADD_DEFINITIONS(-DGLM_ENABLE_EXPERIMENTAL)

SET(SOURCE_FILES
  ui/iconfonts/iconsfontawesome.hpp
  ui/iconfonts/iconsmaterialdesign.hpp
  settings/configutils.cpp
  settings/configutils.hpp
  settings/settings.cpp
  settings/settings.hpp
  settings/settingsstructs.hpp
  ui/uimanager.cpp
  ui/uimanager.hpp
  utilities/stb/stb_image.h
  utilities/stb/stb_image_write.h
  utilities/gl/glincludes.hpp
  utilities/gl/glincludes.hpp
  utilities/gl/glutils.cpp
  utilities/gl/glutils.hpp
  utilities/imgui/imconfig.h
  utilities/imgui/imgui.cpp
  utilities/imgui/imgui.h
  utilities/imgui/imgui_demo.cpp
  utilities/imgui/imgui_draw.cpp
  utilities/imgui/imgui_internal.h
  utilities/imgui/imgui_widgets.cpp
  utilities/imgui/imstb_rectpack.h
  utilities/imgui/imstb_textedit.h
  utilities/imgui/imstb_truetype.h
  utilities/imgui/imgui_impl_opengl3.cpp
  utilities/imgui/imgui_impl_opengl3.h
  utilities/imgui/imgui_impl_sdl.cpp
  utilities/imgui/imgui_impl_sdl.h
  utilities/imgui/imgui_impl_opengl3.cpp
  utilities/imgui/imgui_impl_opengl3.h
  utilities/imgui/imgui_impl_sdl.cpp
  utilities/imgui/imgui_impl_sdl.h
  utilities/input/controls.cpp
  utilities/input/controls.hpp
  objects/definitions.hpp
  objects/camera.hpp
  objects/camera.cpp
  objects/fractal.hpp
  objects/fractal.cpp
  bofrak.cpp
  bofrak.hpp
  main.cpp)

SET(Bofrak_Fonts
  resources/fonts/fontawesome-webfont.ttf
  resources/fonts/material-icons-regular.ttf)

SET(Bofrak_Textures
    resources/textures/pal.png)

SET(Bofrak_Images_xcassets
  resources/Images.xcassets/AppIcon.appiconset/Contents.json
  resources/Images.xcassets/AppIcon.appiconset/icon_128x128.png
  resources/Images.xcassets/AppIcon.appiconset/icon_128x128@2x.png
  resources/Images.xcassets/AppIcon.appiconset/icon_16x16.png
  resources/Images.xcassets/AppIcon.appiconset/icon_16x16@2x.png
  resources/Images.xcassets/AppIcon.appiconset/icon_256x256.png
  resources/Images.xcassets/AppIcon.appiconset/icon_256x256@2x.png
  resources/Images.xcassets/AppIcon.appiconset/icon_32x32.png
  resources/Images.xcassets/AppIcon.appiconset/icon_32x32@2x.png
  resources/Images.xcassets/AppIcon.appiconset/icon_512x512.png
  resources/Images.xcassets/AppIcon.appiconset/icon_512x512@2x.png)

SET(Bofrak_Misc
  resources/Bofrak.icns
  resources/Bofrak.ico
  resources/Bofrak_Settings.ini)

IF (Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND)
  IF (APPLE)
    INCLUDE(CMakeToolsHelpers OPTIONAL)

    ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})

    # Misc Files
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB Bofrak_Ini_Files "resources/*.ini")
    FILE(COPY ${Bofrak_Ini_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB Bofrak_Icns_Files "resources/*.icns")
    FILE(COPY ${Bofrak_Icns_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB Bofrak_Ico_Files "resources/*.ico")
    FILE(COPY ${Bofrak_Ico_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB Bofrak_Textures "resources/textures/*.*")
    FILE(COPY ${Bofrak_Textures} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/textures/")

    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/Info.plist")

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${COCOA_LIBRARY})
  ENDIF(APPLE)
ELSE(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND)
  MESSAGE(FATAL_ERROR "Some of the required libraries were not found. Please check the log.")
ENDIF(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenGL_LIBRARY})

IF(Boost_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

IF(GLM_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLM_LIBRARIES})
ENDIF(GLM_FOUND)

IF(SDL2_FOUND)
  STRING(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})
ENDIF(SDL2_FOUND)

IF(GLFW3_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLFW3_LIBRARIES})
ENDIF(GLFW3_FOUND)
