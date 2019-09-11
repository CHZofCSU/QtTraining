#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QButtonGroup>
#include <QTimer>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();
signals:
    void signalOfLoginSuccess();//补货员登录成功的信号


public slots:
    void checkLoginAndJump();//检验补货员登录密码等信息是否正确，若正确的跳转页面
    void sendVerificationCode();//从数据库中得到补货员的邮箱，生成验证码并发送
    void commitChangePass();//先检查密码修改是否正确后，修改数据库中的数据，并跳回登录界面
    void delayForCode();//获得验证码的延迟

private:
    Ui::LoginDialog *ui;
    QString realCode;//补货员收到的验证码
    QList<QLineEdit *> lineEditList;//补货员窗口的所有lineEdit组
    QButtonGroup btnGroup;//补货员窗口的所有button组
    QTimer codeTimer;//用于设置获取验证码的延迟
};

#endif // LOGINDIALOG_H
