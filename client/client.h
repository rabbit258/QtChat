#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QHostAddress>
#include<QDateTime>
#include<QString>
#include<QColor>
#include<QFontDialog>
#include<QLabel>
#include<QPoint>
#include<QFont>
#include<QColorDialog>
#include<QFontDialog>
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QListWidget>
#include<QFile>
#include <QByteArray>
namespace Ui {
class client;
}

class client : public QMainWindow
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0);

    QLabel * status;
    QLabel * perm;
    QString name;
    QString message;
    void showmessage(const QString &);
    void ini();
    QTcpSocket * socket;
    bool speakable;
    ~client();
    QByteArray  encode(const QString & name,const QString & message);
    void decode(const QByteArray &);
private slots:
     void on_actionconnect_triggered();

     void on_actiondisconnect_triggered();

     void on_sendButton_clicked();
     void slo_messrec();

     void on_clearbutton_clicked();

     void on_toolButton_clicked();

     void on_toolButton_6_clicked();

     void on_toolButton_4_clicked();

     void on_toolButton_5_clicked();

private:
    Ui::client *ui;
};

#endif // CLIENT_H
