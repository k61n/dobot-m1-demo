# dobot-m1-demo
A demo software to access basic functions of Dobot M1 written with Qt.

A custom class MyDobot is created to handle multiple DobotM1s as objects.

In this demo connection to DobotM1s is only possible via network (connection via serial port is not supported). In order to allow smoothless operation on both windows and linux machines, a self-made method to search DobotM1s was implemented. It is assumed IP addresses are assigned by dhcp server in local network.

Following functions are implemented:
1) jogging;
2) PTP move;
3) homing;
4) alarm management;
5) pump control;
6) position monitoring.

Position minitoring is handled through QTimer class, and due to connection timeouts sometimes may cause not immideate response of other commands. It was found that setting QTimer timeout to a value of 1 s was a good trade-off.
