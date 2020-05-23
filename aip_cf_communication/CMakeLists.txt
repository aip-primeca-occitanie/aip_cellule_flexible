cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_communication)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  aip_cf_automates
  aip_cf_commande_locale
)



catkin_package(CATKIN_DEPENDS roscpp rospy std_msgs message_runtime)

include_directories(${catkin_INCLUDE_DIRS})


add_executable(start src/main_communication.cpp src/communication.cpp)
target_link_libraries(start ${catkin_LIBRARIES})
add_dependencies(start ${catkin_EXPORTED_TARGETS})
