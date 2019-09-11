#include "paydialog.h"
#include "ui_paydialog.h"
#include "dbhelper.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QMediaPlayer>

int drinkId;//全局存储饮料的id
int maxAmount;//全局存储取到的饮料数量
int salesAmount;//全局存储取到的饮料销量
double drinkPrice;//全局存储取到的饮料的单价

PayDialog::PayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PayDialog)
{
    ui->setupUi(this);
    /**设置付款窗口属性**/
    this->setWindowTitle("PayForDrink");
    this->setStyleSheet("background-color:white;");
    this->setFixedSize(560,420);

    /**设置tabWidget属性**/
    ui->tabWidget->setTabIcon(0,QIcon(":/images/images/wechat.png"));
    ui->tabWidget->setTabText(0,"Wechat");
    ui->tabWidget->setTabIcon(1,QIcon(":/images/images/ali.png"));
    ui->tabWidget->setTabText(1,"AliPay");

    /*设置tabWidget中的付款label*/
    ui->label_wechatPay->move(0,0);
    ui->label_wechatPay->resize(270,350);
    ui->label_wechatPay->setPixmap(QPixmap(":/images/images/wechatPay.png"));

    ui->label_aliPay->move(0,-5);
    ui->label_aliPay->resize(270,350);
    ui->label_aliPay->setPixmap(QPixmap(":/images/images/aliPay.png"));

    /*设置整个背景*/
    ui->frame_payMsg->move(290,0);
    ui->frame_payMsg->resize(270,420);
    ui->frame_payMsg->setStyleSheet("background:transparent;");
    ui->label_payMsgImg->move(0,0);
    ui->label_payMsgImg->resize(560,420);
    ui->label_payMsgImg->setPixmap(QPixmap(":/images/images/pay_bg.png"));

    //设置确认购买和取消按钮样式
    ui->btn_pay_commit->setStyleSheet("QPushButton{background-color:rgb(233,103,61);"
                                      "border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_pay_commit->setText("Buy");
    ui->btn_pay_cancel->setStyleSheet("QPushButton{background-color:rgb(0,132,250);"
                                      "border-radius:17px;color:white;font-family:Arial,\"Microsoft YaHei\";}");
    ui->btn_pay_cancel->setText("Cancel");

    //设置饮料和价格的图片和文字格式
    ui->label_drinkNameImg->resize(40,40);
    ui->label_drinkPriceImg->resize(32,32);
    ui->label_drinkPriceImg->setPixmap(QPixmap(":/images/images/money.png"));
    QPixmap drinkNamePix(":/images/images/drink.png");
    QPixmap fitpixmap = drinkNamePix.scaled(40, 40, Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
    ui->label_drinkNameImg->setPixmap(fitpixmap);//自适应label的图片

    ui->label_pay_drinkName->setStyleSheet("font-size:25px;color:rgb(0,132,250);font-family:Arial,\"Microsoft YaHei\";");
    ui->label_pay_drinkPrice->setStyleSheet("font-size:25px;color:rgb(233,103,61);font-family:Arial,\"Microsoft YaHei\";");

    //设置spinbox样式
    ui->spinBox_payAmount->setStyleSheet("QSpinBox:up-button{subcontrol-origin:border;"
                                                    "subcontrol-position:right;"
                                                    "image:url(:/images/images/add.png);"
                                                    "width:32px;"
                                                    "height:32px;}"
                                         "QSpinBox:down-button{subcontrol-origin:border;"
                                                    "subcontrol-position:left;"
                                                    "image:url(:/images/images/sub.png);"
                                                    "width:32px;"
                                                    "height:32px;}"
                                         "QSpinBox{color:rgb(233,103,61);font-size:20px;text-align:center;"
                                                    "font-weight:bold;}");

    /**鼠标事件**/
    //取消付款
    connect(ui->btn_pay_cancel,&QPushButton::clicked,
            [this](){
                this->hide();
            });
    //确定付款
    connect(ui->btn_pay_commit,&QPushButton::clicked,
            [this](){
                if(ui->spinBox_payAmount->value() == 0){
                    QMessageBox::warning(this,"Error","The number of drinks can't be 0!");
                }
                else{
                    //db操作减少饮料数量
                    changeDrinkAmount(drinkId, ui->spinBox_payAmount->value());
                    //播放语音，提示用户购买成功
                    QMediaPlayer *p = new QMediaPlayer(this);
                    p->setMedia(QUrl("qrc:/mp3/mp3/successfulPurchase.mp3"));
                    p->setVolume(90);
                    p->play();
                    //发信号给主窗口，传参，让其改变drinkMap的值
                    emit signalOfNewStock();
                    //弹窗，提示用户购买成功
                    QMessageBox::information(this,"Success","Successful shopping operation.");
                    this->hide();
                    ui->spinBox_payAmount->setValue(1);
                }
            });
}

//将得到的饮料信息写在控件上
void PayDialog::getDrinkId(int id){
    drinkId = id;
}
void PayDialog::getDrinkName(QString str){
    ui->label_pay_drinkName->setText(str);
}
void PayDialog::getDrinkPrice(QString str){

    drinkPrice = str.toDouble();
    ui->label_pay_drinkPrice->setText(QString::number(drinkPrice*ui->spinBox_payAmount->value()));
}
void PayDialog::getDrinkAmount(QString str){
    maxAmount = str.toInt();
    //购买饮料数量不能超过饮料的库存
    ui->spinBox_payAmount->setMaximum(str.toInt());
}
void PayDialog::getDrinkSalesAmount(QString str){
    salesAmount = str.toInt();
}

//更改饮料的数量
void PayDialog::changeDrinkAmount(int id, int buyAmount){
    //DB实例化
    DBHelper *dBHelper = DBHelper::getInstance();
    QSqlDatabase database = dBHelper->database;
    if (!database.open())
    {
        qDebug() << "Fail!" << database.lastError();
    }
    //更新饮料库存数量
    int newStock = maxAmount - buyAmount; //得到新的饮料总数
    QSqlQuery query;
    QString sql_stock = QString("update drink set stock = %1 where id = %2")
            .arg(newStock).arg(id);
    query.exec(sql_stock);

    //更新饮料的已售数量
    int newSalesStock = salesAmount + buyAmount;  //得到新的销售总数
    QString sql_sales_amount = QString("update drink set sales_amount = %1 where id = %2")
            .arg(newSalesStock).arg(id);
    query.exec(sql_sales_amount);
}
//算出购买饮料的总价并显示
void PayDialog::on_spinBox_payAmount_valueChanged(int amount)
{
    double total = amount*drinkPrice;
    ui->label_pay_drinkPrice->setText(QString::number(total));
}


PayDialog::~PayDialog()
{
    delete ui;
}

