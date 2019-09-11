#ifndef SMTP_H
#define SMTP_H
#include<QByteArray>
#include<QString>
#include<QTcpSocket>

class Smtp
{
public:
    Smtp(QByteArray username,QByteArray password);
    ~Smtp();
    void send(QByteArray recvaddr,QString subject,QString content);

private:
    QTcpSocket * clientsocket;
    QByteArray username;//发送方的账号
    QByteArray password;//发送方的授权码
    QByteArray recvaddr;//接收方的地址
    //以下是用于SMTP协议中的命令
    QByteArray mailfrom = "mail from:<";
    QByteArray rcptto = "rcpt to:<";
    QByteArray prefrom = "from:";
    QByteArray preto = "to:";
    QByteArray presubject ="subject:";

    QString subject; //主题
    QString content; //发送内容
};

#endif // SMTP_H
