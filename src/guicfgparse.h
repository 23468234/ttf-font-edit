#ifndef GUICFGPARSE_H
#define GUICFGPARSE_H


#include <QList>
#include <QCoreApplication>
#include <QRect>
#include <QListWidget>
#include <QTreeWidget>

class TsParse;
#include "tsparse.h"

#define TRANT_TO_TS_BY_SELECT 1 // 是否用循环对比souce的方式，转换ts

typedef struct
{
    QString ctrIdc; //IDC
    QString translation; //翻译
    QString ctrRect;  //坐标四个数
    quint32 listCount; //list 数目，<list:数目>，
    QList<QString> strList; //strList.count并不一定等于上面的strCount
}CtrlInfoS;
typedef struct
{
    QString fileName; //文件名称 confirm.txt
    QString dlgName; //dlg name
    QString dlgRect;
    QList<CtrlInfoS> ctrList; //控件列表
}CfgFileS;
typedef struct
{
    QString dirName;    //文件夹名称 chn
    QList<CfgFileS> cfgFileList; //一个文件 confirm.txt
}GuiConfigS;

class GuiCfgParse
{
    Q_DECLARE_TR_FUNCTIONS(GuiCfgParse);

private:
    bool openFlag;
    GuiConfigS guiConfig;

public:
    GuiCfgParse();
    ~GuiCfgParse();
    qint32 parseOneFile(QString fileName, CfgFileS & cfgFile);
    qint32 createOneFile(QString &dirPath, CfgFileS * oneFileInfo) const;
    void guiCfgOpen(QStringList &fileList);
    void guiCfgSave(QString & dirPath);
    void clear();
    bool isOpened();
    void fillListWidget(QListWidget *widget);
    void fillTreeWidget(QTreeWidget *treeWidget);
    qint32 translateToTs(TsParse &tsCfg);

private:
    friend class TsParse;

protected:
    void guiCfgOpenErrorBox(QString fileName, QString Source, qint32 line, const QString info);

};



#endif //GUICFGPARSE_H
