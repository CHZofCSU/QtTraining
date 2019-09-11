#ifndef DBHELPER_H
#define DBHELPER_H
//添加头文件
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#define dBHelperInstance DBHelper::getInstance()

class DBHelper
{
public:
    static DBHelper* getInstance()
    {
        if(dBHelper == nullptr)
        {
            dBHelper = new DBHelper();
        }

        return dBHelper;
    }

private:
    DBHelper();//禁止构造函数
    DBHelper(const DBHelper&);//禁止拷贝构造函数
    DBHelper & operator=(const DBHelper &);//禁止赋值拷贝

public:
    QSqlDatabase database;

private:
    static DBHelper *dBHelper;

    static QString dbType;
    static QString dbHost;
    static QString dbName;
    static QString dbUser;
    static QString dbPasswd;
};

#endif // DBHELPER_H

