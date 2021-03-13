#include "client.h"
#include "ui_client.h"
client::client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::client)
{
    ui->setupUi(this);
    ini();
    socket = new QTcpSocket;
}

client::~client()
{
    delete ui;
}
void client::showmessage(const QString & str)
{
    QDateTime time=QDateTime::currentDateTime();
    ui->mess_rec->append(time.toString("MM-dd hh:mm:ss")+" "+str);
}
void client::ini()
{
    status=new QLabel(QString::fromUtf8("欢迎"));
    perm= new QLabel(QString::fromUtf8("网络聊天室"));
    ui->statusBar->addWidget(status);
    ui->statusBar->addPermanentWidget(perm);
    ui->actiondisconnect->setEnabled(false);
    ui->mess_rec->setReadOnly(true);
    speakable=true;
}

void client::on_actionconnect_triggered()
{
    if(!ui->nameedit->text().isEmpty())
    {
        ui->actionconnect->setEnabled(false);
        ui->actiondisconnect->setEnabled(true);
        socket->connectToHost(QHostAddress(ui->ipline->text()),ui->portline->text().toShort());

        if(socket->state()==QTcpSocket::UnconnectedState)
        {
            status->setText(QString::fromUtf8("连接失败"));
            return;
        }
        else
        {
            status->setText(QString::fromUtf8("连接成功"));
        }
        connect(socket,&QTcpSocket::readyRead,this,&client::slo_messrec,Qt::UniqueConnection);
        socket->write(encode(ui->nameedit->text(),QString::fromUtf8("大家好")));
    }
    else
        status->setText("你必须要有一个用户名");
}
void client::slo_messrec()
{
    decode(socket->readAll());
    showmessage(name+":\n"+message);
}

void client::on_actiondisconnect_triggered()
{
    socket->close();
    ui->actionconnect->setEnabled(true);
    ui->actiondisconnect->setEnabled(false);
}

void client::on_sendButton_clicked()
{
    if(speakable)
    {


        socket->write(encode(ui->nameedit->text(),ui->mess_send->toPlainText()));
        status->setText(QString::fromUtf8("发送成功"));
        ui->mess_send->clear();
    }
    else
    {
        status->setText(QString::fromUtf8("你已经被禁言"));
    }
}

void client::on_clearbutton_clicked()
{
    ui->mess_send->clear();
}

void client::on_toolButton_clicked()
{
    QFont  font=ui->mess_send->font();
    font.setBold(!font.bold());
    ui->mess_send->setFont(font);
}

void client::on_toolButton_6_clicked()
{
    QColor  color=ui->mess_send->textColor();
    QColorDialog  dialog;
    dialog.setCurrentColor(color);
    dialog.exec();
    color=dialog.selectedColor();
    ui->mess_send->setTextColor(color);
}

void client::on_toolButton_4_clicked()
{
    QFont  font =ui->mess_send->font();
    QFontDialog  dialog ;
    dialog.setCurrentFont(font);
    dialog.exec();
    font=  dialog.selectedFont();
    ui->mess_send->setFont(font);
}

void client::on_toolButton_5_clicked()
{
    QFont  font =ui->mess_send->font();
    font.setUnderline(!font.underline());
    ui->mess_send->setFont(font);
}
void client::decode(const QByteArray & raw)
{
    QJsonParseError error;
    QJsonDocument document=QJsonDocument::fromJson(raw,&error);

    if(error.error!=QJsonParseError::NoError)
    {
        qDebug()<<"json parsing error";
        if(document.isEmpty())
            qDebug()<<"null";
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
        speakable=object.value("speakable").toBool();
        ui->mess_send->setFont(font);
        ui->mess_send->setTextColor(color);
    }
}
QByteArray client::encode(const QString & name,const QString & message)
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
