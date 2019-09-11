#include "Citymanager.h"

#include <QtNetwork>
#include <QTextCodec>
#include <QXmlStreamReader>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

Citymanager::Citymanager(QObject *parent) : QObject(parent)
{
    net = new QNetworkAccessManager(this);
    QObject::connect(net, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void Citymanager::getWeatherbyCityName(const QString &cityName, const OnMessage &msg)
{
    onMessage = msg;
    QString str("http://www.webxml.com.cn/WebServices/WeatherWebService.asmx/getWeatherbyCityName?theCityName=");
    str.append(cityName);
    QUrl url(str);
    QNetworkRequest request(url);
    net->get(request);
}

void Citymanager::replyFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString allMsg = codec->toUnicode(reply->readAll());

    QStringList list;
    //xml格式读取
    QXmlStreamReader reader(allMsg);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == "string") {
                list.append(reader.readElementText());
            }
        }
    }

    if (onMessage) {
        onMessage(list);
    }
    //用完后需要清除
    reply->deleteLater();
}
