#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>
#include <QTextCodec>

#include <QDebug>
#include <QHash>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Config::globalInit();
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    /*在globalInit() 函数中加载语言的话，没效果，没弄清*/
    QTranslator translator;
    switch(Config::curLanguage)
    {
        case LANGUAGE_ENGLISH:
        {
            translator.load(LANGUAGE_ENGLISH_FILE);
            break;
        }

        case LANGUAGE_CHINESE:
        {
            translator.load(LANGUAGE_CHINESE_FILE);
            break;
        }

        case LANGUAGE_KOREAN:
        {
            translator.load(LANGUAGE_KOREAN_FILE);
            break;
        }
        default:
           break;
    }
    a.installTranslator(&translator);

    a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return a.exec();
}

