#include "mybglabel.h"
#include <QLabel>

MyBgLabel::MyBgLabel(QWidget *parent) : QLabel(parent)
{
    this->setStyleSheet("QLabel{background-color:rgba(11,233,42,0.1);border-radius:30px;}");

}

void MyBgLabel::enterEvent(QEvent *e){

    emit enterSignal();

}

void MyBgLabel::leaveEvent(QEvent *e){

    emit leaveSignal();

}
