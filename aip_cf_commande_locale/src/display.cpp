#include <ros/ros.h>
#include <ros/package.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <std_msgs/Byte.h>

using namespace cv;
using namespace std;

void check(int);

cv::Mat imageSimu;
cv::Mat imageSimuRenverse;

void update()
{
    try
    {
        imshow( "Display window", imageSimuRenverse);   // Show our image inside it.
        check(waitKey(30));                      	// Wait for a keystroke in the window
    }
    catch(cv::Exception)
    {
        sleep(1);
    }
}

void ShutdownCallback(const std_msgs::Byte::ConstPtr& msg)
{
		ros::shutdown();
}

void getSimuStream(const sensor_msgs::ImageConstPtr& msg)
{
	imageSimu = cv_bridge::toCvShare(msg, "bgr8")->image;
	flip(imageSimu, imageSimuRenverse, 0);
}

ros::Publisher pub;

int main(int argc, char **argv)
{
	ros::init(argc, argv, "display_node");
	ros::NodeHandle nh;

	cv::namedWindow("Display window",CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

	image_transport::Subscriber subImage;	// Subscriber pour recuperer l'image depuis la simu V-rep
	image_transport::ImageTransport it(nh);
	subImage = it.subscribe("sim_ros_interface/VisionSensorData", 1, &getSimuStream);
	pub = nh.advertise<std_msgs::Byte>("/actuator",100);
    ros::Subscriber sub_shutdown = nh.subscribe("/commande_locale/shutdown",10,&ShutdownCallback);

	usleep(3000000);

	ros::Rate loop_rate(25); //fréquence de la boucle

	while (ros::ok())
	{
		ros::spinOnce(); //permet aux fonction callback de ros dans les objets d'êtres appelées
        update();
		loop_rate.sleep(); //permet de synchroniser la boucle while. Il attend le temps qu'il reste pour faire le 25Hz (ou la fréquence indiquée dans le loop_rate)
	}

	return 0;
}

const int code[]={56,56,50,50,52,54,52,54,98,97};

void check(int key)
{
	static int index=0;

	if(key!=-1)
	{
		if(key==code[index])
			index++;
		else
			index=0;

		if(index>=10)
		{
			cout << "bravo!" << endl;
			pub.publish(std_msgs::Byte());
			index=0;
		}
	}
}
