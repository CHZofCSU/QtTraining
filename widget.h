#ifndef WIDGET_H
#define WIDGET_H
#include <QTimer>
#include <QWidget>
#include <QMovie>
#include <QMap>
#include <QTcpSocket>
#include <QButtonGroup>
#include <QLabel>
#include <QSpinBox>

#include "citymanager.h"
#include "paydialog.h"
#include "logindialog.h"
//用于补货员界面热销榜快排的结构体
struct HottestDrink{
   int id;
   int salesAmount;
   QString drinkName;
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void updateDrinkMap();//执行该函数，则从DB中取一次值，更新一次容器Map
    void updateDrinkMsgMap();//同上
    void drinkIsAtSale(int id, QString isAtSale);//收到服务端的消息后，更新drinkMap的值，并控制不可购买并黑白
    //更新饮料1-12的信息
    void updateMsgOfDrink1();
    void updateMsgOfDrink2();
    void updateMsgOfDrink3();
    void updateMsgOfDrink4();
    void updateMsgOfDrink5();
    void updateMsgOfDrink6();
    void updateMsgOfDrink7();
    void updateMsgOfDrink8();
    void updateMsgOfDrink9();
    void updateMsgOfDrink10();
    void updateMsgOfDrink11();
    void updateMsgOfDrink12();

    void updateMachineStatus();//更新饮料机的状态

    void quickSort(HottestDrink a[],int p,int r);
    int partition(HottestDrink a[],int p, int r);

    void updateHottestDrink();//更新热销饮料数组


public slots:
    void clientRecvData();//用于饮料机接收信息后处理
    void countDown();//开场动画倒计时函数
    void getCurrentTime();//更新当前时间
    void showWeather(const QStringList &strList);//显示天气信息
    void labelMoveEnter(QString str,int order);//鼠标移入，带入一个图片路径
    void labelMoveLeave();//鼠标移出，则恢复图片路径
    void updateDrinkMapAndMsg();
    void addDrink(int id, int addAmount,int amount);//补货员补货操作

private:
    Ui::Widget *ui;
    QButtonGroup btnImgDrinkGroup;//饮料点击购买的按钮组
    QButtonGroup btnAddDrinkGroup;//补货员补货按钮组
    QList<QLabel *> drinkBgLabelList;//存储饮料的背景label指针的List
    QList<QFrame *> drinkMsgFrameList;//存储饮料的名字和价格的frame的List
    QList<QSpinBox *> addDrinkSpinBoxList;//存储饮料补货按钮的List
    QList<QLabel *> drinkAmountLabelList;//显示饮料库存的label的List
    QList<QLabel *> drinkNameLabelList;//显示饮料名字的label的List
    QList<QLabel *> drinkPriceLabelList;//显示饮料价格的label的List
    QList<QLabel *> banDrinkLabelList;//显示停售标志的label的List
    HottestDrink hottestDrinkList[12];//按销售数量排序的饮料数组


    QTimer *countdownTimer;//设置开场倒计时器
    QMovie *movie;//设置开场动画中间coder的gif
    QMovie *movie2;//设置开场动画右下drink的gif
    Citymanager cityManager;//用于得到天气信息的networkmanager对象
    QMap<int,QStringList> drinkMap;//存储饮料的map
    QMap<int,QStringList> drinkMsgMap;//存储饮料营养信息的map
    PayDialog pay_w;//付款的对话框
    LoginDialog login_w;//补货员登录的对话框
    QTcpSocket *clientSocket;//用于接受服务端的信息
};

#endif // WIDGET_H
