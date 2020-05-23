ARG UBUNTU_VERSION=18.04
FROM nvidia/opengl:1.0-glvnd-runtime-ubuntu${UBUNTU_VERSION}

ARG UBUNTU=bionic
ARG ROS_DISTRO=melodic

# ROS packages
RUN apt-get update -qqy && DEBIAN_FRONTEND=noninteractive apt-get install -qqy gnupg2  && rm -rf /var/lib/apt/lists \
 && echo "deb http://packages.ros.org/ros/ubuntu ${UBUNTU} main" > /etc/apt/sources.list.d/ros-latest.list \
 && apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654

RUN apt-get update -qqy && apt-get install -qqy wget xz-utils libglib2.0-0 
    
RUN /usr/bin/wget https://www.coppeliarobotics.com/files/CoppeliaSim_Edu_V4_0_0_Ubuntu18_04.tar.xz

RUN tar Jxvf ./CoppeliaSim_Edu_V4_0_0_Ubuntu18_04.tar.xz
RUN mv CoppeliaSim_Edu_V4_0_0_Ubuntu18_04 /opt

RUN apt-get install -qqy qt5dxcb-plugin
RUN apt-get install -qqy libqt5core5a
RUN apt-get install -qqy liblua5.1-0-dev
RUN apt-get install -qqy libavcodec-dev libavformat-dev libswscale-dev
RUN apt-get install -qqy libmodbus-dev

