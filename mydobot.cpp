#include "mydobot.h"
#include <QDebug>

MyDobot::MyDobot(QObject *parent) : QObject(parent)
{

}

MyDobot::~MyDobot()
{
    DisconnectDobot();
}

void MyDobot::initDobot(QByteArray IPaddress)
{
    thisDobotIP = IPaddress;
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);

    while (SetCmdTimeout(3000) != DobotCommunicate_NoError) {}
    while (SetQueuedCmdClear() != DobotCommunicate_NoError) {}
    while (SetQueuedCmdStartExec() != DobotCommunicate_NoError) {}

    while (GetDeviceSN(deviceSN, sizeof (deviceSN)) != DobotCommunicate_NoError) {}
    while (GetDeviceName(deviceName, sizeof(deviceName))  != DobotCommunicate_NoError) {}
    //qDebug() << IPaddress << "Device SN:" << deviceSN << "Device name" << deviceName;

    JOGJointParams jogJointParams;
    for (int i = 0; i < 4; i++) {
        jogJointParams.velocity[i] = 100;
        jogJointParams.acceleration[i] = 100;
    }
    while (SetJOGJointParams(&jogJointParams, false, nullptr) != DobotCommunicate_NoError) {}

    JOGCoordinateParams jogCoordinateParams;
    for (int i = 0; i < 4; i++) {
        jogCoordinateParams.velocity[i] = 100;
        jogCoordinateParams.acceleration[i] = 100;
    }
    while (SetJOGCoordinateParams(&jogCoordinateParams, false, nullptr)  != DobotCommunicate_NoError) {}

    JOGCommonParams jogCommonParams;
    jogCommonParams.velocityRatio = 100;
    jogCommonParams.accelerationRatio = 100;
    while (SetJOGCommonParams(&jogCommonParams, false, nullptr) != DobotCommunicate_NoError) {}

    PTPJointParams ptpJointParams;
    for (int i = 0; i < 4; i++) {
        ptpJointParams.velocity[i] = 100;
        ptpJointParams.acceleration[i] = 100;
    }
    while (SetPTPJointParams(&ptpJointParams, false, nullptr) != DobotCommunicate_NoError) {}

    PTPCoordinateParams ptpCoordinateParams;
    ptpCoordinateParams.xyzVelocity = 100;
    ptpCoordinateParams.xyzAcceleration = 100;
    ptpCoordinateParams.rVelocity = 100;
    ptpCoordinateParams.rAcceleration = 100;
    while (SetPTPCoordinateParams(&ptpCoordinateParams, false, nullptr) != DobotCommunicate_NoError) {}

    PTPCommonParams ptpCommonParams;
    ptpCommonParams.velocityRatio = 100;
    ptpCommonParams.accelerationRatio = 100;
    while (SetPTPCommonParams(&ptpCommonParams, false, nullptr) != DobotCommunicate_NoError) {}

    PTPJumpParams ptpJumpParams;
    ptpJumpParams.jumpHeight = 20;
    ptpJumpParams.zLimit = 150;
    while (SetPTPJumpParams(&ptpJumpParams, false, nullptr) != DobotCommunicate_NoError) {}
}

void MyDobot::setAirPump(int status, int direction)
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    IODO out18;
    out18.address = 18;
    out18.level = static_cast<uint8_t>(status);
    IODO out17;
    out17.address = 17;
    out17.level = static_cast<uint8_t>(direction);

    while (SetIODO(&out17, false, nullptr) != DobotCommunicate_NoError) {}
    while (SetIODO(&out18, false, nullptr) != DobotCommunicate_NoError) {}
}

void MyDobot::setOutput(uint8_t address, uint8_t level)
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    IODO out;
    out.address = address;
    out.level = level;
    while (SetIODO(&out, false, nullptr) != DobotCommunicate_NoError) {}
}

void MyDobot::goHomeShngld()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    while (SetQueuedCmdClear() != DobotCommunicate_NoError) {}

    Pose position;
    while (GetPose(&position) != DobotCommunicate_NoError) {}

    PTPCmd safePosition;
    safePosition.x = position.x;
    safePosition.y = position.y;
    safePosition.z = 100;
    safePosition.r = 0;
    safePosition.ptpMode = PTPMOVJXYZMode;

    while (SetPTPCmd(&safePosition, true, nullptr) != DobotCommunicate_NoError) {}

    safePosition.x = -150;
    safePosition.y = 140;

    while (SetPTPCmd(&safePosition, true, nullptr) != DobotCommunicate_NoError) {}

    while (SetHOMEWithSwitch(0, true, nullptr) != DobotCommunicate_NoError) {}

    safePosition.x = 470;
    safePosition.y = 0;
    safePosition.z = 100;
    while (SetPTPCmd(&safePosition, true, nullptr) != DobotCommunicate_NoError) {}

    safePosition.x = -150;
    safePosition.y = 140;
    while (SetPTPCmd(&safePosition, true, nullptr) != DobotConnect_NoError) {}
}

void MyDobot::goHome()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    SetQueuedCmdClear();
    while (SetHOMEWithSwitch(0, true, nullptr) != DobotCommunicate_NoError) {}
}

Pose MyDobot::getCurrentPosition()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);

    while (GetPose(&currentPosition) != DobotCommunicate_NoError) {}

    return currentPosition;
}

void MyDobot::goPosition(float x, float y, float z, float r)
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);

    PTPCmd PTPCmd;
    PTPCmd.ptpMode = PTPMOVJXYZMode;
    PTPCmd.x = x;
    PTPCmd.y = y;
    PTPCmd.z = z;
    PTPCmd.r = r;

    while (SetPTPCmd(&PTPCmd, true, nullptr) != DobotCommunicate_NoError) {}
}

void MyDobot::goJog(int index)
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);

    JOGCmd jogCmd;
    jogCmd.isJoint = index > 8;
    if (index > 8) index -= 8;
    jogCmd.cmd = static_cast<uint8_t>(index);
    while (SetJOGCmd(&jogCmd, false, nullptr) != DobotCommunicate_NoError) {}
}

alarmState MyDobot::getAlarms()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    alarmState result;
    uint32_t len, maxLen = 32;

    while (GetAlarmsState(result.value, &len, maxLen) != DobotCommunicate_NoError) {}
    return result;
}

void MyDobot::clearAlarms()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    while (ClearAllAlarmsState() != DobotCommunicate_NoError) {}
}

QString MyDobot::getName()
{
    ConnectDobot(thisDobotIP, 115200, nullptr, nullptr);
    return QString(deviceName);
}
