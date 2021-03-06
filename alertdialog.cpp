#include "alertdialog.h"
#include "ui_alertdialog.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

AlertDialog::AlertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertDialog)
{
    ui->setupUi(this);

}

AlertDialog::~AlertDialog()
{
    delete ui;
}

void AlertDialog::input(alarmState a)
{
    QVector<int> code = alarmStateToCode(a);
    for (int i=0; i<code.count(); i++){
        if (code[i] != 8*32-1) {
            ui->plainTextEdit->appendPlainText("Error code " + QString::number(code[i]) + " 0x" + QString::number(code[i], 16));
            parseError(code[i]);
        }
    }
}

void AlertDialog::parseError(int a)
{
    QFile file(":/resources/resources/alarms");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "can't open alarm list";
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if ((line[0]=='0')&&(line[1]=='x'))
            if (line == "0x" + QString::number(a, 16)) {
                QString msg="";
                do {
                    ui->plainTextEdit->appendPlainText(msg);
                    msg = stream.readLine();
                } while ((msg[0]!='0')&&(msg[1]!='x'));
                break;
            }
    }
    file.close();
}

void AlertDialog::on_btnClearAlarm_clicked()
{
    emit on_SignalRequest("clear");
    this->close();
}

void AlertDialog::on_btnReboot_clicked()
{
    emit on_SignalRequest("reboot");
    this->close();
}

void AlertDialog::on_btnExit_clicked()
{
    this->close();
}

QVector<int> alarmStateToCode(alarmState a)
{
    QVector<int> result;

    QString alarm = "";
    int len = sizeof (a);
    for (int i=0; i<len; i++) {
        alarm += QString::number(a.value[i]) + " ";
        if (a.value[i] != 0) {
            QString b = QString::number(a.value[i], 2);
            int j=b.length()-1, pos=1;
            while (b[j] != '1') {
                j--;
                pos++;
            }
            result.push_back(pos + 8*i - 1);
        }
    }

    if (result.count() == 0) result.push_back(8*32-1);
    return result;
}
