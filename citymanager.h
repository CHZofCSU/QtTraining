#ifndef CITYMANAGER_H
#define CITYMANAGER_H

#include <QObject>
#include <functional>

class QNetworkAccessManager;
class QNetworkReply;

//只要是返回值是void类型，并且传入参数为const QStringList &strList的函数
//都可用OnMessage来调用（OnMessage(strList);）
typedef std::function<void (const QStringList &strList)> OnMessage;

class Citymanager : public QObject
{
    Q_OBJECT
public:
    explicit Citymanager(QObject *parent = 0);

    void getWeatherbyCityName(const QString &cityName, const OnMessage &msg);

signals:

public slots:
    void replyFinished(QNetworkReply *);

private:
    QNetworkAccessManager *net;//发出http请求的对象
    OnMessage onMessage;
};

#endif // CITYMANAGER_H
