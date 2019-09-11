#ifndef MYBGLABEL_H
#define MYBGLABEL_H

#include <QLabel>
#include <QEvent>

class MyBgLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MyBgLabel(QWidget *parent = 0);

public:
    void enterEvent(QEvent *e);//自定义进入离开事件；
    void leaveEvent(QEvent *e);

signals:
    void enterSignal();
    void leaveSignal();

public slots:
};

#endif // MYBGLABEL_H
