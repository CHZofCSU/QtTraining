/**BugLog**/
#include "widget.h"
#include "ui_widget.h"
#include <QMovie>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>
#include <QDateTime>
#include <QChar>
#include <QMediaPlayer>
#include <QTableWidget>

//DB headfile
#include "dbhelper.h"
#include <QSqlQuery>

int time = 18;//开头动画倒计时用

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    /***窗口属性***/
    this->setWindowTitle("Vending Machine");
    this->setFixedSize(1200,850);
    ui->stackedWidget->resize(1200,850);

    //补货员热销榜样式设置，此处表内容在updateDrinkMapAndMsg()中添加
    //因此样式要在该函数之前设定
    //设置补货员查看的热销榜的表格样式
    ui->hottestDrinkTable->setColumnCount(2);
    ui->hottestDrinkTable->setRowCount(13);
    ui->hottestDrinkTable->resize(272,485);
    ui->hottestDrinkTable->move(880,290);
    ui->hottestDrinkTable->verticalHeader()->setVisible(false);
    ui->hottestDrinkTable->horizontalHeader()->setVisible(false);
    ui->hottestDrinkTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hottestDrinkTable->setItem(0,0,new QTableWidgetItem("DrinkName"));
    ui->hottestDrinkTable->setItem(0,1,new QTableWidgetItem("SalesAmount"));
    ui->hottestDrinkTable->setStyleSheet("QTableWidget{background-color:rgba(233,103,61,0.6);"
                                                  "color:white;}");
    ui->hottestDrinkTable->setFrameShape(QFrame::NoFrame);//去除边框
    ui->hottestDrinkTable->setShowGrid(false);
    ui->hottestDrinkTable->item(0,0)->setTextColor(QColor(0,132,250));
    ui->hottestDrinkTable->item(0,1)->setTextColor(QColor(0,132,250));
    //内容居中
    ui->hottestDrinkTable->item(0,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->hottestDrinkTable->item(0,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);


    /***数据库起始操作***/
    DBHelper *dBHelper = DBHelper::getInstance();
    QSqlDatabase database = dBHelper->database;
    if (!database.open())
    {
        qDebug() << "DB Open Fail!" << database.lastError();
    }
    //初始就将信息放进drinkMap和drinkMsgMap和hottestDrinkList中
    //初始化饮料和饮料机和饮料热榜的信息
    updateDrinkMapAndMsg();


    /***饮料机和服务端点对点连接***/
    clientSocket = new QTcpSocket();
    clientSocket->connectToHost("127.0.0.1",7888);
    if(!clientSocket->waitForConnected(10000))
    {
        QMessageBox::information(this, "QT_Network", "Connection failed!");
    }
    connect(clientSocket,&QTcpSocket::readyRead,this,&Widget::clientRecvData);

    /***控件属性***/
    /**欢迎界面动画控件**/
    //gif
    ui->label_p1_gif->setMinimumWidth(250);
    ui->label_p1_gif->setMinimumHeight(250);
    ui->label_p1_gif->move(475,300);
    movie = new QMovie(":/images/images/coder.gif");
    movie->setScaledSize(ui->label_p1_gif->size());//gif自适应
    ui->label_p1_gif->setMovie(movie);
    movie->start();

    ui->label_p1_drink->setMinimumHeight(100);
    ui->label_p1_drink->setMinimumWidth(100);
    movie2 = new QMovie(":/images/images/drink.gif");
    movie2->setScaledSize(ui->label_p1_drink->size());
    ui->label_p1_drink->setMovie(movie2);
    movie2->start();

    //随时间缩小的label
    ui->label_p1_opacity->setText("");
    ui->label_p1_opacity->resize(1200,850);
    ui->label_p1_opacity->move(0,0);
    ui->label_p1_opacity->setText("");
    ui->label_p1_opacity->setStyleSheet("QLabel{background-color:rgba(245,121,65,0.6);}");

    //背景图设置
    QPixmap welcomePix(":/images/images/shade_green3.jpg");
    ui->stackedWidget->widget(0)->setAutoFillBackground(true);
    QPalette welcomePalette;
    welcomePalette.setBrush(QPalette::Window, QBrush(welcomePix));
    ui->stackedWidget->widget(0)->setPalette(welcomePalette);
    //倒计时图标设置
    QPixmap timerPix(":/images/images/timer.png");
    ui->label_p1_timer->setPixmap(timerPix);
    ui->label_p1_timer->move(440,70);
    //倒计时设置
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(500);
    countdownTimer->start();
    connect(countdownTimer,&QTimer::timeout,this,&Widget::countDown);
    ui->label_p1_time->move(580,105);
    ui->label_p1_time->setText(QString::number(time));
    ui->label_p1_time->setStyleSheet("QLabel{color:red;font-family:Arial,"
                                     "\"Microsoft YaHei\";font-weight:bold;font-size:100px;}");
    //label_p1_msg提示
    ui->label_p1_msg->setText("How I look like from 7.1 to 7.18.");
    ui->label_p1_msg->setStyleSheet("QLabel{color:red;font-family:Arial,"
                                    "\"Microsoft YaHei\";font-size:40px;}");

    ui->label_p1_arrow->resize(200,200);
    ui->label_p1_arrow->move(530,545);
    QPixmap arrowPix(":/images/images/up_arrow.png");
    ui->label_p1_arrow->setPixmap(arrowPix);

    /**查看饮料界面控件**/
    //背景图
    QPixmap viewDrinkPix(":/images/images/bg2.png");
    ui->stackedWidget->widget(1)->setAutoFillBackground(true);
    QPalette viewDrinkPalette;
    viewDrinkPalette.setBrush(QPalette::Window, QBrush(viewDrinkPix));
    ui->stackedWidget->widget(1)->setPalette(viewDrinkPalette);
    //换页按钮样式设置
    ui->btn_next->resize(79,64);
    ui->btn_previous->resize(79,64);
    ui->btn_next->move(740,785);
    ui->btn_previous->move(40,785);
    ui->btn_next->setFlat(true);
    ui->btn_previous->setFlat(true);
    ui->btn_next->setStyleSheet("QPushButton{background:transparent;background-image:url(:/images/images/next.png);}");
    ui->btn_previous->setStyleSheet("QPushButton{background:transparent;background-image:url(:/images/images/previous.png);}");

    /*客户端界面*/
    //显示饮料库存数量的label组
    drinkAmountLabelList.append(ui->label_drink1Amount);
    drinkAmountLabelList.append(ui->label_drink2Amount);
    drinkAmountLabelList.append(ui->label_drink3Amount);
    drinkAmountLabelList.append(ui->label_drink4Amount);
    drinkAmountLabelList.append(ui->label_drink5Amount);
    drinkAmountLabelList.append(ui->label_drink6Amount);
    drinkAmountLabelList.append(ui->label_drink7Amount);
    drinkAmountLabelList.append(ui->label_drink8Amount);
    drinkAmountLabelList.append(ui->label_drink9Amount);
    drinkAmountLabelList.append(ui->label_drink10Amount);
    drinkAmountLabelList.append(ui->label_drink11Amount);
    drinkAmountLabelList.append(ui->label_drink12Amount);

    //显示饮料名字的label组
    drinkNameLabelList.append(ui->label_drink1Name);
    drinkNameLabelList.append(ui->label_drink2Name);
    drinkNameLabelList.append(ui->label_drink3Name);
    drinkNameLabelList.append(ui->label_drink4Name);
    drinkNameLabelList.append(ui->label_drink5Name);
    drinkNameLabelList.append(ui->label_drink6Name);
    drinkNameLabelList.append(ui->label_drink7Name);
    drinkNameLabelList.append(ui->label_drink8Name);
    drinkNameLabelList.append(ui->label_drink9Name);
    drinkNameLabelList.append(ui->label_drink10Name);
    drinkNameLabelList.append(ui->label_drink11Name);
    drinkNameLabelList.append(ui->label_drink12Name);

    //显示饮料价格的label组
    drinkPriceLabelList.append(ui->label_drink1Price);
    drinkPriceLabelList.append(ui->label_drink2Price);
    drinkPriceLabelList.append(ui->label_drink3Price);
    drinkPriceLabelList.append(ui->label_drink4Price);
    drinkPriceLabelList.append(ui->label_drink5Price);
    drinkPriceLabelList.append(ui->label_drink6Price);
    drinkPriceLabelList.append(ui->label_drink7Price);
    drinkPriceLabelList.append(ui->label_drink8Price);
    drinkPriceLabelList.append(ui->label_drink9Price);
    drinkPriceLabelList.append(ui->label_drink10Price);
    drinkPriceLabelList.append(ui->label_drink11Price);
    drinkPriceLabelList.append(ui->label_drink12Price);

    //饮料名字、价格的frame组
    drinkMsgFrameList.append(ui->frame_drink1Msg);
    drinkMsgFrameList.append(ui->frame_drink2Msg);
    drinkMsgFrameList.append(ui->frame_drink3Msg);
    drinkMsgFrameList.append(ui->frame_drink4Msg);
    drinkMsgFrameList.append(ui->frame_drink5Msg);
    drinkMsgFrameList.append(ui->frame_drink6Msg);
    drinkMsgFrameList.append(ui->frame_drink7Msg);
    drinkMsgFrameList.append(ui->frame_drink8Msg);
    drinkMsgFrameList.append(ui->frame_drink9Msg);
    drinkMsgFrameList.append(ui->frame_drink10Msg);
    drinkMsgFrameList.append(ui->frame_drink11Msg);
    drinkMsgFrameList.append(ui->frame_drink12Msg);

    //饮料背景label组
    drinkBgLabelList.append(ui->label_bg1);
    drinkBgLabelList.append(ui->label_bg2);
    drinkBgLabelList.append(ui->label_bg3);
    drinkBgLabelList.append(ui->label_bg4);
    drinkBgLabelList.append(ui->label_bg5);
    drinkBgLabelList.append(ui->label_bg6);
    drinkBgLabelList.append(ui->label_bg7);
    drinkBgLabelList.append(ui->label_bg8);
    drinkBgLabelList.append(ui->label_bg9);
    drinkBgLabelList.append(ui->label_bg10);
    drinkBgLabelList.append(ui->label_bg11);
    drinkBgLabelList.append(ui->label_bg12);

    //饮料购买按钮组
    btnImgDrinkGroup.addButton(ui->btn_img_drink1,1);
    btnImgDrinkGroup.addButton(ui->btn_img_drink2,2);
    btnImgDrinkGroup.addButton(ui->btn_img_drink3,3);
    btnImgDrinkGroup.addButton(ui->btn_img_drink4,4);
    btnImgDrinkGroup.addButton(ui->btn_img_drink5,5);
    btnImgDrinkGroup.addButton(ui->btn_img_drink6,6);
    btnImgDrinkGroup.addButton(ui->btn_img_drink7,7);
    btnImgDrinkGroup.addButton(ui->btn_img_drink8,8);
    btnImgDrinkGroup.addButton(ui->btn_img_drink9,9);
    btnImgDrinkGroup.addButton(ui->btn_img_drink10,10);
    btnImgDrinkGroup.addButton(ui->btn_img_drink11,11);
    btnImgDrinkGroup.addButton(ui->btn_img_drink12,12);

    //饮料停售标志label组
    banDrinkLabelList.append(ui->label_ban1);
    banDrinkLabelList.append(ui->label_ban2);
    banDrinkLabelList.append(ui->label_ban3);
    banDrinkLabelList.append(ui->label_ban4);
    banDrinkLabelList.append(ui->label_ban5);
    banDrinkLabelList.append(ui->label_ban6);
    banDrinkLabelList.append(ui->label_ban7);
    banDrinkLabelList.append(ui->label_ban8);
    banDrinkLabelList.append(ui->label_ban9);
    banDrinkLabelList.append(ui->label_ban10);
    banDrinkLabelList.append(ui->label_ban11);
    banDrinkLabelList.append(ui->label_ban12);


    //饮料名字和价格frame组样式
    for(int i=0; i<12; i++){
        drinkMsgFrameList.at(i)->move(65,230);//坐标
        drinkMsgFrameList.at(i)->resize(350,50);

    }

    //饮料名字组样式
    for(int i=0 ;i<12; i++){

        drinkNameLabelList.at(i)->setStyleSheet("font-size:18px;color:rgb(0,132,250);"
                                                "font-family:Arial,\"Microsoft YaHei\";"
                                                "font-weight:bold;");
        drinkNameLabelList.at(i)->move(-92,0);
        drinkNameLabelList.at(i)->resize(300,20);
        drinkNameLabelList.at(i)->setAlignment(Qt::AlignCenter);
    }

    //饮料价格组样式
    for(int i=0; i<12; i++){

        drinkPriceLabelList.at(i)->setStyleSheet("font-size:20px;color:rgb(255,103,61);"
                                                 "font-family:Arial,\"Microsoft YaHei\";"
                                                 "font-weight:bold;");
        drinkPriceLabelList.at(i)->move(30,30);
    }

    //饮料数量组样式
    for(int i=0; i<12; i++){

        drinkAmountLabelList.at(i)->setStyleSheet("font-size:20px;color:rgb(255,103,61);"
                                                  "font-family:Arial,\"Microsoft YaHei\";"
                                                  "font-weight:bold;");
        drinkAmountLabelList.at(i)->move(70,288);
        drinkAmountLabelList.at(i)->resize(150,20);
    }


    //饮料购买按钮组样式
    for(int i=1; i<13; i++){
        btnImgDrinkGroup.button(i)->resize(100,170);//大小
        btnImgDrinkGroup.button(i)->move(75,35);//坐标

        QString btnImgDrinkStyle = "QPushButton{background:transparent;"
                                   "background-image: url(:/images/images/" + drinkMap.value(i).at(5) +
                                   ");background-repeat:no-repeat;}";
        btnImgDrinkGroup.button(i)->setStyleSheet(btnImgDrinkStyle);//贴图路径

    }

    //饮料的背景淡绿色label样式设置
    for(int i=0; i<12;i++){
        drinkBgLabelList.at(i)->move(0,0);//坐标
        drinkBgLabelList.at(i)->resize(258,330);//大小
        drinkBgLabelList.at(i)->setStyleSheet("QLabel{background-color:"
                     "rgba(11,233,42,0.15);border-radius:30px;}");//样式
    }

    //饮料停售标志样式设置
    for(int i=0; i<12; i++){

        banDrinkLabelList.at(i)->resize(128,128);
        banDrinkLabelList.at(i)->move(60,70);
        banDrinkLabelList.at(i)->setPixmap(QPixmap(":/images/images/ban.png"));

    }

    //右方放大饮料的label属性
    ui->label_move->move(880,140);
    ui->label_move->resize(240,460);
    ui->label_move->setStyleSheet("background-repeat:no-repeat;");
    //右下方表格属性
    ui->drinkMsgTable->setColumnCount(2);
    ui->drinkMsgTable->setRowCount(6);
    ui->drinkMsgTable->hide();//默认隐藏
    //表头信息
    QStringList header;
    header<<"Ingredient"<<"/100ml";
    ui->drinkMsgTable->setHorizontalHeaderLabels(header);
    //隐藏表头
    ui->drinkMsgTable->verticalHeader()->setVisible(false);
    ui->drinkMsgTable->horizontalHeader()->setVisible(false);
    //表格设置只读
    ui->drinkMsgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置表格内容
    ui->drinkMsgTable->setItem(0,0,new QTableWidgetItem("Ingredient"));
    ui->drinkMsgTable->setItem(0,1,new QTableWidgetItem("/100ml"));
    ui->drinkMsgTable->setItem(1,0,new QTableWidgetItem("Energy"));
    ui->drinkMsgTable->setItem(2,0,new QTableWidgetItem("Protein"));
    ui->drinkMsgTable->setItem(3,0,new QTableWidgetItem("Fat"));
    ui->drinkMsgTable->setItem(4,0,new QTableWidgetItem("Carbohydrate"));
    ui->drinkMsgTable->setItem(5,0,new QTableWidgetItem("Na"));
    //设置表头内容靠左
    ui->drinkMsgTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    //设置表格大小和坐标
    ui->drinkMsgTable->resize(253,225);
    ui->drinkMsgTable->move(890,610);
    //设置表格的css样式
    ui->drinkMsgTable->setStyleSheet("QTableWidget{background-color:rgba(233,103,61,0.6);"
                                                  "color:white;}");
    ui->drinkMsgTable->setFrameShape(QFrame::NoFrame);//去除边框
    ui->drinkMsgTable->setShowGrid(false);

    ui->drinkMsgTable->item(0,0)->setTextColor(QColor(0,132,250));
    ui->drinkMsgTable->item(0,1)->setTextColor(QColor(0,132,250));


    /*补货员补货界面*/
    //补货按钮组
    btnAddDrinkGroup.addButton(ui->btn_addDrink1,1);
    btnAddDrinkGroup.addButton(ui->btn_addDrink2,2);
    btnAddDrinkGroup.addButton(ui->btn_addDrink3,3);
    btnAddDrinkGroup.addButton(ui->btn_addDrink4,4);
    btnAddDrinkGroup.addButton(ui->btn_addDrink5,5);
    btnAddDrinkGroup.addButton(ui->btn_addDrink6,6);
    btnAddDrinkGroup.addButton(ui->btn_addDrink7,7);
    btnAddDrinkGroup.addButton(ui->btn_addDrink8,8);
    btnAddDrinkGroup.addButton(ui->btn_addDrink9,9);
    btnAddDrinkGroup.addButton(ui->btn_addDrink10,10);
    btnAddDrinkGroup.addButton(ui->btn_addDrink11,11);
    btnAddDrinkGroup.addButton(ui->btn_addDrink12,12);
    //补货spinbox组
    addDrinkSpinBoxList.append(ui->spinBox_addDrink1);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink2);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink3);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink4);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink5);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink6);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink7);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink8);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink9);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink10);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink11);
    addDrinkSpinBoxList.append(ui->spinBox_addDrink12);

    //补货按钮和spinbox组的样式
    for(int i=1; i<13; i++){
        //补货按钮和spinbox初始状态为隐藏
        btnAddDrinkGroup.button(i)->hide();
        addDrinkSpinBoxList.at(i-1)->hide();

        //spinbox样式设置
        addDrinkSpinBoxList.at(i-1)->setStyleSheet("QSpinBox:up-button{subcontrol-origin:border;"
                                                                      "subcontrol-position:right;"
                                                                      "image:url(:/images/images/add.png);"
                                                                      "width:32px;"
                                                                      "height:32px;}"
                                                   "QSpinBox:down-button{subcontrol-origin:border;"
                                                                        "subcontrol-position:left;"
                                                                        "image:url(:/images/images/sub.png);"
                                                                        "width:32px;"
                                                                        "height:32px;}"
                                                   "QSpinBox{color:rgb(233,103,61);font-size:20px;"
                                                            "text-align:center;"
                                                            "font-weight:bold;"
                                                            "background:transparent;}");
        addDrinkSpinBoxList.at(i-1)->move(78,216);
        addDrinkSpinBoxList.at(i-1)->resize(90,30);

        //补货按钮样式设置
        btnAddDrinkGroup.button(i)->resize(105,34);
        btnAddDrinkGroup.button(i)->move(73,251);
        btnAddDrinkGroup.button(i)->setStyleSheet("QPushButton{background-color:rgb(233,103,61);"
                                                              "border-radius:17px;color:white;"
                                                              "font-family:Arial,\"Microsoft YaHei\";}");
        btnAddDrinkGroup.button(i)->setText("Add");

    }
    ui->btn_logout->hide();//退出登录按钮初始隐藏
    ui->frame_status->hide();//设置饮料机状态单选框初始隐藏
    ui->hottestDrinkTable->hide();//热销榜表格初始隐藏
    ui->label_hottestImg->hide();//热销榜logo初始隐藏

    //补货员login和logout按钮样式设置
    ui->btn_login->move(1000,40);
    ui->btn_logout->move(1000,40);
    ui->btn_login->resize(150,50);
    ui->btn_logout->resize(150,50);
    ui->btn_login->setText("Login");
    ui->btn_logout->setText("Logout");
    ui->btn_login->setStyleSheet("QPushButton{background-color:rgb(233,103,61);"
                                             "border-radius:20px;color:white;"
                                             "font-family:Arial,\"Microsoft YaHei\";"
                                             "font-size:20px;}");

    ui->btn_logout->setStyleSheet("QPushButton{background-color:rgb(255,80,76);"
                                             "border-radius:17px;color:white;"
                                             "font-family:Arial,\"Microsoft YaHei\";"
                                             "font-size:20px;}");

    //设置补货员控制饮料机状态的radioButton
    ui->radioButton_cool->setStyleSheet("color:rgb(0,132,250);font-weight:bold;font-size:20px;");
    ui->radioButton_heating->setStyleSheet("color:rgb(255,0,0);font-weight:bold;font-size:20px;");
    ui->radioButton_warm->setStyleSheet("color:rgb(233,103,61);font-weight:bold;font-size:20px;");
    ui->frame_status->setStyleSheet("background-color:rgba(11,233,42,0.15);border-radius:30px;");
    //热销榜的labelImg样式
    ui->label_hottestImg->resize(64,64);
    ui->label_hottestImg->move(1090,730);
    ui->label_hottestImg->setPixmap(QPixmap(":/images/images/hottest.png"));


    /*时间显示界面*/
    //三个分隔栏label样式设置
    ui->label_divide1->move(0,0);
    ui->label_divide2->move(0,0);
    ui->label_divide3->move(0,0);

    ui->label_divide1->resize(5,100);
    ui->label_divide2->resize(5,100);
    ui->label_divide3->resize(5,100);

    ui->label_divide1->setStyleSheet("background-color:rgb(58,164,86);");
    ui->label_divide2->setStyleSheet("background-color:rgb(95,184,96);");
    ui->label_divide3->setStyleSheet("background-color:rgb(148,209,104);");

    //时间显示样式设置
    ui->label_currentDate->setStyleSheet("font-family:Arial,\"Microsoft YaHei\";"
                                         "font-size:25px;color:rgb(58,164,86);");
    ui->label_currentTime->setStyleSheet("font-family:Arial,\"Microsoft YaHei\";"
                                         "font-size:25px;"
                                         "color:rgb(245,121,65);");
    //开启时间显示计时器
    QTimer *getCurrentTimeTimer = new QTimer(this);
    getCurrentTimeTimer->setInterval(1000);
    getCurrentTimeTimer->start();
    connect(getCurrentTimeTimer,&QTimer::timeout,this,&Widget::getCurrentTime);
    ui->frame_time->setStyleSheet("color:red;");

    /*天气显示界面*/
    cityManager.getWeatherbyCityName("中山",std::bind(&Widget::showWeather,
                                         this, std::placeholders::_1));

    //天气显示样式
    ui->label_weather->resize(64,64);
    ui->label_weather->move(20,25);
    ui->label_temperature->move(95,25);
    ui->label_temperature->resize(150,50);
    ui->label_temperature->setStyleSheet("font-family:Arial,\"Microsoft YaHei\";"
                                         "font-size:25px;"
                                         "color:rgb(0,132,250);");

    /***鼠标事件***/
    /**用户查看饮料界面**/
    /*翻页按钮*/
    connect(ui->btn_previous,&QPushButton::clicked,
            [this](){
                ui->stackedWidget_2->setCurrentIndex(0);
            });
    connect(ui->btn_next,&QPushButton::clicked,
            [this](){
                ui->stackedWidget_2->setCurrentIndex(1);
            });

    /*鼠标hover在饮料图片上，图片放大*/
    //鼠标hover在饮料图片上事件,此处无法for循环按钮组，因用的信号是自定义类MyDrinkBtn里的信号
    connect(ui->btn_img_drink1,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("nestlewaterbig.png",1);
            });
    connect(ui->btn_img_drink2,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("spritebig.png",2);
            });
    connect(ui->btn_img_drink3,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("pepsibig.png",3);
            });
    connect(ui->btn_img_drink4,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("tomatojuicebig.png",4);
            });
    connect(ui->btn_img_drink5,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("dietcokebig.png",5);
            });
    connect(ui->btn_img_drink6,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("redbullbig.png",6);
            });
    connect(ui->btn_img_drink7,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("spritezerobig.png",7);
            });
    connect(ui->btn_img_drink8,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("evianbig.png",8);
            });
    connect(ui->btn_img_drink9,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("nestlecoffeebig.png",9);
            });
    connect(ui->btn_img_drink10,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("orangejuicebig.png",10);
            });
    connect(ui->btn_img_drink11,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("spritebottlebig.png",11);
            });
    connect(ui->btn_img_drink12,&MyDrinkBtn::enterSignal,this,
            [=](){
                labelMoveEnter("beerbig.png",12);
            });
    //鼠标离开饮料图片上事件
    connect(ui->btn_img_drink1,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink2,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink3,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink4,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink5,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink6,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink7,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink8,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink9,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink10,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink11,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);
    connect(ui->btn_img_drink12,&MyDrinkBtn::leaveSignal,this,
            &Widget::labelMoveLeave);

    /*点击付款按钮事件*/
    for(int i=1; i<13; i++){
        connect(btnImgDrinkGroup.button(i),&QAbstractButton::clicked,
                [=](){
                    if(drinkMap.value(i).at(2)=="0"){
                        QMessageBox::warning(this,"Sorry","Please choose another drink.");
                    }
                    else{
                        //传值给付款窗口
                        pay_w.getDrinkId(i);
                        pay_w.getDrinkName(drinkMap.value(i).at(0));
                        pay_w.getDrinkPrice(drinkMap.value(i).at(1));
                        pay_w.getDrinkAmount(drinkMap.value(i).at(2));
                        pay_w.getDrinkSalesAmount(drinkMap.value(i).at(3));
                        pay_w.exec();
                    }
                });
    }
    /*付款完成，收到付款窗口的信号，再更新一次drinkMap和界面的信息*/
    connect(&pay_w,&PayDialog::signalOfNewStock,this,&Widget::updateDrinkMapAndMsg);

    /**补货员鼠标事件**/
    /*登录弹窗*/
    connect(ui->btn_login,&QPushButton::clicked,
            [this](){
                login_w.exec();
            });

    /*登录成功，界面变化*/
    connect(&login_w,&LoginDialog::signalOfLoginSuccess,
            [this](){
                //一样是饮料展示界面，只是显示和隐藏一些控件；
                for(int i=1; i<13; i++){
                    btnAddDrinkGroup.button(i)->show();
                }
                for(int i=0; i<12; i++){
                    addDrinkSpinBoxList.at(i)->show();
                }
                ui->btn_logout->show();
                ui->frame_status->show();
                ui->hottestDrinkTable->show();
                ui->label_hottestImg->show();
                ui->drinkMsgTable->move(890,2000);

                for(int i=0 ;i<12; i++){
                    drinkMsgFrameList.at(i)->hide();
                }
                ui->btn_login->hide();
                ui->label_move->hide();

                //同时限定spinbox的最大值
                for(int i=0; i<12; i++){
                    //此处设置一种饮料在机器上的最大存储值为20
                    addDrinkSpinBoxList.at(i)->setMaximum(10-QString(drinkMap.value(i+1).at(2)).toInt());

                }

                //在补货员下，设置饮料的按钮不能按，也不相应hover事件
                for(int i=1; i<13; i++){
                    btnImgDrinkGroup.button(i)->setEnabled(false);
                }
            });
    /*退出登录*/
    connect(ui->btn_logout,&QPushButton::clicked,[this](){
                QMessageBox::information(this,"Information","Log out successfully.");
                //显示隐藏对应控件
                for(int i=1; i<13; i++){
                    btnAddDrinkGroup.button(i)->hide();
                }
                for(int i=0; i<12; i++){
                    addDrinkSpinBoxList.at(i)->hide();
                }
                ui->btn_logout->hide();
                ui->frame_status->hide();
                ui->hottestDrinkTable->hide();
                ui->label_hottestImg->hide();
                ui->drinkMsgTable->move(890,610);

                for(int i=0 ;i<12; i++){
                    drinkMsgFrameList.at(i)->show();
                }
                ui->btn_login->show();
                ui->label_move->show();


                //将购买按钮放开
                for(int i=1; i<13; i++){
                    btnImgDrinkGroup.button(i)->setEnabled(true);
                }

                //更新饮料机的状态
                updateMachineStatus();
            });
    /*点击补货按钮，进行补货*/
    for(int i=1 ;i<13; i++){
        connect(btnAddDrinkGroup.button(i),&QAbstractButton::clicked,
                [=](){
                    addDrink(i,addDrinkSpinBoxList.at(i-1)->value(),QString(drinkMap.value(i).at(2)).toInt());
                });
    }

}

//接受到服务端的字符串，响应事件
void Widget::drinkIsAtSale(int id,QString isAtSale){

    //更改drinkMap内容
    drinkMap[id][4] = isAtSale;
    //黑白图标设置
    if(isAtSale == "1"){
        btnImgDrinkGroup.button(id)->setEnabled(true);
        banDrinkLabelList.at(id-1)->hide();

    }
    else{
        btnImgDrinkGroup.button(id)->setEnabled(false);//服务端停卖，购买按钮不能按,并显示ban的图标
        banDrinkLabelList.at(id-1)->show();
    }

}

//用于饮料机接收信息后处理
void Widget::clientRecvData(){

    char recvMsg[1024] = {0};
    int recvRe = clientSocket->read(recvMsg, 1024);
    if(recvRe == -1)
    {
        QMessageBox::information(this, "QT网络通信", "接收服务端数据失败！");
        return;
    }
    else{
        QString isAtSale = QString(recvMsg[0]);
        int id1 = QString(recvMsg[2]).toInt();
        QChar id2 = recvMsg[3];
        //此处需要判断是个位数还是两位数
        if( id2 == '\x0' ){
            switch(id1){
                case 1:drinkIsAtSale(1,isAtSale);break;
                case 2:drinkIsAtSale(2,isAtSale);break;
                case 3:drinkIsAtSale(3,isAtSale);break;
                case 4:drinkIsAtSale(4,isAtSale);break;
                case 5:drinkIsAtSale(5,isAtSale);break;
                case 6:drinkIsAtSale(6,isAtSale);break;
                case 7:drinkIsAtSale(7,isAtSale);break;
                case 8:drinkIsAtSale(8,isAtSale);break;
                case 9:drinkIsAtSale(9,isAtSale);break;
                default:break;
            }
        }
        else{
            switch(QString(id2).toInt()){
                case 0:drinkIsAtSale(10,isAtSale);break;
                case 1:drinkIsAtSale(11,isAtSale);break;
                case 2:drinkIsAtSale(12,isAtSale);break;
                default:break;
            }
        }
    }

}
//开始动画倒计时函数
void Widget::countDown(){
    //设置透明的label随时间以中心为原点变小
    int width = ui->label_p1_opacity->width();
    int height = ui->label_p1_opacity->height();
    int x = ui->label_p1_opacity->x() + 60;
    int y = ui->label_p1_opacity->y() + 42.5;
    width = width-120;
    height = height-85;
    ui->label_p1_opacity->move(x,y);
    ui->label_p1_opacity->resize(width,height);

    if(time==0){//时间到
        //倒计时定时器关闭
        countdownTimer->stop();
        //gif停止
        movie->stop();
        //提示语音播放
        QMediaPlayer *p = new QMediaPlayer(this);
        p->setMedia(QUrl("qrc:/mp3/mp3/startMachine.mp3"));
        p->setVolume(80);
        p->play();
        //弹窗提示
        QMessageBox box(this);
        box.setWindowTitle("Welcome");
        box.setIcon(QMessageBox::Information);
        box.setText("Click OK to use my vending machine.");
        QPushButton *okBtn = box.addButton("OK",
                                     QMessageBox::YesRole);
        box.exec();
        //按下ok，跳往查看货物的界面
        if(box.clickedButton()== okBtn){
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
    else{
        time = time-1;
        ui->label_p1_time->setText(QString::number(time));

    }
}


//得到当前时间
void Widget::getCurrentTime(){

    QDateTime time = QDateTime::currentDateTime();
    QString current_date = time.toString("yyyy-MM-dd");
    QString current_time = time.toString("hh:mm:ss");
    ui->label_currentDate->setText(current_date);
    ui->label_currentTime->setText(current_time);
}

//显示当前天气
void Widget::showWeather(const QStringList &strList){

    //判断返回是否异常
    if (strList.count() < 23) {
        return;
    }

    /**整体信息**/
    //strList[5]是温度信息；strList[8]是对应的天气图像信息
    QString weatherMsg = strList[8];
    QString weatherImg;
    for(int i=0; i<weatherMsg.length(); i++){

        if(weatherMsg.at(i) != '.' ){
            weatherImg.append(weatherMsg.at(i));
        }
        else{
            break;
        }

    }

    QString pathWeather = ":/images/images/" + weatherImg + ".png";
    ui->label_weather->setPixmap(QPixmap(pathWeather));
    ui->label_temperature->setText(strList[5]);

}

//鼠标移入img函数
void Widget::labelMoveEnter(QString str,int order){

    //设置label_move的图片
    QString style = "background-image:url(:/images/images/" + str + ");";
    ui->label_move->setStyleSheet(style);

    //设置drinkMsgTable的内容
    ui->drinkMsgTable->show();
    ui->drinkMsgTable->setItem(1,1,new QTableWidgetItem(drinkMsgMap.value(order).at(0)));
    ui->drinkMsgTable->setItem(2,1,new QTableWidgetItem(drinkMsgMap.value(order).at(1)));
    ui->drinkMsgTable->setItem(3,1,new QTableWidgetItem(drinkMsgMap.value(order).at(2)));
    ui->drinkMsgTable->setItem(4,1,new QTableWidgetItem(drinkMsgMap.value(order).at(3)));
    ui->drinkMsgTable->setItem(5,1,new QTableWidgetItem(drinkMsgMap.value(order).at(4)));

}
//鼠标移出img函数
void Widget::labelMoveLeave(){

    //清空label_move的图片
    ui->label_move->setStyleSheet("background-image:url();"
                                  "background-repeat:no-repeat;");

    //隐藏drinkMsgTable的内容
    ui->drinkMsgTable->hide();
}
//更新drinkMap操作
void Widget::updateDrinkMap(){

    QSqlQuery query;
    QStringList message;//一个暂时的变量
    int i =1;
    QString sql = "select * from drink";
    query.exec(sql);

    while(query.next()){
        message.append(query.value(1).toString());
        message.append(query.value(2).toString());
        message.append(query.value(3).toString());
        message.append(query.value(4).toString());
        message.append(query.value(5).toString());
        message.append(query.value(6).toString());
        message.append(query.value(7).toString());
        drinkMap.insert(i,message);
        i++;
        message.clear();//需要清空才可重复
    }
}
//更新drinkMsgMap操作
void Widget::updateDrinkMsgMap(){

    QSqlQuery query;
    QStringList message;//一个暂时的变量
    int i =1;
    QString sql = "select * from drink_msg";
    query.exec(sql);

    while(query.next()){
        message.append(query.value(1).toString());
        message.append(query.value(2).toString());
        message.append(query.value(3).toString());
        message.append(query.value(4).toString());
        message.append(query.value(5).toString());
        message.append(query.value(6).toString());
        message.append(query.value(7).toString());
        drinkMsgMap.insert(i,message);
        i++;
        message.clear();//需要清空才可重复
    }
}

//更新饮料1-12的信息

void Widget::updateMsgOfDrink1(){

    //若饮料卖光，则发生相应动作
    if(drinkMap.value(1).at(4)=="0"){
        ui->btn_img_drink1->setEnabled(false);
        ui->label_ban1->show();
    }
    else{

        ui->label_ban1->hide();
    }
    ui->label_drink1Name->setText(drinkMap.value(1).at(0));
    ui->label_drink1Price->setText("￥ "+ drinkMap.value(1).at(1));
    ui->label_drink1Amount->setText("Amount: "+ drinkMap.value(1).at(2));
}
void Widget::updateMsgOfDrink2(){

    if(drinkMap.value(2).at(4)=="0"){
        ui->btn_img_drink2->setEnabled(false);
        ui->label_ban2->show();
    }
    else{
        ui->label_ban2->hide();
    }
    ui->label_drink2Name->setText(drinkMap.value(2).at(0));
    ui->label_drink2Price->setText("￥ "+ drinkMap.value(2).at(1));
    ui->label_drink2Amount->setText("Amount: "+ drinkMap.value(2).at(2));
}
void Widget::updateMsgOfDrink3(){

    if(drinkMap.value(3).at(4)=="0"){
        ui->btn_img_drink3->setEnabled(false);
        ui->label_ban3->show();
    }
    else{
        ui->label_ban3->hide();
    }
    ui->label_drink3Name->setText(drinkMap.value(3).at(0));
    ui->label_drink3Price->setText("￥ "+ drinkMap.value(3).at(1));
    ui->label_drink3Amount->setText("Amount: "+ drinkMap.value(3).at(2));
}
void Widget::updateMsgOfDrink4(){

    if(drinkMap.value(4).at(4)=="0"){
        ui->btn_img_drink4->setEnabled(false);
        ui->label_ban4->show();
    }
    else{
        ui->label_ban4->hide();
    }
    ui->label_drink4Name->setText(drinkMap.value(4).at(0));
    ui->label_drink4Price->setText("￥ "+ drinkMap.value(4).at(1));
    ui->label_drink4Amount->setText("Amount: "+ drinkMap.value(4).at(2));
}
void Widget::updateMsgOfDrink5(){

    if(drinkMap.value(5).at(4)=="0"){
        ui->btn_img_drink5->setEnabled(false);
        ui->label_ban5->show();
    }
    else{
        ui->label_ban5->hide();
    }
    ui->label_drink5Name->setText(drinkMap.value(5).at(0));
    ui->label_drink5Price->setText("￥ "+ drinkMap.value(5).at(1));
    ui->label_drink5Amount->setText("Amount: "+ drinkMap.value(5).at(2));
}
void Widget::updateMsgOfDrink6(){

    if(drinkMap.value(6).at(4)=="0"){
        ui->btn_img_drink6->setEnabled(false);
        ui->label_ban6->show();
    }
    else{
        ui->label_ban6->hide();
    }
    ui->label_drink6Name->setText(drinkMap.value(6).at(0));
    ui->label_drink6Price->setText("￥ "+ drinkMap.value(6).at(1));
    ui->label_drink6Amount->setText("Amount: "+ drinkMap.value(6).at(2));
}
void Widget::updateMsgOfDrink7(){

    if(drinkMap.value(7).at(4)=="0"){
        ui->btn_img_drink7->setEnabled(false);
        ui->label_ban7->show();
    }
    else{
        ui->label_ban7->hide();
    }
    ui->label_drink7Name->setText(drinkMap.value(7).at(0));
    ui->label_drink7Price->setText("￥ "+ drinkMap.value(7).at(1));
    ui->label_drink7Amount->setText("Amount: "+ drinkMap.value(7).at(2));
}
void Widget::updateMsgOfDrink8(){

    if(drinkMap.value(8).at(4)=="0"){
        ui->btn_img_drink8->setEnabled(false);
        ui->label_ban8->show();
    }
    else{
        ui->label_ban8->hide();
    }
    ui->label_drink8Name->setText(drinkMap.value(8).at(0));
    ui->label_drink8Price->setText("￥ "+ drinkMap.value(8).at(1));
    ui->label_drink8Amount->setText("Amount: "+ drinkMap.value(8).at(2));
}
void Widget::updateMsgOfDrink9(){

    if(drinkMap.value(9).at(4)=="0"){
        ui->btn_img_drink9->setEnabled(false);
        ui->label_ban9->show();
    }
    else{
        ui->label_ban9->hide();
    }
    ui->label_drink9Name->setText(drinkMap.value(9).at(0));
    ui->label_drink9Price->setText("￥ "+ drinkMap.value(9).at(1));
    ui->label_drink9Amount->setText("Amount: "+ drinkMap.value(9).at(2));
}
void Widget::updateMsgOfDrink10(){

    if(drinkMap.value(10).at(4)=="0"){
        ui->btn_img_drink10->setEnabled(false);
        ui->label_ban10->show();
    }
    else{
        ui->label_ban10->hide();
    }
    ui->label_drink10Name->setText(drinkMap.value(10).at(0));
    ui->label_drink10Price->setText("￥ "+ drinkMap.value(10).at(1));
    ui->label_drink10Amount->setText("Amount: "+ drinkMap.value(10).at(2));
}
void Widget::updateMsgOfDrink11(){

    if(drinkMap.value(11).at(4)=="0"){
        ui->btn_img_drink11->setEnabled(false);
        ui->label_ban11->show();
    }
    else{
        ui->label_ban11->hide();
    }
    ui->label_drink11Name->setText(drinkMap.value(11).at(0));
    ui->label_drink11Price->setText("￥ "+ drinkMap.value(11).at(1));
    ui->label_drink11Amount->setText("Amount: "+ drinkMap.value(11).at(2));
}
void Widget::updateMsgOfDrink12(){

    if(drinkMap.value(12).at(4)=="0"){
        ui->btn_img_drink12->setEnabled(false);
        ui->label_ban12->show();
    }
    else{
        ui->label_ban12->hide();
    }
    ui->label_drink12Name->setText(drinkMap.value(12).at(0));
    ui->label_drink12Price->setText("￥ "+ drinkMap.value(12).at(1));
    ui->label_drink12Amount->setText("Amount: "+ drinkMap.value(12).at(2));
}

//更新饮料机的状态
void Widget::updateMachineStatus(){
    QString statusContent;
    QString statusColor;
    QString timeColor;
    QString bgImg;

    if(ui->radioButton_cool->isChecked()){

        statusContent = "Frozen";
        statusColor = "rgb(0,132,250)";
        timeColor = "rgb(0,132,250)";
        bgImg = "bg_cool.png";
        for(int i=0; i<12; i++){
            drinkNameLabelList.at(i)->setStyleSheet("font-size:18px;color:rgb(31,88,147);"
                                                    "font-family:Arial,\"Microsoft YaHei\";"
                                                    "font-weight:bold;");
        }
    }
    else if(ui->radioButton_heating->isChecked()){

        statusContent = "Heating";
        statusColor = "rgb(255,0,0)";        
        timeColor = "rgb(245,121,65)";
        bgImg = "bg_warm.png";

    }
    else{

        statusContent = "Warm";
        statusColor = "rgb(255,103,61)";
        timeColor = "rgb(245,121,65)";
        bgImg = "bg2.png";

    }

    //设置状态label内容
    ui->label_status->setText("Status: " + statusContent);
    //设置状态label的样式
    ui->label_status->setStyleSheet("font-family:Arial,\"Microsoft YaHei\";"
                                    "font-size:25px;"
                                    "color:" + statusColor + ";");
    //设置时间的颜色
    ui->label_currentTime->setStyleSheet("font-family:Arial,\"Microsoft YaHei\";"
                                                 "font-size:25px;"
                                                 "color:" + timeColor +";");
    //设置背景图片
    ui->stackedWidget->widget(1)->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(":/images/images/"+ bgImg)));
    ui->stackedWidget->widget(1)->setPalette(palette);
}

//收到付款完成的信号，更新相关数据
void Widget::updateDrinkMapAndMsg(){
    updateMachineStatus();
    updateDrinkMap();
    updateDrinkMsgMap();
    updateMsgOfDrink1();
    updateMsgOfDrink2();
    updateMsgOfDrink3();
    updateMsgOfDrink4();
    updateMsgOfDrink5();
    updateMsgOfDrink6();
    updateMsgOfDrink7();
    updateMsgOfDrink8();
    updateMsgOfDrink9();
    updateMsgOfDrink10();
    updateMsgOfDrink11();
    updateMsgOfDrink12();
    updateHottestDrink();
}

//补货员补货操作
void Widget::addDrink(int id, int addAmount, int amount){

    //DB操作
    QSqlQuery query;
    int newAmount = addAmount + amount;
    QString sql = QString("update drink set stock = %1 where id = %2").arg(newAmount).arg(id);
    query.exec(sql);

    //更新界面信息
    updateDrinkMapAndMsg();
    //补货成功后，需要清空spinbox的值，并重设最大值，否则会有bug
    addDrinkSpinBoxList[id-1]->clear();
    addDrinkSpinBoxList[id-1]->setMaximum(10-QString(drinkMap.value(id).at(2)).toInt());
    QMessageBox::information(this,"Add Operation","Add successfully.");
}

//快排函数
int Widget::partition(HottestDrink a[], int p, int r){
    int i = p,j = r+1;
    int x = a[p].salesAmount;
    while(true){
        while(a[++i].salesAmount<x && i<r);//a[i]比3大
        while(a[--j].salesAmount>x);//a[j]比3小
        if(i<j){
            int tempSalesAmount;
            int tempId;
            QString tempDrinkName;

            tempSalesAmount = a[i].salesAmount;
            tempId = a[i].id;
            tempDrinkName = a[i].drinkName;

            a[i].id = a[j].id;
            a[i].salesAmount = a[j].salesAmount;
            a[i].drinkName = a[j].drinkName;

            a[j].id = tempId;
            a[j].drinkName =tempDrinkName;
            a[j].salesAmount = tempSalesAmount;
        }
        else{
            break;
        }
    }
    int tempId2;
    QString tempDrinkName2;
    tempId2 = a[p].id;
    tempDrinkName2 = a[p].drinkName;

    a[p].id = a[j].id;
    a[p].drinkName = a[j].drinkName;
    a[p].salesAmount = a[j].salesAmount;

    a[j].id = tempId2;
    a[j].drinkName = tempDrinkName2;
    a[j].salesAmount = x;
    return j;
}
void Widget::quickSort(HottestDrink a[], int p, int r){

    if(p<r){
        int q = partition(a,p,r);
        quickSort(a,p,q-1);
        quickSort(a,q+1,r);
    }
}

//更新热销饮料数组
void Widget::updateHottestDrink(){

    //设置补货员查看的饮料热销榜快排
    for(int i=0 ;i<12; i++){
        hottestDrinkList[i].id = i+1;
        hottestDrinkList[i].drinkName = drinkMap.value(i+1).at(0);
        hottestDrinkList[i].salesAmount = QString(drinkMap.value(i+1).at(3)).toInt();
    }
    //快排得到新的排序后的饮料顺序
    quickSort(hottestDrinkList,0,11);

    //将数据插入表格中
    for(int i=0; i<12; i++){
        ui->hottestDrinkTable->setItem(12-i,0,new QTableWidgetItem(hottestDrinkList[i].drinkName));
        ui->hottestDrinkTable->setItem(12-i,1,new QTableWidgetItem(QString::number(hottestDrinkList[i].salesAmount)));

        ui->hottestDrinkTable->item(12-i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->hottestDrinkTable->item(12-i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

Widget::~Widget()
{
    delete ui;
}
