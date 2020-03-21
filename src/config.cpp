
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QDebug>
#include "config.h"

LANGUAGE_TYPE Config::curLanguage = LANGUAGE_INVALID;

Config:: Config()
{

}

Config::~Config()
{

}

bool Config::saveConfig()
{
    qint64 writeSize = 0;
    QFile file;

    file.setFileName(CONFIG_FILE);
    if (false == file.open(QIODevice::WriteOnly))
    {
        qDebug() << "File open";
        return false;
    }

    writeSize = file.write((char*)&Config::curLanguage, sizeof(LANGUAGE_TYPE));
    if (writeSize < sizeof(LANGUAGE_TYPE))
    {
        qDebug() << "Bad file!";
        file.close();
        return false;
    }

    return true;
}


bool Config::readConfig()
{
    qint64 readSize = 0;
    QFile file;

    file.setFileName(CONFIG_FILE);
    if (false == file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File open";
        return false;
    }

    readSize = file.read((char*)&Config::curLanguage , sizeof(LANGUAGE_TYPE));
    if (readSize < sizeof(LANGUAGE_TYPE))
    {
        qDebug() << "Bad file!";
        file.close();
        return false;
    }

    return true;
}


void Config::globalInit()
{

    if (false == readConfig())
    {
        qDebug()<<"curLanguage "<<Config::curLanguage;
        Config::curLanguage = LANGUAGE_CHINESE;
    }

}



