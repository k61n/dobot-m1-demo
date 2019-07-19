#ifndef MYDOBOT_H
#define MYDOBOT_H

#include <QObject>
#include "DobotDll.h"
#include "DobotType.h"
#include "mytypes.h"
#include <QVector>

class MyDobot : public QObject
{
    Q_OBJECT

public:
    explicit MyDobot(QObject *parent = nullptr);
    ~MyDobot();
    void initDobot(QByteArray IPaddress);
    Pose getCurrentPosition();

    void goHomeShngld();
    void goHome();
    void goPosition(float x, float y, float z, float r);
    void goJog(int index);

    void setAirPump(int status, int direction); //status 1 off 0 on; direction 0 suck 1 push
    void setOutput(uint8_t address, uint8_t level);

    alarmState getAlarms();
    void clearAlarms();

    QString getName();

public slots:

private:
    char deviceSN[64];
    char deviceName[64];
    QByteArray thisDobotIP;
    Pose currentPosition, futurePosition;
};

#endif // MYDOBOT_H
