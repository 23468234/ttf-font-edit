#include "common.h"
#include "guicfgparse.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>


GuiCfgParse::GuiCfgParse()
{
    this->openFlag = false;
}


GuiCfgParse::~GuiCfgParse()
{

}

/***************************************************************
*Description:
   GUI配置文件标签
*Input:
    无
*Return:
    无
****************************************************************/
#define MINIGUICONFIGFILE   "mini gui config file"
#define DLGSTART            "<dlg>"
#define DLGEND              "</dlg>"
#define CTRSTART            "<control>"
#define CTREND              "</control>"
#define LISTSTART           "list:"
#define LISTEND             "</list>"

/***************************************************************
*Description:
    标题弹出框字符串信息
*Input:
    无
*Return:
    无
****************************************************************/
enum
{
    GUICFG_NOT_OPEN,
    GUICFG_NO_VALID_FILE,
    GUICFG_NOT_CFGFILE,
    GUICFG_NOT_DLG_TAG,
    GUICFG_NO_DLG_TAG,
    GUICFG_NOT_DLG_END,
    GUICFG_MISS_LIST_END,
    GUICFG_NOT_CTR_END,
    GUICFG_UNKNOWN_TAG,
    GUICFG_NOT_lIST_START,
    GUICFG_UNKNOWN_TXT,
    GUICFG_EMPTY_FILE,

    GUICFG_TS_NOT_OPEN,
    GUICFG_TS_LANGUAGE_NOT_MATCH,
    GUICFG_TS_CONTEXT_COUNT_NOT_MATCH,
    GUICFG_TS_CONTEXT_NAME_NOT_MATCH,
    GUICFG_TS_CONTEXT_CANT_FIND,
    GUICFG_TS_TRAN_CANT_FIND,
    GUICFG_TS_CTR_COUNT_NOT_MATCH,
    GUICFG_TS_CTR_SOURCE_NOT_MATCH,
};

const static char* guiError[] =
{
    QT_TRANSLATE_NOOP("GuiCfgParse", "MiniGUI config file not open!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "There is no valid txt file!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "It's not a gui config file!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "It's not a \"<dlg>\" tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "There is no \"<dlg> tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "It's not \"</dlg>\" tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Miss a \"</list>\" tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "It's not a \"</control>\" tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Unknown tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "It's not a \"<list:n>\" tag!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Unknown content!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Empty file!"),

    QT_TRANSLATE_NOOP("GuiCfgParse", "Ts file not opened!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Dir name not match the language of ts!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Context count not match!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Name not match!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Can't find out the file!"),

    QT_TRANSLATE_NOOP("GuiCfgParse", "Can't find out the item!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Control count not match!"),
    QT_TRANSLATE_NOOP("GuiCfgParse", "Control source not match!"),
};

/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
inline bool checkTheSame(const char* str, QString string)
{
    if (0 == string.compare(QString::fromUtf8(str),Qt::CaseSensitive))
    {
        return true;
    }
    return false;
}
/***************************************************************
*Description:
    isATagText() 如果是一个标签
*Input:
    无
*Return:
    无
****************************************************************/
inline bool isATagText(QString string)
{
    if (checkTheSame(DLGSTART, string)||
        checkTheSame(DLGEND, string)||
        checkTheSame(CTRSTART, string)||
        checkTheSame(CTREND, string)||
        checkTheSame(LISTSTART, string)||
        checkTheSame(LISTEND, string))
    {
        return true;
    }

    return false;
}
/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::guiCfgOpenErrorBox(QString fileName, QString Source, qint32 line, const QString info)
{
    QString errorStr;
    errorStr = fileName;
    errorStr += QString("  line:%1 \"%2\"   %3").arg(line).arg(Source).arg(info);

    QMessageBox::warning(NULL, tr("MiniGUI config parse"), errorStr, \
                                QMessageBox::No, QMessageBox::Yes);
    qDebug() << info;
}

/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
#define GUIOPENDEBUG(x) //(qDebug() << x)

qint32 GuiCfgParse::parseOneFile(QString filePath, CfgFileS &cfgFile)
{
    QFile file;
    qint32 line = 0;

    GUIOPENDEBUG(filePath);
    file.setFileName(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.errorString();
        guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), "", 1, file.errorString());
        return FAIL;
    }

    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF8"));

    /*Config file head*/
    line++;
    QString lineText = in.readLine().trimmed();
    if (!checkTheSame(MINIGUICONFIGFILE, lineText)) //第一行没有"mini gui config file"
    {
        qDebug() << "It is not a MiniGUI cfg file";

        guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), "", 1,  tr(guiError[GUICFG_NOT_CFGFILE]));
        file.close();
        return FAIL;
    }
    GUIOPENDEBUG(lineText);

    /*寻找<dlg>*/
    bool findFlag = false;
    while(!in.atEnd())
    {
        line++;
        lineText = in.readLine().trimmed();
        GUIOPENDEBUG(lineText);
        if (!lineText.isEmpty())
        {
            if (checkTheSame(DLGSTART, lineText))
            {
                findFlag = true;
                break;
            }
            else
            {
                qDebug() << "line"<< line <<"Not <dlg> tag but" << lineText;
                guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_NOT_DLG_TAG]));
                file.close();
                return FAIL;
            }
        }
    }

    if (!findFlag)
    {
        qDebug() << "Empty file!";
        guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), "", line, tr(guiError[GUICFG_EMPTY_FILE]));
        file.close();
        return FAIL;
    }

    line++;
    lineText = in.readLine();
    cfgFile.dlgName = lineText; //dlg 的名称
    GUIOPENDEBUG(lineText);

    line++;
    lineText = in.readLine();
    cfgFile.dlgRect = lineText; //dlg 的坐标
    GUIOPENDEBUG(lineText);

    line++;
    lineText = in.readLine().trimmed();
    if (!checkTheSame(DLGEND, lineText)) //如果不是</dlg>
    {
        qDebug() << QString("%1.txt").arg(cfgFile.fileName)
                    << "line"<< line <<"Not </dlg> tag but" << lineText;
        guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_NOT_DLG_END]));
        file.close();
        return FAIL;
    }
    GUIOPENDEBUG(lineText);


    /*control*/
    while(!in.atEnd())
    {
        line++;
        lineText = in.readLine();
        if (lineText.trimmed().isEmpty()) //忽略控件与控件之间的空格
        {
            GUIOPENDEBUG(" ");
            continue;
        }
        else if (checkTheSame(CTRSTART, lineText.trimmed())) //<control>
        {
            CtrlInfoS oneCtr;   //当前控件项
            GUIOPENDEBUG(lineText);

            line++;
            lineText = in.readLine();
            oneCtr.ctrIdc = lineText;  //IDC
            GUIOPENDEBUG(lineText);

            line++;
            lineText = in.readLine();
            oneCtr.translation = lineText; //翻译
            GUIOPENDEBUG(lineText);

            line++;
            lineText = in.readLine();
            oneCtr.ctrRect = lineText; //坐标
            GUIOPENDEBUG(lineText);

            line++;
            lineText = in.readLine().trimmed();
            GUIOPENDEBUG(lineText);
            if (!checkTheSame(CTREND, lineText)) //不是</control>，可能是 <list>
            {
                bool ok;
                QString listHead;
                quint32 count = 0;

                listHead = lineText.mid(1, 5);
                if (checkTheSame(LISTSTART, listHead)) // 第1-5个字节是"list:"
                {
                    count = lineText.mid(6, lineText.count() - 7).toInt(&ok, 10); //<list:count>
                    oneCtr.listCount = count;

                    if (ok && 0 < count)
                    {
                        findFlag = false;
                        while (!in.atEnd())
                        {
                            line++;
                            lineText = in.readLine();
                            GUIOPENDEBUG(lineText);
                            if (isATagText(lineText.trimmed())) //如果是一个标签
                            {
                                if (checkTheSame(LISTEND, lineText.trimmed())) //</list>
                                {
                                    findFlag = true;
                                    break;
                                }
                                else
                                {
                                    //qDebug() <<QString("%1.txt").arg(cfgFile.fileName) << "line" << line - 1 << "Miss a" << LISTEND;
                                    guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line - 1, tr(guiError[GUICFG_MISS_LIST_END]));
                                    file.close();
                                    return FAIL;
                                }
                            }

                            oneCtr.strList << lineText;  //控件项添加 list文本
                        }

                        if (!findFlag) //文件读完了，都没有找到</list>
                        {
                            //qDebug() <<QString("%1.txt").arg(cfgFile.fileName) << "line" << line - 1 << "Miss a" << LISTEND;
                            guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line - 1, tr(guiError[GUICFG_MISS_LIST_END]));
                            file.close();
                            return FAIL;
                        }

                        line++;
                        lineText = in.readLine().trimmed();
                        GUIOPENDEBUG(lineText);
                        if (!checkTheSame(CTREND, lineText)) //是不是</control>
                        {
                            qDebug() <<QString("%1.txt").arg(cfgFile.fileName)
                                        << "line"<< line << lineText << "Shold be:" << CTREND;

                            guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_NOT_CTR_END]));
                            file.close();
                            return FAIL;
                        }
                    }
                    else //count <= 0;
                    {
                        //qDebug() <<QString("%1.txt").arg(cfgFile.fileName) <<"line" << line << lineText << "unknown";
                        guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_UNKNOWN_TAG]));
                        file.close();
                        return FAIL;
                    }
                }
                else // 第1-5个字节不是 "list:"
                {
                    qDebug() <<QString("%1.txt").arg(cfgFile.fileName)<<"line" << line << lineText << "Shold be" << CTREND;
                    guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_NOT_CTR_END]));
                    file.close();
                    return FAIL;
                }
            } //if (!checkTheSame(CTREND, lineText))

            cfgFile.ctrList << oneCtr; //list中 加一个 control
        }
        else
        {
            //qDebug() <<QString("%1.txt").arg(cfgFile.fileName) <<"line" << line << lineText << "unknown";
            guiCfgOpenErrorBox(QString("%1.txt").arg(cfgFile.fileName), lineText, line, tr(guiError[GUICFG_UNKNOWN_TXT]));
            file.close();
            return FAIL;
        }
    }

    file.close();
    return SUCCESS;
}

/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::guiCfgOpen(QStringList &fileList)
{
    QDir dir;
    QFileInfo fileInfo;

    this->clear();

    if (fileList.isEmpty())
    {
        QMessageBox::critical(NULL, tr("MiniGUI config parse"),tr(guiError[GUICFG_NOT_OPEN]), \
                                    QMessageBox::No, QMessageBox::Yes);
        return;
    }

    /*获取文件所在的文件夹*/
    fileInfo = QFileInfo(fileList.at(0));
    dir.setPath(fileInfo.absolutePath());
    this->guiConfig.dirName = dir.dirName();
    //qDebug() << this->guiConfig.dirName;

    /*目录下的文件列表*/
    for(int i = 0;i < fileList.count(); ++i)
    {
        CfgFileS cfgFile;
        fileInfo = QFileInfo(fileList.at(i));
        cfgFile.fileName = fileInfo.baseName();
        //qDebug() << cfgFile.fileName;
        if (SUCCESS == parseOneFile(fileList.at(i), cfgFile))
        {
            this->guiConfig.cfgFileList << cfgFile;
        }
    }

    /*有minigui txt文件解析成功*/
    if (this->guiConfig.cfgFileList.isEmpty())
    {
        QMessageBox::critical(NULL, tr("MiniGUI config parse"),tr(guiError[GUICFG_NO_VALID_FILE]), \
                                     QMessageBox::No, QMessageBox::Yes);
        this->clear();
    }
    else
    {
        this->openFlag = true;
    }

}

/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
#define GUIWRITEDBG(x) (qDebug() << x)
#define RETURN_CHAR "\n"//换行符号
qint32 GuiCfgParse::createOneFile(QString &dirPath, CfgFileS * oneFileInfo) const
{
    QFile file;
    QString fileName = QDir::toNativeSeparators(dirPath);

    if (!fileName.endsWith(QDir::separator()))
    {
        fileName += QDir::separator();    /*加文件路径分隔符'\\'*/
    }
    fileName += oneFileInfo->fileName;
    fileName += ".txt";

    GUIWRITEDBG(fileName);

    file.setFileName(fileName);
#if 0
    if (file.exists())
    {
        QFile::remove(fileName);
        qDebug() << "rm " << fileName;
    }
#endif
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "open error";
        return FAIL;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF8"));

    out << QString::fromUtf8(MINIGUICONFIGFILE);    //mini gui config file
    out << RETURN_CHAR;
    out << RETURN_CHAR;

    out << QString::fromUtf8(DLGSTART); //<dlg>
    out << RETURN_CHAR;
    out << oneFileInfo->dlgName; //dlg name
    out << RETURN_CHAR;
    out << oneFileInfo->dlgRect; //dlg rect
    out << RETURN_CHAR;
    out << QString::fromUtf8(DLGEND); //<dlg>
    out << RETURN_CHAR;
    out << RETURN_CHAR;

    for (int i = 0; i < oneFileInfo->ctrList.count(); ++i)
    {
        CtrlInfoS oneCtr = oneFileInfo->ctrList.at(i);

        out << QString::fromUtf8(CTRSTART); //<control>
        out << RETURN_CHAR;
        out << oneCtr.ctrIdc;
        out << RETURN_CHAR;
        out << oneCtr.translation;
        out << RETURN_CHAR;
        out << oneCtr.ctrRect;
        out << RETURN_CHAR;

        if (!oneCtr.strList.isEmpty()) //list
        {
            out << QString("<list:%1>").arg(oneCtr.listCount);
            out << RETURN_CHAR;
            for(int j = 0; j < oneCtr.strList.count(); ++j)
            {
                out << oneCtr.strList.at(j);
                out << RETURN_CHAR;
            }
            out << QString::fromUtf8(LISTEND); //</list>
            out << RETURN_CHAR;
        }

        out << QString::fromUtf8(CTREND); //</control>
        out << RETURN_CHAR;
        out << RETURN_CHAR;
    }

    file.close();
    return SUCCESS;
}
/***************************************************************
*Description:
    deleteDir() 递归删除某个文件夹
*Input:
    无
*Return:
    无
****************************************************************/
bool deleteDir(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
    {
        srcPath += QDir::separator();    /*加文件路径分隔符'/'*/
    }

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }

    /*删除根目录*/
    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;
}
/***************************************************************
*Description:
    guiCfgSave() ts文件保存
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::guiCfgSave(QString &dirPath)
{
#if 0
    QDir dir;
    QString targetDir = QDir::toNativeSeparators(dirPath);
    if (!targetDir.endsWith(QDir::separator()))
    {
        targetDir += QDir::separator();    /*加文件路径分隔符'/'*/
    }
    targetDir += this->guiConfig.dirName;

    if (this->guiConfig.dirName.isEmpty())
    {
        return;
    }

    GUIWRITEDBG(targetDir);

    deleteDir(targetDir); //万一前面出了bug，这里会删错，要考虑是否删除

    if (!dir.mkdir(targetDir))
    {
        qDebug() << "why not create ok?";
        return;
    }
#endif
    for (int i = 0; i < this->guiConfig.cfgFileList.count(); i++)
    {
        CfgFileS itOneFile = this->guiConfig.cfgFileList.at(i);
        createOneFile(dirPath, &itOneFile);
    }
}

/***************************************************************
*Description:
    clear() GuiCfgParse 数据清空
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::clear()
{
    this->openFlag = false;
    this->guiConfig.dirName.clear();
    this->guiConfig.cfgFileList.clear();
}

bool GuiCfgParse::isOpened()
{
    return this->openFlag;
}
/***************************************************************
*Description:
    clear() translateToTs 数据清空,根据原ts文件信息，只更改ts信息的翻译项
*Input:
    无
*Return:
    无
****************************************************************/
qint32 GuiCfgParse::translateToTs(TsParse &tsCfg)
{
    QString errorStr;

    if (!tsCfg.isOpened())
    {
        QMessageBox::warning(NULL, tr("MiniGUI config parse"),tr(guiError[GUICFG_TS_NOT_OPEN]), \
                                     QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }
#if 0
    /*minigui配置文件夹的名称， 和ts文件的 language项不一样*/

    if (tsCfg.xmlFileInfo.tsInfo.language.compare(this->guiConfig.dirName))
    {
        QMessageBox::critical(NULL, BOXTITLE,tr(guiError[GUICFG_TS_LANGUAGE_NOT_MATCH]), \
                                     QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    /*ts文件的context数目 和minigui配置文件的txt文件数不一样*/
    /*屏蔽原因，只要在文本列表找到对应的txt即可，多的文件也不会处理*/
    if (tsCfg.xmlFileInfo.tranContext.count() != this->guiConfig.cfgFileList.count())
    {
        QMessageBox::critical(NULL, tr("MiniGUI config parse"),tr(guiError[GUICFG_TS_CONTEXT_COUNT_NOT_MATCH]) + tr(" contex : %1 MiniGUI cfg : %2") \
                              .arg(tsCfg.xmlFileInfo.tranContext.count()).arg(this->guiConfig.cfgFileList.count()), QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }
#endif

    qint32 txtIndex = 0;
    CfgFileS oneTxt;
    QList<TranContext_S>::iterator oneContext;
    /*一次循环处理一个 context*/
    for (oneContext = tsCfg.xmlFileInfo.tranContext.begin(); oneContext < tsCfg.xmlFileInfo.tranContext.end(); ++oneContext)
    {
        /*一个context 所有的type 都是 vanished*/
        if (tsCfg.isContextInvalid(*oneContext))
        {
            continue;
        }

        /*在txt中的列表找对应的项*/
        for (txtIndex = 0; txtIndex < this->guiConfig.cfgFileList.count(); ++txtIndex )
        {
            oneTxt = this->guiConfig.cfgFileList.at(txtIndex);
            if (oneTxt.fileName == oneContext->contextName)
            {
                /*minigui 一个txt文件的控件数目， 和ts一个context的 有效翻译项数*/
                if (oneTxt.ctrList.count() != tsCfg.getContextValidCount(*oneContext))
                {
                    errorStr = QString("\"%1.txt\" . ").arg(oneTxt.fileName);
                    errorStr += tr(guiError[GUICFG_TS_CTR_COUNT_NOT_MATCH]);
                    QMessageBox::critical(NULL, tr("MiniGUI config parse"), errorStr, QMessageBox::No, QMessageBox::Yes);
                    return FAIL;
                }

        #if (!(TRANT_TO_TS_BY_SELECT&& TRAN_SOURCE_TO_GUIIDC))
                QList <OneTextInfo_S>::iterator curText;
                QList <CtrlInfoS> ::iterator curCtl;
                curText = oneContext->textInfo.begin();
                curCtl = oneTxt.ctrList.begin();

                /*一次循环一个控件、ts一个翻译项*/
                while (curText < oneContext->textInfo.end())
                {
                    /*不用转换 vanished 的*/
                    if ("vanished" == curText->tranType)
                    {
                        curText++;
                        continue;
                    }
                    //qDebug () << curText->translation;
                    curText->translation = curCtl->translation;
                    if ( "unfinished" == curText->tranType && !curText->translation.isEmpty())
                    {
                        curText->tranType.clear();
                    }

                    curCtl++;
                    curText++;
                }
        #else
                QList <OneTextInfo_S>::iterator curText;
                QList <CtrlInfoS> ::iterator curCtl;
                curText = oneContext->textInfo.begin();
                curCtl = oneTxt.ctrList.begin();
                for (curText = oneContext->textInfo.begin(); curText < oneContext->textInfo.end(); ++curText)
                {
                    if ("vanished" == curText->tranType)
                    {
                        continue;
                    }

                    for (curCtl = oneTxt.ctrList.begin(); curCtl < oneTxt.ctrList.end(); ++curCtl)
                    {
                        if (curCtl->ctrIdc == curText->source)
                        {
                            curText->translation = curCtl->translation;
                            break;
                        }
                    }

                    /*没有找到*/
                    if (oneTxt.ctrList.end() == curCtl)
                    {
                        //qDebug () << curText->source;
                        errorStr = QString("context: %1, source: %2\n").arg(oneContext->contextName).arg(curText->source);
                        errorStr += tr(guiError[GUICFG_TS_TRAN_CANT_FIND]);
                        QMessageBox::critical(NULL, tr("MiniGUI config parse"), errorStr, QMessageBox::No, QMessageBox::Yes);
                        return FAIL;
                    }
                }
        #endif
                break;
            } //if
        } //for

        /*context 没有找到对应的项*/
        if (txtIndex == this->guiConfig.cfgFileList.count())
        {
            errorStr = QString("\"%1.txt\" . ").arg(oneContext->contextName);
            errorStr += tr(guiError[GUICFG_TS_CONTEXT_CANT_FIND]);
            QMessageBox::critical(NULL, tr("MiniGUI config parse"), errorStr, QMessageBox::No, QMessageBox::Yes);
            return FAIL;
        }
    } //for

    return SUCCESS;
}
/***************************************************************
*Description:
    txt 名称比较
*Input:
    无
*Return:
    无
****************************************************************/
bool txtLessThan(const CfgFileS &txt1, const CfgFileS &txt2)
{
    return txt1.fileName.toLower() < txt2.fileName.toLower();
}
/***************************************************************
*Description:
    翻译项source比较
*Input:
    无
*Return:
    无
****************************************************************/
bool ctrLessThan(const CtrlInfoS &ctr1, const CtrlInfoS  &ctr2)
{
    return ctr1.ctrIdc.toLower() < ctr2.ctrIdc.toLower();
}
/***************************************************************
*Description:
    fillListWidget() 信息填充到listwidget
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::fillListWidget(QListWidget *widget)
{
    if (NULL == widget)
    {
        return ;
    }

    widget->clear();
    for (int i = 0; i < this->guiConfig.cfgFileList.count(); ++i)
    {
        widget->addItem(QString("%1.txt").arg(this->guiConfig.cfgFileList.at(i).fileName));
    }
}
/***************************************************************
*Description:
    fillListWidget() 填充数据到QListWidget
*Input:
    无
*Return:
    无
****************************************************************/
void GuiCfgParse::fillTreeWidget(QTreeWidget *treeWidget)
{
    GuiConfigS curGuiCfg = this->guiConfig;

    if (NULL == treeWidget)
    {
        return ;
    }
    treeWidget->clear();
    treeWidget->setColumnCount(2);
    treeWidget->setColumnWidth(0, treeWidget->size().width() * 1 / 2);
    //qSort(curGuiCfg.cfgFileList.begin(), curGuiCfg.cfgFileList.end(), txtLessThan);

    /*tree 标题栏*/
    QStringList headers;
    headers << "id" << "translation";
    treeWidget->setHeaderLabels(headers);

    QList<QTreeWidgetItem *> rootList;
    for (int i = 0; i < curGuiCfg.cfgFileList.count(); ++i)
    {
        CfgFileS curFile = curGuiCfg.cfgFileList.at(i);
        //qSort(curFile.ctrList.begin(), curFile.ctrList.end(), ctrLessThan);

        /*根标签*/
        QStringList rootTextList;
        rootTextList << QString("%1").arg(curFile.fileName);
        QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget, rootTextList);
        //root->setExpanded(true);

        for (int j = 0; j < curFile.ctrList.count(); ++j)
        {
            CtrlInfoS curCtr = curFile.ctrList.at(j);

            /*子标签*/
            QStringList leafTextList;
            leafTextList << curCtr.ctrIdc << curCtr.translation;
            QTreeWidgetItem *leaf = new QTreeWidgetItem(root, leafTextList);
            root->addChild(leaf);

        }
        /*一个context一个根*/
        rootList << root;
    }
    treeWidget->insertTopLevelItems(0, rootList);

}
