# Copyright 2015, akashche at redhat.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
if ( NOT DEFINED STATICLIB_CMAKE )
    set ( STATICLIB_CMAKE ${CMAKE_CURRENT_LIST_DIR}/cmake CACHE INTERNAL "" )
endif ( )
set ( CMAKE_TOOLCHAIN_FILE ${STATICLIB_CMAKE}/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )
set ( CMAKE_SHARED_LINKER_FLAGS /MANIFEST:NO )
set ( OPTIONS WIN32 )

# project
project ( java_winlaunch CXX )
include ( ${STATICLIB_CMAKE}/staticlibs_common.cmake )

# options
set ( JL_JAVA_RELATIVE_PATH "path/to/jre/bin/java" CACHE STRING "Relative path to java launcher" )
set ( JL_JAR_RELATIVE_PATH "path/to/launcher.jar" CACHE STRING "Relative path to application JAR" )
set ( JL_JAR_ARGUMENTS "-i foo -o bar" CACHE STRING "Java application arguments" )
set ( JL_ICON_PATH "path/to/icon.ico" CACHE STRING "Path to launcher icon" )

# resource file
configure_file ( ${CMAKE_CURRENT_LIST_DIR}/resources/resources.rc
        ${CMAKE_CURRENT_BINARY_DIR}/resources/resources.rc )

# executable
add_executable ( launcher 
    ${CMAKE_CURRENT_LIST_DIR}/src/launcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/resources/resources.rc )
target_include_directories ( launcher BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src )
target_compile_definitions ( launcher PRIVATE
        -DJL_JAVA_RELATIVE_PATH="${JL_JAVA_RELATIVE_PATH}" 
        -DJL_JAR_RELATIVE_PATH="${JL_JAR_RELATIVE_PATH}" 
        -DJL_JAR_ARGUMENTS="${JL_JAR_ARGUMENTS}" )
set_target_properties ( launcher PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
set_target_properties ( launcher PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )

# manifest
add_custom_command( TARGET launcher POST_BUILD COMMAND
        "mt.exe" -manifest \"${CMAKE_CURRENT_LIST_DIR}/resources/launcher.exe.manifest\" -outputresource:\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/launcher.exe\"
        COMMENT
        "Embedding manifest" )
