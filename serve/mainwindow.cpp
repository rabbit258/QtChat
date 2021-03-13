#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<qdebug.h>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ini();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ini()
{
    status=new QLabel(QString::fromUtf8("欢迎"));
    perm= new QLabel(QString::fromUtf8("网络聊天室"));
    ui->statusBar->addWidget(status);
    ui->statusBar->addPermanentWidget(perm);
    ui->actionclose->setEnabled(false);
    ui->mess_rec->setReadOnly(true);
    speakable=true;

    count=0;
    ui->iplist->setRowCount(0);
}

void MainWindow::on_actionopen_triggered()
{
    ui->actionopen->setEnabled(false);
    ui->actionclose->setEnabled(true);
    server=new QTcpServer;
    server->listen(QHostAddress(ui->ipline->text()),ui->portline->text().toShort());
    connect(server,&QTcpServer::newConnection,this,&MainWindow::slo_newconnnection);
}

void MainWindow::showmessage(const QString & str)
{
    QDateTime time=QDateTime::currentDateTime();
    ui->mess_rec->append(time.toString("MM-dd hh:mm:ss")+" "+str);
}

void MainWindow::slo_newconnnection()
{

    QTcpSocket * socket=server->nextPendingConnection();

    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::slo_revmess);
    connect(socket,&QTcpSocket::disconnected,this,&MainWindow::slo_disconnection);


}

void MainWindow::slo_disconnection()
{
    QTcpSocket * socket=qobject_cast<QTcpSocket *>(QObject::sender());
    QString tempname;
    socketlist.removeOne(socket);
    for(int c=0;c< ui->iplist->rowCount();c++)
    {
        if(socket->peerAddress().toString()==ui->iplist->item(c,1)->text())
        {
            QTableWidgetItem * temp1=ui->iplist->item(c,0);
            tempname=temp1->text();
            QTableWidgetItem * temp2=ui->iplist->item(c,1);
            ui->iplist->removeRow(c);
            delete temp1;
            delete temp2;
            break;
        }
    }

    showmessage(tempname+QString::fromUtf8("断开连接"));
    status->setText(QString::asprintf("当前%d人在线",socketlist.count()));
}

void MainWindow::slo_revmess()
{
    int c;
    QTcpSocket * socket=qobject_cast<QTcpSocket *>(QObject::sender());
    decode(socket->readAll());
    showmessage(name+":\n"+message);
    for(auto socket:socketlist)
    {
        socket->write(encode(name,message));
    }
    for(c=0;c< ui->iplist->rowCount();c++)
    {
        if(socket->peerAddress().toString()==ui->iplist->item(c,1)->text())
        {
            if(name!=ui->iplist->item(c,0)->text())
            {
                for(auto socket:socketlist)
                {
                    socket->write(encode(ui->iplist->item(c,0)->text(),QString::fromUtf8("改名为")+name));
                }
                showmessage(ui->iplist->item(c,0)->text()+QString::fromUtf8("改名为")+name);

                ui->iplist->item(c,0)->setText(name);
            }

            break;
        }
    }
    if(c==ui->iplist->rowCount())
    {
       socketlist<<socket;
        QTableWidgetItem * nameitem= new QTableWidgetItem(name);
        QTableWidgetItem * ipitem=new QTableWidgetItem(socket->peerAddress().toString());

        ui->iplist->setRowCount(count+1);
        ui->iplist->setItem(count,0,nameitem);
        ui->iplist->setItem(count,1,ipitem);
        showmessage(QString::fromUtf8("新用户")+name+QString::fromUtf8("连接"));
        for(auto socket:socketlist)
            {
                socket->write(encode(ui->nameline->text(),QString::fromUtf8("欢迎")+name+QString::fromUtf8("的到来")));
            }
        status->setText(QString::asprintf("当前%d人在线",count+1));
        count++;
    }
}

void MainWindow::on_sendButton_clicked()
{
    if(socketlist.isEmpty())
    {
        status->setText(QString::fromUtf8("没有用户连接进来,不要自言自语"));
    }
    else
    {
        for(auto socket:socketlist)
        {
            socket->write(encode(ui->nameline->text(),ui->mess_send->toPlainText().toUtf8()));
        }

        showmessage(ui->nameline->text()+QString::fromUtf8(":\n")+ui->mess_send->toPlainText());
    }
    ui->mess_send->clear();
}

void MainWindow::on_actionclose_triggered()
{
    server->close();
    ui->actionopen->setEnabled(true);
}

void MainWindow::on_clearbutton_clicked()
{
    ui->mess_send->clear();
}

void MainWindow::on_toolButton_clicked()
{
    QFont  font=ui->mess_send->font();
    font.setBold(!font.bold());
    ui->mess_send->setFont(font);
}

void MainWindow::on_toolButton_2_clicked()
{
    QColor  color=ui->mess_send->textColor();
    QColorDialog  dialog;
    dialog.setCurrentColor(color);
    dialog.exec();
    color=dialog.selectedColor();
    ui->mess_send->setTextColor(color);
}

void MainWindow::on_toolButton_3_clicked()
{
    QFont  font =ui->mess_send->font();
    QFontDialog  dialog ;
    dialog.setCurrentFont(font);
    dialog.exec();
    font=  dialog.selectedFont();
    ui->mess_send->setFont(font);
}


void MainWindow::on_toolButton_4_clicked()
{
    QFont  font =ui->mess_send->font();
    font.setUnderline(!font.underline());
    ui->mess_send->setFont(font);
}

void MainWindow::on_actionforbid_triggered()
{
    if(speakable==true)
    {
    showmessage(ui->nameline->text()+QString::fromUtf8("开启了全体禁言"));
    ui->actionforbid->setText(QString::fromUtf8("关闭全体禁言"));
    speakable=false;
    for(auto socket:socketlist)
    {
        socket->write(encode(ui->nameline->text(),QString::fromUtf8("开启了全体禁言")));
    }

    }
    else
    {
        speakable=true;
        showmessage(ui->nameline->text()+QString::fromUtf8("关闭了全体禁言"));
        ui->actionforbid->setText(QString::fromUtf8("开启全体禁言"));
        for(auto socket:socketlist)
        {
            socket->write(encode(ui->nameline->text(),QString::fromUtf8("开启了全体禁言")));
        }

    }
}

QByteArray MainWindow:: encode(const QString & name,const QString & message)
{
    QJsonObject object;
    QJsonDocument document;
    QFont font=ui->mess_send->font();
    QColor color=ui->mess_send->textColor();
    object.insert("name",QJsonValue(name));
    object.insert("message",QJsonValue(message));
    object.insert("speakable",QJsonValue(speakable));
    object.insert("isbold",QJsonValue(font.bold()));
    object.insert("isunderline",QJsonValue(font.underline()));
    object.insert("fontfamily",QJsonValue(font.family()));
    object.insert("fontweight",QJsonValue(font.weight()));
    object.insert("fontpoint",QJsonValue(font.pointSize()));
    object.insert("fontr",QJsonValue(color.red()));
    object.insert("fontg",QJsonValue(color.green()));
    object.insert("fontb",QJsonValue(color.blue()));
    object.insert("fonta",QJsonValue(color.alpha()));
    document.setObject(object);
    return document.toJson();
}
void MainWindow::decode(const QByteArray & raw)
{
    QJsonParseError error;
    QJsonDocument document=QJsonDocument::fromJson(raw,&error);

    if(error.error!=QJsonParseError::NoError)
    {
        qDebug()<<"json parsing error";
        qDebug()<<error.errorString();
    }
    else
    {
        QJsonObject object=document.object();
        QFont font=ui->mess_rec->font();
        QColor color=ui->mess_rec->textColor();
        name=object.value("name").toString();
        message=object.value("message").toString();
        font.setBold(object.value("isbold").toBool());
        font.setUnderline(object.value("isunderline").toBool());
        font.setFamily(object.value("fontfamily").toString());
        font.setWeight(object.value("fontweight").toInt());
        font.setPointSize(object.value("fontpoint").toInt());
        color.setRed( object.value("fontr").toInt());
        color.setGreen( object.value("fontg").toInt());
        color.setBlue( object.value("fontb").toInt());
        color.setAlpha( object.value("fonta").toInt());
        ui->mess_send->setFont(font);
        ui->mess_send->setTextColor(color);
    }
}
