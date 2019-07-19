#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include <QDialog>
#include "mytypes.h"

namespace Ui {
class AlertDialog;
}

class AlertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlertDialog(QWidget *parent = nullptr);
    ~AlertDialog();

    void input(alarmState a);

private slots:
    void parseError(int a);

    void on_btnClearAlarm_clicked();
    void on_btnReboot_clicked();
    void on_btnExit_clicked();

signals:
    void on_SignalRequest(QString command);

private:
    Ui::AlertDialog *ui;
    uint8_t alertState;
};

int alarmStateToCode(alarmState a);

#endif // ALERTDIALOG_H
