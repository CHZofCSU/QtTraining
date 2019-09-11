#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include "dbhelper.h"
#include <QSqlQuery>
#include <QSqlQuery>
#include "smtp.h"
#include <QDateTime>
//用于MD5加密引入的头文件
#include <QCryptographicHash>

int delay = 3;//用于获得验证码的延迟

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    /**窗口属性**/
    this->setWindowTitle("Replenisher Login");
    this->setFixedSize(560,420);
    ui->stackedWidget->setCurrentIndex(0);//默认为登录界面

    /**设置登录界面控件属性**/
    ui->frame_login_img->move(0,0);
    ui->frame_login_img->resize(240,420);
    ui->frame_login->move(240,0);
    ui->frame_login->resize(320,420);
    //accountnumber和password的img
    ui->label_login_accountNumber->resize(32,32);
    ui->label_login_accountNumber->move(30,40);
    ui->label_login_pass->resize(32,32);
    ui->label_login_accountNumber->setPixmap(QPixmap(":/images/images/account.png"));
    ui->label_login_pass->setPixmap(QPixmap(":/images/images/password.png"));
    //背景图片
    QPixmap loginPix(":/images/images/login_bg.png");
    ui->frame_login_img->setAutoFillBackground(true);
    QPalette loginPalette;
    loginPalette.setBrush(QPalette::Window,QBrush(loginPix));
    ui->frame_login_img->setPalette(loginPalette);

    //设置登录的图像logo
    ui->label_login_img->resize(300,176);
    ui->label_login_img->move(0,240);
    ui->label_login_img->setPixmap(QPixmap(":/images/images/replenisherLogin.png"));

    //设置登录控件等属性
    ui->frame_login->setStyleSheet("background-color:white;");
    ui->lineEdit_login_pass->setEchoMode(QLineEdit::Password);//输入框密码模式
    //login和forgetpass按钮设置
    ui->btn_login->resize(110,34);
    ui->btn_forgetPass->resize(110,34);
    ui->btn_login->setText("Login");
    ui->btn_forgetPass->setText("ForgetPass");

    btnGroup.addButton(ui->btn_forgetPass,0);
    btnGroup.addButton(ui->btn_forget_cancel,1);
    btnGroup.addButton(ui->btn_forget_commit,2);
    btnGroup.addButton(ui->btn_getCode,3);
    btnGroup.addButton(ui->btn_login,4);

    ui->btn_login->setStyleSheet("QPushButton{background-color:rgb(233,103,61);border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_forgetPass->setStyleSheet("QPushButton{background-color:rgb(255,80,76);border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");

    //输入框设置
    lineEditList.append(ui->lineEdit_login_accountNumber);
    lineEditList.append(ui->lineEdit_login_pass);
    lineEditList.append(ui->lineEdit_forget_accountNumber);
    lineEditList.append(ui->lineEdit_forget_pass1);
    lineEditList.append(ui->lineEdit_forget_pass2);
    lineEditList.append(ui->lineEdit_forget_code);

    for(int i=0; i<6; i++){

        lineEditList.at(i)->setStyleSheet("background:transparent;border-width:0 0 1px 0;border-style:outset;"
                                          "font-family:Arial,\"Microsoft YaHei\";");

    }
    ui->lineEdit_login_accountNumber->setPlaceholderText("AccountNumber");
    ui->lineEdit_login_pass->setPlaceholderText("Password");
    ui->lineEdit_forget_accountNumber->setPlaceholderText("AccountNumber");
    ui->lineEdit_forget_pass1->setPlaceholderText("Password1");
    ui->lineEdit_forget_pass2->setPlaceholderText("Password2");
    ui->lineEdit_forget_code->setPlaceholderText("Verification Code");
    ui->lineEdit_login_accountNumber->move(90,40);

    /**设置忘记密码界面控件属性**/
    ui->frame_forgetPass->setStyleSheet("background-color:white;");
    ui->frame_forgetPass->move(0,0);
    ui->frame_forgetPass->resize(560,420);
    ui->label_forget_bg->move(0,0);
    ui->label_forget_bg->resize(560,271);
    ui->label_forget_bg->setPixmap(QPixmap(":/images/images/forget_bg.png"));

    ui->frame_forgetMsg->setStyleSheet("background:transparent;");

    ui->label_forget_img->resize(230,216);
    ui->label_forget_img->move(-21,200);
    ui->label_forget_img->setPixmap(QPixmap(":/images/images/emptycart.png"));

    ui->lineEdit_forget_pass1->setEchoMode(QLineEdit::Password);//输入框密码模式
    ui->lineEdit_forget_pass2->setEchoMode(QLineEdit::Password);//输入框密码模式

    ui->label_forget_accountNumber->resize(32,32);
    ui->label_forget_pass1->resize(32,32);
    ui->label_forget_pass2->resize(32,32);

    ui->label_forget_accountNumber->setPixmap(QPixmap(":/images/images/account2.png"));
    ui->label_forget_pass1->setPixmap(QPixmap(":/images/images/password.png"));
    ui->label_forget_pass2->setPixmap(QPixmap(":/images/images/password.png"));

    //忘记密码确认和取消按钮，得到验证码按钮样式
    ui->btn_forget_commit->setStyleSheet("QPushButton{background-color:rgb(63,61,86);border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_forget_cancel->setStyleSheet("QPushButton{background-color:rgb(255,0,0);border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_forget_commit->setText("Commit");
    ui->btn_forget_cancel->setText("Cancel");

    ui->btn_getCode->setStyleSheet("QPushButton{background-color:rgb(63,61,86);border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_getCode->setText("Mail");
    //用于延迟的定时器
    codeTimer.setInterval(1000);
    connect(&codeTimer,&QTimer::timeout,this,&LoginDialog::delayForCode);

    /**鼠标事件**/
    //点击登录按钮
    connect(ui->btn_login,&QPushButton::clicked,this,&LoginDialog::checkLoginAndJump);
    //点击忘记密码，跳往忘记密码界面
    connect(ui->btn_forgetPass,&QPushButton::clicked,
            [this](){
                ui->stackedWidget->setCurrentIndex(1);
            });
    //点击获得验证码
    connect(ui->btn_getCode,&QPushButton::clicked,this,&LoginDialog::sendVerificationCode);
    //补货员点击确定修改密码按钮事件
    connect(ui->btn_forget_commit,&QPushButton::clicked,this,&LoginDialog::commitChangePass);
    //补货员点击取消忘记密码按钮后，跳转回登录界面
    connect(ui->btn_forget_cancel,&QPushButton::clicked,
            [this](){
                ui->stackedWidget->setCurrentIndex(0);
            });

}

//生产随机字符串
QString getRandomString(int length){

    //为随机值设定一个seed
    qsrand(QDateTime::currentMSecsSinceEpoch());
    const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int chrs_size = sizeof(chrs);

    char* ch = new char[length + 1];
    memset(ch, 0, length + 1);
    int randomx = 0;
    for (int i = 0; i < length; ++i)
    {
        randomx = rand() % (chrs_size - 1);
        ch[i] = chrs[randomx];
    }
    QString ret(ch);
    delete[] ch;
    return ret;
}

//生成随机序号
QString getRandomNum(){
    QString num;
    int x = qrand()%10000;
    num = QString::number(x);
    return num;
}

//登录校验并跳转
void LoginDialog::checkLoginAndJump(){

    QString accountNumber = ui->lineEdit_login_accountNumber->text();
    QString pass = ui->lineEdit_login_pass->text();
    QString realPass;
    //此处由于md5加密，需要将补货员输入在lineEdit上的字符串同样加密
    QString passMD5;
    QByteArray temp;//用于转换类型
    temp = QCryptographicHash::hash(pass.toLatin1(),QCryptographicHash::Md5);
    passMD5.append(temp.toHex());

    if( accountNumber.isEmpty() | pass.isEmpty() ){
        QMessageBox::information(this,"Wrong Operation","Please fill in your name and password");
    }
    else{
        //DB操作，查询补货员信息
        QString sql = QString("select * from replenisher where account_number=%1").arg(accountNumber);
        QSqlQuery query(sql);
        while (query.next()) {
            realPass = query.value(2).toString();
        }
        if( realPass.isEmpty() ){   //没查到密码，说明是账号错误
            QMessageBox::warning(this,"Error","AccountNumber is wrong.Please sign in again.");
        }
        else{   //查到密码，账号正确，接着判断密码正确与否
            if( passMD5 == realPass ){
                //要清空lineEdit，否则会记录下来，下次登录会显示
                ui->lineEdit_login_accountNumber->clear();
                ui->lineEdit_login_pass->clear();
                //跳转界面，发送信号
                emit signalOfLoginSuccess();//给父窗口发送信号
                this->hide();
            }
            else{
                QMessageBox::warning(this,"Error","Wrong Password. Please sign in again.");
            }
        }
    }

}

//从数据库中获取补货员的邮箱，并发送验证码到邮箱
void LoginDialog::sendVerificationCode(){

    QString accountNumber = ui->lineEdit_forget_accountNumber->text();
    QString mail;

    QSqlQuery query;
    //此处就不用再将QString转为int了，因为是直接塞进sql语句中，只要在语句中不加''即可
    QString sql = QString("select * from replenisher where account_number = %1")
            .arg(accountNumber);
    query.exec(sql);
    while (query.next()) {
        mail = query.value(3).toString();
    }

    if(mail.isEmpty()){ //若取出的值为空，则accountNumber错误或者没填
        QMessageBox::warning(this,"Error","Wrong Information."
                     " Please fill in a correct accountNumber.");
    }
    else{
        //生成验证码
        realCode = getRandomString(6);//生成长度为6的随机验证码
        QString order = getRandomNum();//生成随机序号
        QString subject = "Change Password" + order;//生成邮件主题
        QString contents = "From vending machine: "
                           "your verification code is " + realCode;//生成邮件内容
        //服务端的邮箱，用于发送邮件
        Smtp smtp("test_for_qt@163.com","passwordofcode1");
        smtp.send(mail.toUtf8(),subject,contents); //最终邮件形式,并发送邮件

        //触发定时器,禁止用户多次点击
        codeTimer.start();

    }

}

//检查密码是否一致，验证码是否正确，并修改数据库，跳回登陆界面
void LoginDialog::commitChangePass(){

    QString accountNumber = ui->lineEdit_forget_accountNumber->text();
    QString pass1 = ui->lineEdit_forget_pass1->text();
    QString pass2 = ui->lineEdit_forget_pass2->text();
    QString code = ui->lineEdit_forget_code->text();

    if(accountNumber.isEmpty()| pass1.isEmpty()|
            pass2.isEmpty()| code.isEmpty()){
        QMessageBox::warning(this,"Error","Incompleted Information!");
    }
    else if(pass1 != pass2 ){
        QMessageBox::warning(this,"Error","Inconsistent password!");
    }
    else if(pass1.length()<6){
        QMessageBox::warning(this,"Error","Length of your password shouldn't be less than 6.");
    }
    else if(code != realCode ){
        QMessageBox::warning(this,"Error","Inconsistent verification code!");
    }
    else{
        //修改数据库内容
        QSqlQuery query;
        QString sql = QString("update replenisher set password = MD5('%1') where account_number = %2")
                .arg(pass1).arg(accountNumber);
        query.exec(sql);
        QMessageBox::information(this,"Success","Change password successfully!");
        ui->stackedWidget->setCurrentIndex(0);//跳回登录界面
    }
}
//获得验证码的延迟
void LoginDialog::delayForCode(){


    if(delay!=0){
        delay = delay-1;

        ui->btn_getCode->setFlat(true);
        ui->btn_getCode->setStyleSheet("QPushButton{background-color:grey;border-radius:17px;"
                                       "color:white;font-family:Arial,\"Microsoft YaHei\";}");
    }
    else{

        ui->btn_getCode->setFlat(false);
        ui->btn_getCode->setStyleSheet("QPushButton{background-color:rgb(63,61,86);border-radius:17px;"
                                       "color:white;font-family:Arial,\"Microsoft YaHei\";}");
        codeTimer.stop();
        delay = 3;//恢复时间
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
