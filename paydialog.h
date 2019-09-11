#ifndef PAYDIALOG_H
#define PAYDIALOG_H

#include <QDialog>

namespace Ui {
class PayDialog;
}

class PayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PayDialog(QWidget *parent = 0);
    ~PayDialog();

public:
    /**得到pay框需要的内容**/
    void getDrinkId(int id);
    void getDrinkName(QString str);
    void getDrinkPrice(QString str);
    void getDrinkAmount(QString str);
    void getDrinkSalesAmount(QString str);
    /**修改db内容**/
    void changeDrinkAmount(int id, int buyAmount);


signals:
    //由于购买，导致饮料数量变化，因此需要将新的数量写回给myMap
    void signalOfNewStock();

private slots:
    //算出购买饮料的总价
    void on_spinBox_payAmount_valueChanged(int arg1);

private:
    Ui::PayDialog *ui;
};

#endif // PAYDIALOG_H
