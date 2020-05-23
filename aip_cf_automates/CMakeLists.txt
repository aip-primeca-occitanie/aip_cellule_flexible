cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_automates)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
)

add_message_files(FILES Entrees.msg Sorties.msg)

generate_messages(DEPENDENCIES std_msgs)

catkin_package(CATKIN_DEPENDS roscpp rospy std_msgs message_runtime)

include_directories(${catkin_INCLUDE_DIRS})


add_executable(connect
  src/automates.cpp
  src/API_schneider.cpp)

target_link_libraries(connect ${catkin_LIBRARIES} modbus)
add_dependencies(connect ${PROJECT_NAME}_gencpp)


