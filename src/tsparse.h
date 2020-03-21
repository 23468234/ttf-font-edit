#ifndef XMLPARSE_H
#define XMLPARSE_H

#include <QList>
#include <QListWidget>
#include <QCoreApplication>
#include <QTreeWidget>

class GuiCfgParse;
#include "guicfgparse.h"

#define TRAN_SOURCE_TO_GUIIDC 0 //0：虚拟的写gui的IDC项，1：将ts source转换到gui ID

#define TSWRITEBYDOM 0 //0：一行一行写ts，1: 用系统的dom方法写ts文件

typedef struct //一个文本的文件位置信息
{
    QString fileName;
    QString line;
}TextLocalFile_S;
typedef struct
{
    QString tranType; //翻译类型， "vanished"
    QString source; //原文
    QString translation; //翻译
    QString oldsource; //以前的原文
    QString tranComment; //注释
    QList <TextLocalFile_S> localFile; //一个文本的文件位置信息
}OneTextInfo_S;
typedef struct //一个上下文
{
    QString contextName;
    QList <OneTextInfo_S> textInfo;
}TranContext_S;
typedef struct //一个xm文件
{
    struct
    {
        QString name;   //xml
        QString value;  //version="1.0" encoding="utf-8"
    }xmlVersion;
    struct
    {
        QString name;       // "TS"
        QString version;    // "2.1"
        QString language;   // "zh_CN"
    }tsInfo;
    QString docType;
    QList <TranContext_S> tranContext;
}XmlFile_S;

class TsParse
{
    Q_DECLARE_TR_FUNCTIONS(TsParse);

private:
    bool openFlag;
    XmlFile_S xmlFileInfo;

public:
    TsParse();
    ~TsParse();
    void clear();
    bool isOpened();
    void fillTreeWidget(QTreeWidget *treeWidget);
    void fillListWidget(QListWidget *widget);
    bool isContextInvalid(TranContext_S context);
    void readTsFile(QString &fileName);
    TsParse operator=(TsParse &xmlParse);
    qint32 getContextValidCount(TranContext_S context);
    qint32 checkTsFile(QString fileName);
    qint32 writeTs(QString &fileName);
    qint32 translateToGui(GuiCfgParse &guiCfg);

private:
    friend class GuiCfgParse;

protected:
    void showErrorTagBox(QString src, quint32 line, QString info);
    QString xmlSetContenError(QString errorMsg, qint32 line, qint32 column);
    QString xmlFrameTitle(int title);

};

#endif //XMLPARSE_H
