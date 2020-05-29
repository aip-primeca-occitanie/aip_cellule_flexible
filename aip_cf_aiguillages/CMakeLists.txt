cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_aiguillages)

add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  message_generation
  roscpp
  rospy
  std_msgs
)

add_message_files(FILES
  Capteurs.msg Actionneurs.msg
  ExchangeSh.msg Msg_SensorState.msg)

generate_messages(DEPENDENCIES std_msgs)
catkin_package(CATKIN_DEPENDS roscpp message_runtime)


include_directories(${catkin_INCLUDE_DIRS})
include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(run_Aiguillage src/main_aiguillage.cpp src/Aiguillage.cpp)
target_link_libraries(run_Aiguillage ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(run_Aiguillage
  ${PROJECT_NAME}_gencpp
  ${catkin_EXPORTED_TARGETS}
  aip_cf_aiguillages_generate_messages_cpp
)
