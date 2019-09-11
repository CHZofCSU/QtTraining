#include "dbhelper.h"

DBHelper *DBHelper::dBHelper = nullptr;

QString DBHelper::dbType = "QMYSQL";
QString DBHelper::dbHost = "127.0.0.1";
QString DBHelper::dbName = "vending_machine";
QString DBHelper::dbUser = "root";
QString DBHelper::dbPasswd = "123456";

DBHelper::DBHelper()
{
    //建立并打开数据库
    database = QSqlDatabase::addDatabase(dbType);
    database.setHostName(dbHost);
    database.setUserName(dbUser);
    database.setPassword(dbPasswd);
    database.setDatabaseName(dbName);

}

