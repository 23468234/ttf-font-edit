
#include "common.h"
#include "tsparse.h"

#include <string.h>
#include <QDebug>
#include <QCoreApplication>
#include <QtXml>
#include <QFileDialog>
#include <QDomNodeList>
#include <QMessageBox>
#include <QRegExp>
#include <QApplication>
#include <QtAlgorithms>
#include <QTextCodec>

TsParse::TsParse()
{
    this->openFlag = false;
}

TsParse::~TsParse()
{
}

/***************************************************************
*Description:
    ts文件标签
*Input:
    无
*Return:
    无
****************************************************************/
#define XMLTAG          "xml"
#define DOCTYPE         "TS"
#define TSTAG           "TS"
#define TSVERSION       "2.1"
#define CONTEXTTAG      "context"
#define NAMETAG       "name"
#define MESSAGETAG      "message"
#define SOURCESTAG      "source"
#define OLDTAG          "oldsource"
#define TRCOMMENTTAG    "translatorcomment"
#define TRANTAG         "translation"
#define LOCATIONSTART   "location"

/***************************************************************
*Description:
    标题弹出框标题
*Input:
    无
*Return:
    无
****************************************************************/
enum
{
    XML_FRAME_CHECK,
    XML_FRAME_OPEN,
    XML_FRAME_SETCONTENT,
    XML_FRAME_TRTOGUI,
    XML_FRAME_WRITE

};

QString TsParse::xmlFrameTitle(int title)
{
    static const char *messageBoxTitle[] =
    {
        QT_TR_NOOP("Ts check"),
        QT_TR_NOOP("Ts open"),
        QT_TR_NOOP("Ts set content"),
        QT_TR_NOOP("Translate to gui cfg"),
        QT_TR_NOOP("Write ts file"),
    };
    return tr(messageBoxTitle[title]);
}
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
    XML_ERROR_OPEN,
    XML_ERROR_SETCONTENT,
    XML_ERROR_TSNOTOPENED,
};

const static char* xmlError[] =
{
    QT_TRANSLATE_NOOP("TsParse", "Ts file open error!"),
    QT_TRANSLATE_NOOP("TsParse", "Set content error!"),
    QT_TRANSLATE_NOOP("TsParse", "Ts file not opened!"),
};

/***************************************************************
*Description:
    xmlSetContenError() 系统打开xml文件错误翻译, 原文在 QXmlParseException 那一页介绍
*Input:
    无
*Return:
    无
****************************************************************/
QString TsParse::xmlSetContenError(QString errorMsg, qint32 line, qint32 column)
{
    const char* setContentErrorSource[] =
    {
        "no error occurred",
        "error triggered by consumer",
        "unexpected end of file",
        "more than one document type definition",
        "error occurred while parsing element",
        "tag mismatch",
        "error occurred while parsing content",
        "unexpected character",
        "invalid name for processing instruction",
        "version expected while reading the XML declaration",
        "wrong value for standalone declaration",
        "encoding declaration or standalone declaration expected while reading the XML declaration",
        "standalone declaration expected while reading the XML declaration",
        "error occurred while parsing document type definition",
        "letter is expected",
        "error occurred while parsing comment",
        "error occurred while parsing reference",
        "internal general entity reference not allowed in DTD",
        "external parsed general entity reference not allowed in attribute value",
        "external parsed general entity reference not allowed in DTD",
        "unparsed entity reference n wrong context",
        "recursive entities",
        "error in the text declaration of an external entity",
    };

    const static char* setContentErrorTrans[] =
    {
        QT_TR_NOOP("no error occurred"),
        QT_TR_NOOP("error triggered by consumer"),
        QT_TR_NOOP("unexpected end of file"),
        QT_TR_NOOP("more than one document type definition"),
        QT_TR_NOOP("error occurred while parsing element"),
        QT_TR_NOOP("tag mismatch"),
        QT_TR_NOOP("error occurred while parsing content"),
        QT_TR_NOOP("unexpected character"),
        QT_TR_NOOP("invalid name for processing instruction"),
        QT_TR_NOOP("version expected while reading the XML declaration"),
        QT_TR_NOOP("wrong value for standalone declaration"),
        QT_TR_NOOP("encoding declaration or standalone declaration expected while reading the XML declaration"),
        QT_TR_NOOP("standalone declaration expected while reading the XML declaration"),
        QT_TR_NOOP("error occurred while parsing document type definition"),
        QT_TR_NOOP("letter is expected"),
        QT_TR_NOOP("error occurred while parsing comment"),
        QT_TR_NOOP("error occurred while parsing reference"),
        QT_TR_NOOP("internal general entity reference not allowed in DTD"),
        QT_TR_NOOP("external parsed general entity reference not allowed in attribute value"),
        QT_TR_NOOP("external parsed general entity reference not allowed in DTD"),
        QT_TR_NOOP("unparsed entity reference n wrong context"),
        QT_TR_NOOP("recursive entities"),
        QT_TR_NOOP("error in the text declaration of an external entity"),
    };


    qint32 i = 0;
    qint32 errorCount = sizeof(setContentErrorSource) / sizeof (char*);
    QString outString;

    for (i = 0; i < errorCount; ++i)
    {
        if (setContentErrorSource[i] == errorMsg)
        {
            outString = tr(setContentErrorTrans[i]);
            //qDebug() << "find!";
            break;
        }
    }
    if (errorCount == i)
    {
        outString = errorMsg;
    }
    outString += tr("  line : %1  column : %2").arg(line).arg(column);

    return outString;
}

/***************************************************************
*Description:
    operator= 拷贝函数，浅拷贝
*Input:
    无
*Return:
    无
****************************************************************/
TsParse TsParse::operator=(TsParse &xmlParse)
{

    if (!xmlParse.isOpened())
    {
        this->openFlag = false;
        return *this;
    }
    this->openFlag = true;
    this->xmlFileInfo = xmlParse.xmlFileInfo;
    return *this;
}

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
    readXmlFile() 读ts文件
*Input:
    无
*Return:
    无
****************************************************************/
#define READTSDBG 0
void TsParse::readTsFile(QString &fileName)
{
    this->clear();

    /*ts 文件打开*/
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_OPEN), file.errorString(), \
                             QMessageBox::No, QMessageBox::Yes);
        return;
    }

    /*将文件内容读到doc*/
    QString errorStr;
    int errorLine;
    int errorCol;
    QDomDocument doc; //新建QDomDocument类对象，它代表一个XML文档
    if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorCol))
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_SETCONTENT), xmlSetContenError(errorStr ,errorLine ,errorCol), \
                             QMessageBox::No, QMessageBox::Yes);
        file.close();
        return;
    }
    file.close();

    /*doc的第一个子节点，文件头*/
    QDomNode firstNode = doc.firstChild();
    #if READTSDBG
        qDebug() << qPrintable(firstNode.nodeName())
                 <<qPrintable(firstNode.nodeValue()); //返回元素id属性的值
    #endif
    this->xmlFileInfo.xmlVersion.name = firstNode.nodeName(); //"xml"
    this->xmlFileInfo.xmlVersion.value = firstNode.nodeValue(); //"version="1.0" encoding="utf-8""

    this->xmlFileInfo.docType = doc.doctype().name();

    /*根节点 <TS>*/
    QDomElement docElem = doc.documentElement();  //返回根元素
    #if READTSDBG
         qDebug() << qPrintable(docElem.tagName())
         << "version"<<qPrintable(docElem.attribute("version"))
         << "language"<<qPrintable(docElem.attribute("language"));
    #endif
    this->xmlFileInfo.tsInfo.name = docElem.tagName();              //元素标记 "TS"
    this->xmlFileInfo.tsInfo.version = docElem.attribute("version"); //version的值 "2.1"
    this->xmlFileInfo.tsInfo.language =docElem.attribute("language"); //language的值"zh_CN"

    /*context 列表*/
    QDomNode contextNode = docElem.firstChild();   //返回根节点的第一个子节点
    while(!contextNode.isNull()) //如果节点不为空，循环完所有的上下文
    {
        if (contextNode.isElement())  //context 如果节点是元素
        {
            TranContext_S newContex;

            QDomElement contextElement = contextNode.toElement();  //将context节点转换为元素
            #if READTSDBG
                qDebug() << qPrintable(contextElement.tagName());
            #endif
            QDomNodeList messageList = contextElement.childNodes(); //获得元素context的所有子节点的列表

            /*name 标签*/
            QDomNode nameNode = messageList.at(0);                  //上下文名称 <name>MainWindow</name>
            #if READTSDBG
                qDebug() <<" "<< qPrintable(nameNode.toElement().tagName())
                                << qPrintable(nameNode.toElement().text()); //文本
            #endif
            newContex.contextName = nameNode.toElement().text();   //"MainWindow"

            /*message 列表*/
            for(int i = 1; i< messageList.count(); i++)
            {
                QDomNode messageNode = messageList.at(i);
                if(messageNode.isElement())
                {
                    OneTextInfo_S newText; //一个翻译项

                    QDomElement messageElement = messageNode.toElement();   //<message>
                    #if READTSDBG
                        qDebug() <<" "<< qPrintable(messageElement.tagName());
                    #endif

                    /*message 子节点列表 "source"、"translation"等*/
                    QDomNodeList textList = messageElement.childNodes();
                    for (int j = 0; j < textList.count(); j++) //遍历message列表
                    {
                        QDomNode text = textList.at(j);
                        if(text.isElement())
                        {
                            QString tagName = text.toElement().tagName();
                            QString tagText = text.toElement().text();

                            if (checkTheSame(LOCATIONSTART,tagName))    //<location>
                            {
                                TextLocalFile_S localFile;
                                localFile.fileName =  text.toElement().attribute("filename");
                                localFile.line = text.toElement().attribute("line");
                                newText.localFile << localFile;
                                #if READTSDBG
                                    qDebug() <<"   "<<tagName << text.toElement().attribute("filename")
                                                    << text.toElement().attribute("line");
                                #endif
                            }
                            else if (checkTheSame(SOURCESTAG,tagName)) //<source>
                            {
                                newText.source = text.toElement().text();
                                #if READTSDBG
                                    qDebug() <<"   "<< qPrintable(text.toElement().tagName())
                                                    <<qPrintable(text.toElement().text());
                                #endif

                            }
                            else if (checkTheSame(OLDTAG,tagName)) //<oldsource>
                            {
                                newText.oldsource = text.toElement().text();
                                #if READTSDBG
                                    qDebug() <<"   "<< qPrintable(text.toElement().tagName())
                                                    <<qPrintable(text.toElement().text());
                                #endif

                            }
                            else if (checkTheSame(TRANTAG,tagName)) //<translation>
                            {
                                newText.translation = text.toElement().text();
                                if (text.toElement().hasAttributes())
                                {
                                    newText.tranType= text.toElement().attribute("type");
                                }
                                #if READTSDBG
                                    qDebug() <<"   "<< qPrintable(text.toElement().tagName())
                                                    <<newText.tranType <<newText.translation;
                                #endif

                            }
                            else if (checkTheSame(TRCOMMENTTAG,tagName))//<translatorcomment>
                            {
                                newText.tranComment = text.toElement().text();
                                #if READTSDBG
                                    qDebug() <<"   "<< qPrintable(text.toElement().tagName())
                                            <<qPrintable(text.toElement().text());
                                #endif
                            }
                        }
                    }

                    newContex.textInfo << newText;
                }
            }

            this->xmlFileInfo.tranContext << newContex;
        }
        contextNode = contextNode.nextSibling();  //下一个兄弟节点
    }

    this->openFlag = true;
}

/***************************************************************
*Description:
    translateOperatorOut() xml特殊字符转换
*Input:
    无
*Return:
    无
****************************************************************/
#if (0 == TSWRITEBYDOM)
void translateOperatorOut(QTextStream& out, QString str)
{

    for (int i = 0; i < str.count(); ++i)
    {

        if ('&' == str.at(i))
        {
             out << "&amp;";
        }
        else if ('<' == str.at(i))
        {
            out << "&lt;";
        }
        else if ('>' == str.at(i))
        {
            out << "&gt;";
        }
        else if ('\"' == str.at(i))
        {
             out << "&quot;";
        }
        else if ('\'' == str.at(i))
        {
            out << "&apos;";
        }
        else
        {
            out << str.at(i);
        }
    }
}

/***************************************************************
*Description:
    writeTs() 写ts文件
*Input:
    无
*Return:
    无
****************************************************************/
#define FOURBLANK       "    "
#define EIGHTBLANK      "        "
#define TAGSTART        "<%1>"
#define TAGEEND         "</%1>"
#define HEADATTR        "<?%1 %2?>"
#define TSATTRIBUTE     "<%1 version=\"%2\" language=\"%3\">"
#define LOCATTRIBUTE    "<%1 filename=\"%2\" line=\"%3\"/>"
#define TRTYPEATTR      "<translation type=\"%1\">"
#define RETURN_CHAR     "\n"
qint32 TsParse::writeTs(QString &fileName)
{
    if (!this->isOpened())
    {
        QMessageBox::warning(NULL, xmlFrameTitle(XML_FRAME_WRITE), tr(xmlError[XML_ERROR_TSNOTOPENED]), \
                             QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_OPEN), tr(xmlError[XML_ERROR_OPEN]) + file.errorString(), \
                                  QMessageBox::No, QMessageBox::Yes);
        return FAIL; //只写方式打开，并清空以前的信息
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF8"));

    /*写文件头*/
    out << QString(HEADATTR).arg(this->xmlFileInfo.xmlVersion.name)
                                        .arg("version=\"1.0\" encoding=\"utf-8\"");

    /*写DOCTYPE*/
    out << RETURN_CHAR;
    out << QString("<!DOCTYPE TS>");
    out << RETURN_CHAR;

    /*写<TS>*/
    out << QString(TSATTRIBUTE).arg(this->xmlFileInfo.tsInfo.name).arg(this->xmlFileInfo.tsInfo.version)
                                                                .arg(this->xmlFileInfo.tsInfo.language);
    out << RETURN_CHAR;

    QList<TranContext_S>::const_iterator context;
    /*context 标签*/
    for (context = this->xmlFileInfo.tranContext.begin(); context < this->xmlFileInfo.tranContext.end(); ++context)
    {
        /*<context>*/
        out << QString(TAGSTART).arg(CONTEXTTAG);
        out << RETURN_CHAR;

        /*name*/
        out << QString(FOURBLANK); //"空格"
        out << QString(TAGSTART).arg(NAMETAG);
        out << context->contextName;
        out << QString(TAGEEND).arg(NAMETAG);
        out << RETURN_CHAR;

        /*一项翻译*/
        QList<OneTextInfo_S>::const_iterator oneText;
        for (oneText = context->textInfo.begin(); oneText < context->textInfo.end(); ++oneText)
        {
            /*<message>*/
            out << QString(FOURBLANK); //"空格"
            out << QString(TAGSTART).arg(MESSAGETAG);
            out << RETURN_CHAR;

            /*字符路径 location*/
            QList<TextLocalFile_S>::const_iterator oneLocal;
            for (oneLocal = oneText->localFile.begin(); oneLocal < oneText->localFile.end(); ++oneLocal)
            {
                out << QString(EIGHTBLANK); //"空格"
                out << QString(LOCATTRIBUTE).arg(LOCATIONSTART).arg(oneLocal->fileName).arg(oneLocal->line);
                out << RETURN_CHAR;
            }

            /*<source>*/
            //if (!oneText->source.isEmpty())
            {
                out << QString(EIGHTBLANK); //"空格"
                out << QString(TAGSTART).arg(SOURCESTAG);
                translateOperatorOut(out, oneText->source);
                out << QString(TAGEEND).arg(SOURCESTAG);
                out << RETURN_CHAR;
            }

            /*<oldsource>*/
            if (!oneText->oldsource.isEmpty())
            {
                out << QString(EIGHTBLANK); //"空格"
                out << QString(TAGSTART).arg(OLDTAG);
                translateOperatorOut(out, oneText->oldsource);
                out << QString(TAGEEND).arg(OLDTAG);
                out << RETURN_CHAR;
            }

            /*<tranComment>*/
            if (!oneText->tranComment.isEmpty())
            {
                out << QString(EIGHTBLANK); //"空格"
                out << QString(TAGSTART).arg(TRCOMMENTTAG);
                translateOperatorOut(out, oneText->tranComment);
                out << QString(TAGEEND).arg(TRCOMMENTTAG);
                out << RETURN_CHAR;
            }

            /*<translation>*/
            //if (!oneText->translation.isEmpty()) //souce 项和translation项必定有
            {
                out << QString(EIGHTBLANK); //"空格"
                if (!oneText->tranType.isEmpty())
                {
                    out << QString(TRTYPEATTR).arg(oneText->tranType);
                }
                else
                {
                    out << QString(TAGSTART).arg(TRANTAG);
                }
                translateOperatorOut(out, oneText->translation);
                out << QString("</%1>").arg(TRANTAG);
                out << RETURN_CHAR;
            }

            /*</message>*/
            out << QString(FOURBLANK); //"空格"
            out << QString(TAGEEND).arg(MESSAGETAG);
            out << RETURN_CHAR;

        }
        /*</context>*/
        out << QString(TAGEEND).arg(CONTEXTTAG);
        out << RETURN_CHAR;
    }
    /*</TS>*/
    out << QString(TAGEEND).arg(TSTAG);
    file.close();
    return SUCCESS;
}
#else
#define FILEINSTUCTION "<?%1 %2?>"
#define FILETYPE "<!DOCTYPE TS>"
#define RETURNEX "\r\n"

qint32 TsParse::writeTs(QString &fileName)
{
    if (!this->isOpened())
    {
        QMessageBox::warning(NULL, xmlFrameTitle(XML_FRAME_WRITE), tr(xmlError[XML_ERROR_TSNOTOPENED]), \
                             QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_OPEN), tr(xmlError[XML_ERROR_OPEN]) + file.errorString(), \
                                  QMessageBox::No, QMessageBox::Yes);
        return FAIL; //只写方式打开，并清空以前的信息
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF8"));

    QDomText text;
    QDomDocument doc(this->xmlFileInfo.docType); //<!DOCTYPE TS>
    QDomElement root;

    /*文件说明*/
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction(this->xmlFileInfo.xmlVersion.name, "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(instruction);

    /*创建根节点*/
    root = doc.createElement(this->xmlFileInfo.tsInfo.name);
    {
        QDomAttr attr1, attr2;
        /*添加root的version属性*/
        attr1 = doc.createAttribute("version");
        attr1.setValue(this->xmlFileInfo.tsInfo.version);
        root.setAttributeNode(attr1);
        /*添加root的language属性*/
        attr2 = doc.createAttribute("language");
        attr2.setValue(this->xmlFileInfo.tsInfo.language);
        root.setAttributeNode(attr2);
    }
    doc.appendChild(root); //添加根元素


    QList<TranContext_S>::const_iterator context;
    /*context 标签*/
    for (context = this->xmlFileInfo.tranContext.begin(); context < this->xmlFileInfo.tranContext.end(); ++context)
    {
        //qDebug()<<context->contextName;
        QDomElement contextElement = doc.createElement("context");
        QDomElement nameElement = doc.createElement("name");
        text = doc.createTextNode(context->contextName);
        nameElement.appendChild(text);
        contextElement.appendChild(nameElement);

        /*一项翻译*/
        QList<OneTextInfo_S>::const_iterator oneText;
        for (oneText = context->textInfo.begin(); oneText < context->textInfo.end(); ++oneText)
        {
            QDomElement messageElement = doc.createElement("message");;

            QList<TextLocalFile_S>::const_iterator oneLocal;
            for (oneLocal = oneText->localFile.begin(); oneLocal < oneText->localFile.end(); ++oneLocal)
            {
                QDomAttr attr1, attr2;
                QDomElement localElement = doc.createElement("location");

                /*添加location的filename属性*/
                attr1 = doc.createAttribute("filename");
                attr1.setValue(oneLocal->fileName);
                localElement.setAttributeNode(attr1);

                /*添加location的line属性*/
                attr2 = doc.createAttribute("line");
                attr2.setValue(oneLocal->line);
                localElement.setAttributeNode(attr2);

                messageElement.appendChild(localElement);
                //qDebug() << "   " << "local" << oneLocal->fileName << oneLocal->line;
            }

            QDomElement infoElement;
            //if (!oneText->source.isEmpty()) //source项一定有
            {
                infoElement = doc.createElement("source");
                text = doc.createTextNode(oneText->source);
                infoElement.appendChild(text);
                messageElement.appendChild(infoElement);
            }

            if (!oneText->oldsource.isEmpty())
            {
                infoElement = doc.createElement("oldsource");
                text = doc.createTextNode(oneText->oldsource);
                infoElement.appendChild(text);
                messageElement.appendChild(infoElement);
            }

            if (!oneText->tranComment.isEmpty())
            {
                infoElement = doc.createElement("translatorcomment");
                text = doc.createTextNode(oneText->tranComment);
                infoElement.appendChild(text);
                messageElement.appendChild(infoElement);
            }
            //if (!oneText->translation.isEmpty()) //translation 项肯定有
            {
                QDomAttr attr;
                infoElement = doc.createElement("translation");
                if (!oneText->tranType.isEmpty())
                {
                    attr = doc.createAttribute("type");
                    attr.setValue(oneText->tranType);
                    infoElement.setAttributeNode(attr);
                }

                text = doc.createTextNode(oneText->translation);
                infoElement.appendChild(text);
                messageElement.appendChild(infoElement);
            }

            //qDebug() << "   " << "source" << oneText->source;
            //qDebug() << "   " << "translation" << oneText->tranType << oneText->translation;
            //qDebug() << "   " << "oldsource" << oneText->oldsource;
            //qDebug() << "   " << "tranComment" << oneText->tranComment;
            contextElement.appendChild(messageElement);
        }

        root.appendChild(contextElement);
    }

    doc.save(out, 4, QDomNode::EncodingFromDocument);
    file.close();
    return SUCCESS;
}
#endif
/***************************************************************
*Description:
    showErrorTagBox() 校验ts错误弹出框
*Input:
    无
*Return:
    无
****************************************************************/
void TsParse::showErrorTagBox(QString src, quint32 line, QString info )
{
    QString errorStr;

    errorStr = QString("line %1 :\"").arg(line);
    errorStr += src;
    errorStr += tr("\",  %1 ").arg(info);
    QMessageBox::critical(QApplication::activeWindow(), xmlFrameTitle(XML_FRAME_CHECK), errorStr,
                         QMessageBox::No, QMessageBox::Yes);
}

/***************************************************************
*Description:
    checkTsFile() 校验ts文件内容
*Input:
    无
*Return:
    无
****************************************************************/
qint32 TsParse::checkTsFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))//以只读方式打开
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_OPEN), file.errorString(), \
                             QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    QString errorStr;
    int errorLine;
    int errorCol;
    QDomDocument doc; //新建QDomDocument类对象，它代表一个XML文档, 将文件内容读到doc中
    if (!doc.setContent(&file, true, &errorStr, &errorLine, &errorCol))
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_SETCONTENT), xmlSetContenError(errorStr ,errorLine ,errorCol), \
                             QMessageBox::No, QMessageBox::Yes);
        file.close();
        return FAIL;
    }
    file.close();

    /*文件头*/
    QDomNode firstNode = doc.firstChild();
    if (!checkTheSame(XMLTAG, firstNode.nodeName()))
    {
        showErrorTagBox(firstNode.nodeName(), firstNode.lineNumber(), tr("Should be : \"%1\"").arg(XMLTAG));
        return FAIL;
    }

    /*DOCTYPE*/
    if (!checkTheSame(DOCTYPE, doc.doctype().name()))
    {
        showErrorTagBox(doc.doctype().name(), 2,  tr("Should be : \"%1\"").arg(DOCTYPE) );
        return FAIL;
    }


    /*根元素 TS*/
    QDomElement docElem = doc.documentElement(); //返回根元素
    if (!checkTheSame(TSTAG, docElem.tagName())) //"是否 ts 标签"
    {
        showErrorTagBox(docElem.tagName(), docElem.lineNumber(), tr("Should be : \"%1\"").arg(TSTAG));
        return FAIL;
    }

    if (!checkTheSame(TSVERSION, docElem.attribute("version")))
    {
        showErrorTagBox(docElem.tagName(), docElem.lineNumber(), tr("version : %1 , Should be : \"%2\"").arg(docElem.attribute("version")).arg(TSVERSION));
        return FAIL;
    }
#if 0
    if (docElem.attribute("language").isEmpty())
    {
        showErrorTagBox(docElem.tagName(), docElem.lineNumber(), tr("Ts language empty. Use Linguist to set language"));
        return FAIL;
    }
#endif
    /*context 列表*/
    QDomNode contextNode = docElem.firstChild(); //返回TS节点的第一个子节点
    while(!contextNode.isNull()) //如果节点不为空，循环完所有的context
    {
        if (contextNode.isElement()) //context 如果节点是元素
        {
            /*context*/
            QDomElement contextElement = contextNode.toElement(); //将context节点转换为元素
            if (!checkTheSame(CONTEXTTAG, contextElement.tagName())) //"是否 context 标签"
            {
                showErrorTagBox(contextElement.tagName(), contextElement.lineNumber(), tr("Should be : \"%1\"").arg(CONTEXTTAG));
                return FAIL;
            }
            QDomNodeList messageList = contextElement.childNodes(); //获得元素context的所有子节点的列表

            /*name 标签*/
            QDomNode nameNode = messageList.at(0); //上下文名称 <name>MainWindow</name>
            if (!checkTheSame(NAMETAG, nameNode.toElement().tagName())) //"是否 name 标签"
            {
                showErrorTagBox(nameNode.toElement().tagName(), nameNode.lineNumber(),tr("Should be : \"%1\"").arg(NAMETAG));
                return FAIL;
            }

            /*message 标签列表 */
            for(int i = 1; i< messageList.count(); i++)
            {
                qint32 traCount = 0;
                qint32 srcCount = 0;
                qint32 oldCount = 0;
                qint32 comCount = 0;
                QDomNode messageNode = messageList.at(i);
                if(messageNode.isElement())
                {

                    QDomElement messageElement = messageNode.toElement();   //<message>
                    if (!checkTheSame(MESSAGETAG, messageElement.tagName())) //"是否 message 标签"
                    {
                        showErrorTagBox(messageElement.tagName(), messageElement.lineNumber(), tr("Should be : \"%1\"").arg(MESSAGETAG));
                        return FAIL;
                    }

                    /*message 子节点列表 "source"、"translation"等*/
                    QDomNodeList textList = messageElement.childNodes();
                    for (int j = 0; j < textList.count(); j++)           //遍历message列表
                    {
                        QDomNode text = textList.at(j);
                        if(text.isElement())
                        {
                            QString tagName = text.toElement().tagName();
                            QString tagText = text.toElement().text();

                            if (checkTheSame(LOCATIONSTART,tagName))    //<location>
                            {
                            }
                            else if (checkTheSame(SOURCESTAG,tagName)) //<source>
                            {
                                if(text.toElement().text().isEmpty()) //source为空
                                {
                                    showErrorTagBox(tagName, text.lineNumber(), tr("<source> NULL"));
                                    return FAIL;
                                }

                                srcCount++;
                                if (srcCount > 1) //重复
                                {
                                    showErrorTagBox(tagName, text.lineNumber(), tr("Message repeated child tag <source>"));
                                    return FAIL;
                                }
                            }
                            else if (checkTheSame(OLDTAG,tagName)) //<oldsource>
                            {
                                oldCount++;
                                if (oldCount > 1) //重复
                                {
                                    showErrorTagBox(tagName, text.lineNumber(), tr("Message repeated child tag <oldsource>"));
                                    return FAIL;
                                }
                            }
                            else if (checkTheSame(TRANTAG,tagName)) //<translation>
                            {
                                traCount++;
                                if (traCount > 1) //重复
                                {
                                    showErrorTagBox(tagName, text.lineNumber(), tr("Message repeated child tag <translation>"));
                                    return FAIL;
                                }
                            }
                            else if (checkTheSame(TRCOMMENTTAG,tagName))//<translatorcomment>
                            {
                                comCount++;
                                if (comCount > 1) //重复
                                {
                                                                        showErrorTagBox(tagName, text.lineNumber(), tr("Message repeated child tag <translatorcomment>"));
                                    return FAIL;
                                }
                            }
                            else
                            {
                                showErrorTagBox(tagName, text.lineNumber(), tr("Unknown tag"));
                                return FAIL;
                            }
                        }
                    }

                    if (srcCount == 0 || traCount == 0) //缺项
                    {
                        showErrorTagBox(messageElement.tagName(), messageElement.lineNumber(), tr("Message miss <translation> or <source> child"));
                        return FAIL;
                    }
                }
            }
        }
        contextNode = contextNode.nextSibling();  //下一个兄弟节点
    }

 return SUCCESS;
}



/***************************************************************
*Description:
    clearXmlParse() 清除结构体信息
*Input:
    无
*Return:
    无
****************************************************************/
void TsParse::clear()
{
    this->openFlag = false;
    this->xmlFileInfo.xmlVersion.name.clear();
    this->xmlFileInfo.xmlVersion.value.clear();
    this->xmlFileInfo.tsInfo.name.clear();
    this->xmlFileInfo.tsInfo.language.clear();
    this->xmlFileInfo.tsInfo.version.clear();
    this->xmlFileInfo.tranContext.clear();
}

bool TsParse::isOpened()
{
    return this->openFlag;
}
/***************************************************************
*Description:
    clearXmlParse() 清除结构体信息
*Input:
    无
*Return:
    无
****************************************************************/
#define VITRULRECT "0;;0;;0;;0"
#define VITRULIDC "IDC_CONTRL%1"
qint32 TsParse::translateToGui(GuiCfgParse &guiCfg)
{
    if (!this->isOpened())
    {
        QMessageBox::critical(NULL, xmlFrameTitle(XML_FRAME_TRTOGUI), tr(xmlError[XML_ERROR_TSNOTOPENED]), \
                             QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    guiCfg.clear();
    guiCfg.guiConfig.dirName = this->xmlFileInfo.tsInfo.language; //文件夹名，用ts语言项来命名

    for (int i = 0; i < this->xmlFileInfo.tranContext.count(); ++i) //一个context
    {
        CfgFileS guiNewFile;
        TranContext_S curContext = this->xmlFileInfo.tranContext.at(i);

        /*整个 context 都是无效的*/
        if (isContextInvalid(curContext))
        {
            continue;
        }

        guiNewFile.fileName = curContext.contextName;
        guiNewFile.dlgName = guiNewFile.fileName;
        guiNewFile.dlgRect = QString::fromUtf8(VITRULRECT);

        for (int j = 0; j < curContext.textInfo.count(); ++j) //一项文本翻译
        {
            CtrlInfoS guiNewCtr;
            OneTextInfo_S tsOneTxt = curContext.textInfo.at(j);

            /*无效项不要转换过去*/
            if ("vanished" == tsOneTxt.tranType)
            {
                continue;
            }

            guiNewCtr.ctrRect = QString::fromUtf8(VITRULRECT);
            guiNewCtr.translation = tsOneTxt.translation;

        #if !TRAN_SOURCE_TO_GUIIDC
            guiNewCtr.ctrIdc = QString(VITRULIDC).arg(j + 1);
        #else
            guiNewCtr.ctrIdc = tsOneTxt.source;
        #endif
            //guiNewCtr.ctrIdc = QString::fromUtf8(VITRULIDC).arg(j + 1);

            guiNewFile.ctrList << guiNewCtr;
        }

        guiCfg.guiConfig.cfgFileList << guiNewFile;
    }

    return SUCCESS;
}
/***************************************************************
*Description:
    getContextValidCount() context中所有的非vanished数
*Input:
    无
*Return:
    无
****************************************************************/
qint32 TsParse::getContextValidCount(TranContext_S context)
{
    qint32 count = 0;

    if (context.textInfo.isEmpty())
    {
        return 0;
    }

    for (int i = 0; i < context.textInfo.count(); ++i)
    {
        if ("vanished" != context.textInfo.at(i).tranType)
        {
            count++;
        }
    }
    return count;
}

/***************************************************************
*Description:
    isContextInvalid() context中所有的tranlation type 都是 vanished
*Input:
    无
*Return:
    无
****************************************************************/
bool TsParse::isContextInvalid(TranContext_S context)
{
    if (0 < getContextValidCount(context))
    {
        return false;
    }
    return true;
}
/***************************************************************
*Description:
    context 名称比较
*Input:
    无
*Return:
    无
****************************************************************/
bool contextLessThan(const TranContext_S &context1, const TranContext_S &contex2)
{
    return context1.contextName.toLower() < contex2.contextName.toLower();
}
/***************************************************************
*Description:
    翻译项source比较
*Input:
    无
*Return:
    无
****************************************************************/
bool tranItemLessThan(const OneTextInfo_S &tranItem1, const OneTextInfo_S  &tranItem2)
{
    return tranItem1.source.toLower() < tranItem2.source.toLower();
}

/***************************************************************
*Description:
    clearXmlParse() 清除结构体信息
*Input:
    无
*Return:
    无
****************************************************************/
void TsParse::fillListWidget(QListWidget *widget)
{
    if (NULL == widget)
    {
        return ;
    }

    widget->clear();
    for (int i = 0; i < this->xmlFileInfo.tranContext.count(); ++i)
    {
        if (!isContextInvalid(this->xmlFileInfo.tranContext.at(i) ))
        {
              widget->addItem(this->xmlFileInfo.tranContext.at(i).contextName);
        }
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

void TsParse::fillTreeWidget(QTreeWidget *treeWidget)
{
    XmlFile_S tempInfo = this->xmlFileInfo;

    if (NULL == treeWidget)
    {
        return ;
    }
    treeWidget->clear();
    treeWidget->setColumnCount(2);
    treeWidget->setColumnWidth(0, treeWidget->size().width() / 2);
    treeWidget->setColumnWidth(1, treeWidget->size().width() / 2);

    //qSort(tempInfo.tranContext.begin(), tempInfo.tranContext.end(), contextLessThan);
    /*tree 标题栏*/
    QStringList headers;
    headers << "source" << "translation";
    treeWidget->setHeaderLabels(headers);

    QList<QTreeWidgetItem *> rootList;
    for (int i = 0; i < this->xmlFileInfo.tranContext.count(); ++i)
    {
        TranContext_S curContex = tempInfo.tranContext.at(i);

        /*这个context*/
        if (isContextInvalid(curContex))
        {
            continue;
        }
        /*根标签*/
        QStringList rootTextList;
        rootTextList << curContex.contextName;
        QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget, rootTextList);
        //root->setExpanded(true);
        //qSort(curContex.textInfo.begin(), curContex.textInfo.end(), tranItemLessThan);

        for (int j = 0; j < curContex.textInfo.count(); ++j)
        {
            OneTextInfo_S oneTran = curContex.textInfo.at(j);
            if ("vanished" == oneTran.tranType)
            {
                continue;
            }

            /*子标签*/
            QStringList leafTextList;
            leafTextList << oneTran.source << oneTran.translation; // << oneTran.tranType;
            QTreeWidgetItem *leaf = new QTreeWidgetItem(root, leafTextList);
            root->addChild(leaf);

        }
        /*一个context一个根*/
        rootList << root;
    }
    treeWidget->insertTopLevelItems(0, rootList);

}
