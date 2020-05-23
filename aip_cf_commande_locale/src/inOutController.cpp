#include "inOutController.h"

// inOutController doit pouvoir actualiser l'UI à chaque actualisation de capteurs
inOutController::inOutController(vrepController* vrepSA)
{
	vrepServiceAcces = vrepSA;
}

// Fonction Callback pour les capteurs sur les rails
void inOutController::SensorCallbackRail(const std_msgs::Int32::ConstPtr& msg)
{
	for(int i=1;i<=10;i++) SensorState.CP[i] = (msg->data & (int32_t)pow(2,i-1)) > 0;
	planifRailSensorState.publish(SensorState);
}

// Fonction Callback pour les capteurs des stops
void inOutController::SensorCallbackStop(const std_msgs::Int32::ConstPtr& msg)
{
	for(int i=1;i<=24;i++) SensorState.PS[i] = (msg->data & (int32_t)pow(2,i-1)) > 0;
	planifRailSensorState.publish(SensorState);
}

// Fonction Callback pour les capteurs sur les aiguillages
void inOutController::SensorCallbackSwitch(const std_msgs::Int32::ConstPtr& msg)
{
	for(int i=1;i<=12;i++){
		SensorState.DD[i] = (msg->data & (int32_t)pow(2,2*i-2)) > 0;
		SensorState.DG[i] = (msg->data & (int32_t)pow(2,2*i-1)) > 0;
	}
	planifRailSensorState.publish(SensorState);
}

// Fonction Callback pour les actionneurs sur les aiguillages
void inOutController::StateSwitchCallBack(const aip_cf_commande_locale::Msg_SwitchControl::ConstPtr&  msg)
{
	int SwitchRightControl(0), SwitchLeftControl(0), SwitchLockControl(0);
	std_msgs::Int32 Right, Left, Lock;

	SwitchControl.RD = msg->RD;
	SwitchControl.RG = msg->RG;
	SwitchControl.LOCK = msg->LOCK;

	for (int i=1;i<=12;i++){
		if (msg->RD[i]==true)
			SwitchRightControl+=pow(2,i-1);
		if (msg->RG[i]==true)
			SwitchLeftControl+=pow(2,i-1);
		if (msg->LOCK[i]==true)
			SwitchLockControl+=pow(2,i-1);
	}

	Right.data=SwitchRightControl;
	VREPSwitchControllerRight.publish(Right);
	Left.data=SwitchLeftControl;
	VREPSwitchControllerLeft.publish(Left);
	Lock.data=SwitchLockControl;
	VREPSwitchControllerLock.publish(Lock);
}

// Fonction Callback pour les actionneurs sur les stops
void inOutController::StateStopCallBack(const aip_cf_commande_locale::Msg_StopControl::ConstPtr&  msg)
{
	int StopControlInt(0), GoControlInt(0);
	std_msgs::Int32 Stop, Go;

	StopControl.STOP = msg->STOP;
	StopControl.GO = msg->GO;

	for (int i=1;i<=24;i++){
		if (msg->STOP[i]==true)
			StopControlInt+=pow(2,i-1);
		if (msg->GO[i]==true)
			GoControlInt+=pow(2,i-1);
	}

	Stop.data=StopControlInt;
	VREPStopController.publish(Stop);
	Go.data=GoControlInt;
	VREPGoController.publish(Go);
}

// Fonction Callback pour les actionneurs sur les ergots + capteurs des ergots
void inOutController::StatePinCallBack(const aip_cf_commande_locale::Msg_PinControl::ConstPtr&  msg)
{
	int PinOnControlInt(0), PinOffControlInt(0);
	std_msgs::Int32 PinOn, PinOff;

	PinControl.PINON = msg->PINON;
	PinControl.PINOFF = msg->PINOFF;

	for (int i=1;i<=8;i++){
		if (msg->PINON[i]==true)
			PinOnControlInt+=pow(2,i-1);
		if (msg->PINOFF[i]==true)
			PinOffControlInt+=pow(2,i-1);
	}
	PinOn.data=PinOnControlInt;

	PinOff.data=PinOffControlInt;

	for(int i=1;i<=8;i++) SensorState.CPI[i] = (PinOn.data & (int32_t)pow(2,i-1)) > 0;

	planifRailSensorState.publish(SensorState);
}

// On s'abonne aux topic de VREP et du noeud Commande + On se prepare à publier sur les topic de la commande_locale
void inOutController::init(ros::NodeHandle nh)
{
	// Subscribe
	VREPsubRailSensor = nh.subscribe("sim_ros_interface/RailSensor", 100, &inOutController::SensorCallbackRail, this);
	VREPsubStopSensor = nh.subscribe("sim_ros_interface/StopSensor", 100, &inOutController::SensorCallbackStop, this);
	VREPsubSwitchSensor = nh.subscribe("sim_ros_interface/SwitchSensor", 100, &inOutController::SensorCallbackSwitch, this);
	planifSubSwitchState = nh.subscribe("/commande/Simulation/Actionneurs_aiguillages", 100, &inOutController::StateSwitchCallBack, this);
	planifSubStopState = nh.subscribe("/commande/Simulation/Actionneurs_stops", 100, &inOutController::StateStopCallBack, this);
	planifSubPinState = nh.subscribe("/commande/Simulation/Actionneurs_pins", 100, &inOutController::StatePinCallBack, this);

	// Publishers
	VREPSwitchControllerRight = nh.advertise<std_msgs::Int32>("/sim_ros_interface/SwitchControllerRight", 100);
	VREPSwitchControllerLeft = nh.advertise<std_msgs::Int32>("/sim_ros_interface/SwitchControllerLeft", 100);
	VREPSwitchControllerLock = nh.advertise<std_msgs::Int32>("/sim_ros_interface/SwitchControllerLock", 100);
	VREPStopController = nh.advertise<std_msgs::Int32>("/sim_ros_interface/StopController", 100);
	VREPGoController = nh.advertise<std_msgs::Int32>("/sim_ros_interface/GoController", 100);
	planifRailSensorState = nh.advertise<aip_cf_aiguillages::Msg_SensorState>("/commande/Simulation/Capteurs", 100);

	// Initialisation des capteurs des Ergots
	for(int i=1;i<9;i++) SensorState.CPI[i]=0;

	ros::Duration(1).sleep();
}
