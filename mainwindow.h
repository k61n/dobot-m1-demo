#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mydobot.h"
#include "alertdialog.h"
#include <QSerialPort>
#include <QTimer>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_command_received(QString command);
    void readUdpData();

private slots:
    void on_getPoseTimer();
    void on_buttonSearch_clicked();
    void on_listDobots_activated(int index);

    void on_buttonCheckAlarm_clicked();
    void on_buttonClearAlarms_clicked();

    void SetPump();

    void on_buttonGoHomeSafe_clicked();
    void on_buttonGoHomeSimple_clicked();

    void on_buttonExecPtpJ_clicked();
    void on_buttonExecPtpL_clicked();
    void on_Jog_pressed(int index);
    void on_Jog_released();
    void on_radioPumpOn_toggled(bool checked);
    void on_radioPumpPush_toggled(bool checked);
    void on_listJogMode_currentIndexChanged(int index);
    void on_buttonClearQueue_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *getPoseTimer;
    //QList<QSerialPort*> *serials;
    QVector<MyDobot*> *dobot;
    QUdpSocket udpSocketGet;
};

#endif // MAINWINDOW_H
