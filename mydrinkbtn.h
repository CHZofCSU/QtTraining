#ifndef MYDRINKBTN_H
#define MYDRINKBTN_H

#include <QPushButton>
#include <QEvent>

class MyDrinkBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit MyDrinkBtn(QWidget *parent = 0);

public:
    void enterEvent(QEvent *e);//自定义进入离开事件；
    void leaveEvent(QEvent *e);

signals:
    void enterSignal();
    void leaveSignal();

public slots:
};

#endif // MYDRINKBTN_H
