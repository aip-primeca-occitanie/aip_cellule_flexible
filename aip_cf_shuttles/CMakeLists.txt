cmake_minimum_required(VERSION 2.8.3)
project(aip_cf_shuttles)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  genmsg
  message_generation
  aip_cf_aiguillages
)

add_message_files(
  FILES
  msgShuttleChange.msg
  msgShuttleCreate.msg
  Msg_Erreur.msg
  )

add_service_files(
  FILES
  shuttle_id.srv
  )

generate_messages(DEPENDENCIES std_msgs)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS roscpp message_runtime
  LIBRARIES aip_cf_shuttles_capteurs
  )

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(main_ShuttleManager
  src/main_ShuttleManager.cpp
  src/capteurs.cpp src/FileAttente.cpp
  )

target_link_libraries(main_ShuttleManager ${catkin_LIBRARIES})

add_dependencies(main_ShuttleManager
  ${PROJECT_NAME}_gencpp
  ${${shuttles}_EXPORTED_TARGETS}
  aip_cf_shuttles_generate_messages_cpp
  )

# Create library
add_library(aip_cf_shuttles_capteurs
  SHARED
  ./src/capteurs.cpp)

add_dependencies(aip_cf_shuttles_capteurs
  ${PROJECT_NAME}_gencpp
  ${${shuttles}_EXPORTED_TARGETS}
  aip_cf_shuttles_generate_messages_cpp
  )

# INSTALL
install(FILES package.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  )

install(TARGETS
  aip_cf_shuttles_capteurs
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
  )

install(FILES ./include/aip_cf_shuttles/capteurs.h
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  )
