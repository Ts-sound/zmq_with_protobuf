#####################################
# head
#####################################
cmake_minimum_required(VERSION 3.10.0)

message("#####################################")
message("PROJECT_NAME = ${PROJECT_NAME}")
message("#####################################")

set(PROJECT_NAME zmq_with_protobuf)

project(${PROJECT_NAME})

#####################################
# include
#####################################
include_directories(/usr/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)

#####################################
# package_inc
#####################################
file(GLOB_RECURSE PACKAGE_INC
  "src/include/*"	
)

message(STATUS "PACKAGE_INC")
foreach(file ${PACKAGE_INC})
  message(STATUS "${file}")
endforeach(file ${PACKAGE_INC})

#####################################
# package_src
#####################################
file(GLOB_RECURSE PACKAGE_SRC
  "src/src/*.cc" "src/src/*.cpp"
)

message(STATUS "PACKAGE_SRC")
foreach(file ${PACKAGE_SRC})
  message(STATUS "${file}")
endforeach(file ${PACKAGE_SRC})


#####################################
# link and compile
#####################################
set(CMAKE_CXX_FLAGS "-std=c++11 -g -O0")
add_compile_options(-rdynamic)

add_library(${PROJECT_NAME} SHARED ${PACKAGE_SRC})

target_link_libraries(${PROJECT_NAME} 
-lzmq
-lzmqpp
-lpthread)

#####################################
# install
#####################################
# INSTALL (
#   DIRECTORY src/include/${PROJECT_NAME}
#   DESTINATION ${install_path}/include
#   PATTERN "config.h" EXCLUDE
#   PATTERN ".svn" EXCLUDE
#   PATTERN "*.am" EXCLUDE
#   PATTERN "*.in" EXCLUDE
# )

# INSTALL (
#   TARGETS ${PROJECT_NAME}
#   ARCHIVE DESTINATION ${install_path}/lib
# )


#####################################
# 增加单元测试
#####################################
add_subdirectory(src/test)
