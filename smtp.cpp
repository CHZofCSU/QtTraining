#include "smtp.h"
#include <QDebug>

Smtp::Smtp(QByteArray username,QByteArray password)
{
    if(username.contains("@163"))
    {
        this->username = username;
        this->password = password;
    }
}

void Smtp::send(QByteArray recvaddr,QString subject,QString content)
{
    this->recvaddr = recvaddr;
    this->subject = subject;
    this->content = content;
    QByteArray usernametmp = this->username;
    QByteArray recvaddrtmp = this->recvaddr;

    clientsocket=new QTcpSocket();
    //此处以163邮箱作为发送邮箱
    this->clientsocket->connectToHost("smtp.163.com",25,QTcpSocket::ReadWrite);
    this->clientsocket->waitForConnected(1000);

    this->clientsocket->waitForReadyRead(1000);

    //以下模拟telnet发送邮件过程
    this->clientsocket->write("HELO smtp.163.com\r\n");
    this->clientsocket->waitForReadyRead(1000);

    this->clientsocket->write("AUTH LOGIN\r\n");
    this->clientsocket->waitForReadyRead(1000);

    this->clientsocket->write(username.toBase64().append("\r\n"));
    this->clientsocket->waitForReadyRead(1000);

    this->clientsocket->write(password.toBase64().append("\r\n"));
    this->clientsocket->waitForReadyRead(1000);


    this->clientsocket->write(mailfrom.append(usernametmp.append(">\r\n")));
    this->clientsocket->waitForReadyRead(1000);

    this->clientsocket->write(rcptto.append(recvaddrtmp.append(">\r\n")));
    this->clientsocket->waitForReadyRead(1000);

    this->clientsocket->write("data\r\n");
    this->clientsocket->waitForReadyRead(1000);

    usernametmp = this->username;
    recvaddrtmp = this->recvaddr;

    this->clientsocket->write(prefrom.append(usernametmp.append("\r\n")));
    this->clientsocket->write(preto.append(recvaddrtmp.append("\r\n")));
    this->clientsocket->write(presubject.append(subject.toLocal8Bit().append("\r\n")));
    this->clientsocket->write("\r\n");
    this->clientsocket->write(content.toLocal8Bit().append("\r\n"));
    this->clientsocket->write(".\r\n");
    this->clientsocket->waitForReadyRead(1000);

    //完成信息的填写，退出
    this->clientsocket->write("quit\r\n");
    this->clientsocket->waitForReadyRead(1000);

}
Smtp::~Smtp()
{
    delete this->clientsocket;
}


