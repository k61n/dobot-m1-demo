#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QSignalMapper>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dobot = new QVector<MyDobot*>;
    on_buttonSearch_clicked();

    getPoseTimer = new QTimer(this);
    getPoseTimer->setObjectName("getPoseTimer");
    connect(getPoseTimer, &QTimer::timeout, this, &MainWindow::on_getPoseTimer);

    QSignalMapper *signalMapper = new QSignalMapper(this);
    //connect(ui->btnXmJ1m, &QPushButton::pressed, this, &QSignalMapper::map); //can't connect in a new way, so use the old syntax
    connect(ui->btnXmJ1m, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnXpJ1p, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnYmJ2m, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnYpJ2p, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnZmJ3m, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnZpJ3p, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnRmJ4m, SIGNAL(pressed()), signalMapper, SLOT(map()));
    connect(ui->btnRpJ4p, SIGNAL(pressed()), signalMapper, SLOT(map()));

    signalMapper->setMapping(ui->btnXpJ1p, 1);
    signalMapper->setMapping(ui->btnXmJ1m, 2);
    signalMapper->setMapping(ui->btnYpJ2p, 3);
    signalMapper->setMapping(ui->btnYmJ2m, 4);
    signalMapper->setMapping(ui->btnZpJ3p, 5);
    signalMapper->setMapping(ui->btnZmJ3m, 6);
    signalMapper->setMapping(ui->btnRpJ4p, 7);
    signalMapper->setMapping(ui->btnRmJ4m, 8);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(on_Jog_pressed(int)));

    connect(ui->btnXmJ1m, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnXpJ1p, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnYmJ2m, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnYpJ2p, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnZmJ3m, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnZpJ3p, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnRmJ4m, SIGNAL(released()), this, SLOT(on_Jog_released()));
    connect(ui->btnRpJ4p, SIGNAL(released()), this, SLOT(on_Jog_released()));
}

MainWindow::~MainWindow()
{
    delete dobot;
    delete ui;
}

void MainWindow::on_getPoseTimer()
{
    if (ui->listDobots->count() > 0)
    {
        Pose pose;
        GetPose(&pose);
        //while (GetPose(&pose) != DobotCommunicate_NoError) {}
        ui->txtJ1->setText("J1=" + QString::number(static_cast<double>(pose.jointAngle[0])));
        ui->txtJ2->setText("J2=" + QString::number(static_cast<double>(pose.jointAngle[1])));
        ui->txtJ3->setText("J3=" + QString::number(static_cast<double>(pose.jointAngle[2])));
        ui->txtJ4->setText("J4=" + QString::number(static_cast<double>(pose.jointAngle[3])));

        ui->txtX->setText("X=" + QString::number(static_cast<double>(pose.x)));
        ui->txtY->setText("Y=" + QString::number(static_cast<double>(pose.y)));
        ui->txtZ->setText("Z=" + QString::number(static_cast<double>(pose.z)));
        ui->txtR->setText("R=" + QString::number(static_cast<double>(pose.r)));

        uint32_t len, maxLen = 32;
        alarmState alarmstate;
        GetAlarmsState(alarmstate.value, &len, maxLen);
        //while (GetAlarmsState(alarmstate.value, &len, maxLen) != DobotCommunicate_NoError) {}
        int code = alarmStateToCode(alarmstate);
        if (code != 8*32-1) {
            ui->txtAlarms->setText("Alarm: 0x" + QString::number(code, 16));
        }
        else {
            ui->txtAlarms->setText("");
        }
    }
}

void MainWindow::on_buttonSearch_clicked()
{
    ui->listDobots->clear();
/*
    QList<QSerialPortInfo> listSerialPort = QSerialPortInfo::availablePorts();
    for (int i=0; i<listSerialPort.count(); i++) {
        if ((listSerialPort.at(i).vendorIdentifier() == 0x403) && (listSerialPort.at(i).productIdentifier() == 0x6001)) {
            qDebug() << listSerialPort.at(i).portName();
            QSerialPort *serialPort = new QSerialPort(listSerialPort.at(i).portName(), this);
            serialPort->setBaudRate(QSerialPort::Baud115200);
            serialPort->setDataBits(QSerialPort::Data8);
            serialPort->setParity(QSerialPort::NoParity);
            serialPort->setStopBits(QSerialPort::OneStop);
            qDebug() << serialPort->open(QIODevice::ReadWrite);

            MyDobot* a = new MyDobot();
            dobot->push_back(a);
            dobot->last()->initDobot(listSerialPort.at(i).portName().toUtf8());
            ui->listDobots->addItem(listSerialPort.at(i).portName());
            on_listDobots_activated(ui->listDobots->currentIndex());
        }
    }*/

    QHostAddress host;

    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (int i=0; i<list.count(); i++)
        if ((!list.at(i).isLoopback()) && (list.at(i).protocol() == QAbstractSocket::IPv4Protocol))
            host = list.at(i);

    QString subnet = host.toString().section('.',0,2);

    QByteArray data = "WhoisDobotM1";

    QUdpSocket udpSocketSend;
    udpSocketSend.writeDatagram(data);//need tocall it otherwise in Win get socket doesn't open
    udpSocketSend.bind(host, 54321, QAbstractSocket::ShareAddress);

    udpSocketGet.bind(host, udpSocketSend.localPort(), QAbstractSocket::ShareAddress);
    connect(&udpSocketGet, &QUdpSocket::readyRead, this, &MainWindow::readUdpData);

    for (int i=1; i<255; i++)
        udpSocketSend.writeDatagram(data, 32, QHostAddress("192.168.1." + QString::number(i)), 6000);
}

void MainWindow::on_listDobots_activated(int index)
{
    ui->txtConnectedDobotName->setText(dobot->at(index)->getName());
}

void MainWindow::on_buttonCheckAlarm_clicked()
{
    AlertDialog dialog;
    dialog.input(dobot->at(ui->listDobots->currentIndex())->getAlarms());
    connect(&dialog, &AlertDialog::on_SignalRequest, this, &MainWindow::on_command_received);
    dialog.exec();
}

void MainWindow::on_command_received(QString command)
{
    if (command == "clear") on_buttonClearAlarms_clicked();
    if (command == "reboot") {
        dobot->at(ui->listDobots->currentIndex())->getCurrentPosition();
        FirmwareReboot();
    }
}

void MainWindow::readUdpData()
{
    while (udpSocketGet.hasPendingDatagrams()) {
        QNetworkDatagram data = udpSocketGet.receiveDatagram();
        QByteArray ip = data.senderAddress().toString().toUtf8();
        QString name = QString(data.data()).section('_',0,0); // cuts "dobotM1" from "dobotM1_Dobot M1_0033\x00"
        if (name == "dobotM1") {
            MyDobot* a = new MyDobot();
            dobot->push_back(a);
            dobot->last()->initDobot(ip);
            ui->listDobots->addItem(ip);
            on_listDobots_activated(ui->listDobots->currentIndex());
        }
    }
    if (!getPoseTimer->isActive()) getPoseTimer->start(1000);
}

void MainWindow::on_buttonClearAlarms_clicked()
{
    dobot->at(ui->listDobots->currentIndex())->clearAlarms();
}

void MainWindow::SetPump()
{
    //bool status = false, direction = false;
    int intStatus, intDirection;

    if (ui->radioPumpOn->isChecked()) intStatus = 0;
        else intStatus = 1;
    if (ui->radioPumpSuck->isChecked()) intDirection = 0;
        else intDirection = 1;

    dobot->at(ui->listDobots->currentIndex())->setAirPump(intStatus, intDirection);
}

void MainWindow::on_buttonGoHome_clicked()
{
    dobot->at(ui->listDobots->currentIndex())->goHomeShngld();
}

void MainWindow::on_buttonGoHomeSimple_clicked()
{
    dobot->at(ui->listDobots->currentIndex())->goHome();
}

void MainWindow::on_buttonExecPtpJ_clicked()
{
    dobot->at(ui->listDobots->currentIndex())->goPosition(ui->txtPtpX->text().toFloat(), ui->txtPtpY->text().toFloat(), ui->txtPtpZ->text().toFloat(), ui->txtPtpR->text().toFloat());
}

void MainWindow::on_Jog_pressed(int index)
{
    dobot->at(ui->listDobots->currentIndex())->goJog(index + ui->listJogMode->currentIndex()*8);
}

void MainWindow::on_Jog_released()
{
    dobot->at(ui->listDobots->currentIndex())->goJog(0);
}

void MainWindow::on_buttonPickAndPlace_clicked()
{
    int dy = 30;
    float dz = 0.18;

    for (int j = 0; j<4; j++) {
        dobot->at(ui->listDobots->currentIndex())->goPosition(45, 205, 40, 0);
        dobot->at(ui->listDobots->currentIndex())->goPosition(45, 205, 26 - dz*j, 0);
        dobot->at(ui->listDobots->currentIndex())->setAirPump(0, 0); // pump on = 0, suck = 0
        dobot->at(ui->listDobots->currentIndex())->goPosition(45, 205, 40, 0);
        dobot->at(ui->listDobots->currentIndex())->goPosition(250, 30 - dy*j, 40, 90);
        dobot->at(ui->listDobots->currentIndex())->goPosition(250, 30 - dy*j, 26, 90);
        dobot->at(ui->listDobots->currentIndex())->setAirPump(1, 0); // pump on = 0, suck = 0
        dobot->at(ui->listDobots->currentIndex())->goPosition(250, 30 - dy*j, 40, 90);
    }
}

void MainWindow::on_radioPumpOn_toggled(bool checked)
{
    Q_UNUSED(checked);
    this->SetPump();
}

void MainWindow::on_radioPumpPush_toggled(bool checked)
{
    Q_UNUSED(checked);
    this->SetPump();
}

void MainWindow::on_listJogMode_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->btnXpJ1p->setText("X+");
        ui->btnXmJ1m->setText("X-");
        ui->btnYpJ2p->setText("Y+");
        ui->btnYmJ2m->setText("Y-");
        ui->btnZpJ3p->setText("Z+");
        ui->btnZmJ3m->setText("Z-");
        ui->btnRpJ4p->setText("R+");
        ui->btnRmJ4m->setText("R-");
    }
    else {
        ui->btnXpJ1p->setText("J1+");
        ui->btnXmJ1m->setText("J1-");
        ui->btnYpJ2p->setText("J2+");
        ui->btnYmJ2m->setText("J2-");
        ui->btnZpJ3p->setText("J3+");
        ui->btnZmJ3m->setText("J3-");
        ui->btnRpJ4p->setText("J4+");
        ui->btnRmJ4m->setText("J4-");
    }
}
