#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include<QPushButton>
#include<QJsonValue>
#include<QJsonObject>
#include<QJsonDocument>
#include<QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    QList<QTcpSocket * > socketlist;
    QString name;
    QString message;
    QLabel * status;
    QLabel * perm;
    QTcpServer * server;

    void decode(const QByteArray &);
    void showmessage(const QString & str);
    void ini();
    bool speakable;
    int count;

    QByteArray encode(const QString & name,const QString & message);

    ~MainWindow();
private slots:
    void on_actionopen_triggered();
    void slo_newconnnection();
    void slo_disconnection();
    void slo_revmess();
    void on_sendButton_clicked();

    void on_actionclose_triggered();

    void on_clearbutton_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();
    void on_actionforbid_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
