#!/bin/bash          
#
# Final launcher of the simulation
# Projet Long 2017 
# Authors: J.B. Blanc / C. Delage / E. Maldonado / M. Maurin / C. Tomé
# Original code author: Koen Lekkerkerker
# Thu 24 Apr 2014 

bold=$(tput bold)
normal=$(tput sgr0)
RED='\033[1;32m'
NC='\033[0m'

function showHelp(){
    echo 
    echo "${bold}-----------------------------------------------------------"
    echo "   Launcher for the Simualtion of the manufacturing cell"
    echo "                INP - ENSEEIHT - AIP PRIMECA "
    echo "-----------------------------------------------------------${normal}"
    echo 
    echo -e "Make sure that the file is executable ${bold}${RED}(chmod +x launch.sh)${normal} ${NC}"
    echo 
    echo "  To run it from command line use:"
    echo "     $ ./launch.sh [Simulation_File]"
    echo "     Example: $ ./launch.sh SimulationGauche"
    echo 
    echo "${bold}Authors:${normal} J.B. Blanc / C. Delage / E. Maldonado "
    echo "         M. Maurin / A. Quintana / C. Tomé"
    echo "         Mon 6 March 2017"
    echo "${bold}Original code author:${normal} Koen Lekkerkerker (Thu 24 Apr 2014)"
    echo "http://answers.ros.org/question/51474/can-i-run-a-bash-script-using-roslaunch/"
    echo 
}

if [ "$1" = "-h" ]; then
    showHelp
else 

echo "            Launching the 'commande locale' node "
echo "${bold}--------------------------------------------------------------"
   if [ "$1" = "" ]; then
   	echo "        The simulation file was not defined."
	echo "      Default file Simulation4Robots.ttt will be used."
	roslaunch launcher launch_alpha.launch nbRobot:="4" & 
   else
   	echo "        The simulation file chosen is $1.ttt"
	roslaunch launcher launch_alpha.launch nbRobot:=$1 &
   fi
echo "--------------------------------------------------------------${normal}"

    # Launch of the 'commande_locale' node
	
	
	#roslaunch launch_alpha.launch path:=$1 &
	shift

    # Wait... (10 seconds)
    	#sleep 10

    # Launch of the other nodes
	echo "Launching the other nodes " 
	#roslaunch launcher launch_beta.launch 
fi
