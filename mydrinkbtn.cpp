#include "mydrinkbtn.h"

MyDrinkBtn::MyDrinkBtn(QWidget *parent) : QPushButton(parent)
{

}
void MyDrinkBtn::enterEvent(QEvent *e){

    emit enterSignal();

}

void MyDrinkBtn::leaveEvent(QEvent *e){

    emit leaveSignal();

}



