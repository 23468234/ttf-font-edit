#include "characterstatistics.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

QMap<qint32, quint16> CharacterStatistics::unicodeToGb2312Map;
QMap<qint32, quint16> CharacterStatistics::unicodeToGbkMap;

CharacterStatistics::CharacterStatistics()
{
    if (0 == unicodeToGbkMap.count())
    {
        this->createUnicodeMatchGb2312(unicodeToGb2312Map);
    }

    if(0 == unicodeToGbkMap.count())
    {
        this->createUnicodeMatchGbk(unicodeToGbkMap);
    }
}

CharacterStatistics::~CharacterStatistics()
{

}

/***************************************************************
*Description:  将一项中文翻译信息统计到map
*Input: line: 文本行数， tranString：翻译文本
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
void CharacterStatistics::parseUtf8StringChinese(qint32 line, QString tranString, QMap<qint32, CharInfoList> & map)
{
    qint32 index;
    quint16 curUnicode = 0;
    CharInfoList infoList;
    ONE_CHAR_INFO_S charInfo;

    QList <qint32> curLineCodeList; //临时变量，一项翻译中多个相同字符，只统计一个

    /*一行中文，转换成unicode*/
    for (int i = 0; i < tranString.count(); ++i)
    {
        /*当前字符的unicode*/
        curUnicode = tranString.at(i).unicode();

        for (index = 0; index < curLineCodeList.count(); ++index)
        {
            if (curUnicode == curLineCodeList.at(index))
            {
                break;
            }
        }

        /*当前行一种字符，只添加一次*/
        /*没找到，说明没有添加过*/
        if (index == curLineCodeList.count())
        {
            charInfo.line = line;
            charInfo.text = tranString;

            /*当前字符在map中的信息*/
            infoList = map.value(curUnicode);
            infoList.append(charInfo);
            map.insert(curUnicode, infoList);
        }

        /*临时的存unicode，同一行的相同字符，别重复添加*/
        curLineCodeList << curUnicode;
    }

    if (1)//64319 == line)
    {
        //qDebug () << tranString;
    }
}

/***************************************************************
*Description:  打开gui 字典并统计其中的字符个数
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
#define TRANSLATIONSTART "####"
bool CharacterStatistics::openDicFile(QString fileName)
{
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(NULL, tr("Dictionary file open"), file.errorString(), \
                                    QMessageBox::No, QMessageBox::Yes);
        return false;
    }

    /*清空map 数据*/
    charChineseMap.clear();

    /*数据流*/
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("UTF8"));

    QString lineText;
    qint32 line = 0;
    while(!in.atEnd())
    {
        line++;
        lineText = in.readLine().trimmed();

        /*找到了####*/
        if (0 == lineText.compare(TRANSLATIONSTART))
        {
            line++;
            lineText = in.readLine();
            parseUtf8StringChinese(line, lineText, charChineseMap);
            //qDebug () << lineText;
        }
    }

    //qDebug() << charChineseMap.count();
    file.close();
    return true;
}

/***************************************************************
*Description:  创建unicode和gb2312的映射
*Input: 无
*Output: unicode -- gb2312码的 map
*Return: 无
****************************************************************/
void  CharacterStatistics::createUnicodeMatchGb2312(QMap<qint32, quint16> & map)
{
    QTextCodec *textCodec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    quint8 areaCode, digitCode;
    quint16 gb2312Code = 0;

    map.clear();
    /*1 - 9 区*/
    for(areaCode = 0xa1; areaCode < 0xaa; areaCode++)
    {
        for(digitCode = 0xa1; digitCode < 0xff; digitCode++)
        {
            char oneWord[2] = {(char)areaCode, (char)digitCode};
            QString str =  QString::fromLocal8Bit(oneWord, 2);

            //qDebug() <<QString("").arg(str.at(i).unicode(), );
            //qDebug("{0x%04x,0x%02x%02x},", str.at(0).unicode(), digitCode, digitCode);
            gb2312Code = areaCode * 256 + digitCode;
            map.insert(str.at(0).unicode(), gb2312Code);
        }
    }

    /*16 - 87 区*/
    for(areaCode = 0xb0; areaCode < 0xf8; areaCode++)
    {
        for(digitCode = 0xa1; digitCode < 0xff; digitCode++)
        {
            char oneWord[2] = {(char)areaCode, (char)digitCode};
            QString str =  QString::fromLocal8Bit(oneWord, 2);

            //qDebug("{0x%04x,0x%02x%02x},", str.at(0).unicode(), digitCode, digitCode);
            gb2312Code = areaCode * 256 + digitCode;
            map.insert(str.at(0).unicode(), gb2312Code);
        }
    }

    QTextCodec::setCodecForLocale(textCodec);

#if 0
    QMap <qint32 ,quint16>::const_iterator mapIterator;
    for (mapIterator = map.constBegin(); mapIterator != map.constEnd(); ++mapIterator)
    {
        qDebug("{0x%04x, 0x%04x}, //",  mapIterator.key(),  mapIterator.value());
        qDebug() << QString(QChar(mapIterator.key()));
    }
#endif

}

/***************************************************************
*Description:  创建unicode 和gbk码的映射
*Input: 无
*Output: unicode -- gbk 码的 map
*Return: 无
****************************************************************/
void  CharacterStatistics::createUnicodeMatchGbk(QMap<qint32, quint16> & map)
{
    QTextCodec *textCodec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    quint8 areaCode, digitCode;
    quint16 gbkCode = 0;

    map.clear();
    for(areaCode = 0x81; areaCode < 0xFF; areaCode++)
    {
        /*GBK 剔除了0x7F这一列*/
        for(digitCode = 0x40; digitCode < 0x7F; digitCode++)
        {
            char oneWord[2] = {(char)areaCode, (char)digitCode};
            QString str = QString::fromLocal8Bit(oneWord, 2);

            gbkCode = areaCode * 256 + digitCode;
            map.insert(str.at(0).unicode(), gbkCode);

            //qDebug("{0x%04x,0x%02x%02x},", str.at(0).unicode(), digitCode, digitCode);
        }

         /*GBK 剔除了0xFF这一列*/
        for(digitCode = 0x80; digitCode < 0xFF; digitCode++)
        {
            char oneWord[2] = {(char)areaCode, (char)digitCode};
            QString str = QString::fromLocal8Bit(oneWord, 2);

            gbkCode = areaCode * 256 + digitCode;
            map.insert(str.at(0).unicode(), gbkCode);
            //qDebug("{0x%04x,0x%02x%02x},", str.at(0).unicode(), digitCode, digitCode);
        }
    }

    QTextCodec::setCodecForLocale(textCodec);

#if 0
    QMap <qint32 ,quint16>::const_iterator mapIterator;
    for (mapIterator = map.constBegin(); mapIterator != map.constEnd(); ++mapIterator)
    {
        qDebug("{0x%x, 0x%x}, //",  mapIterator.key(),  mapIterator.value());
        qDebug() << QString(QChar(mapIterator.key()));
    }
#endif
}

/***************************************************************
*Description:  在字典总表中找到GB2312中没有的字符
*Input: dicMap 字典总表
*Output: notFoundMap GB2312中没有的字符
*Return: 无
****************************************************************/
void CharacterStatistics::cmpDicWidthGb2312(QMap <qint32, CharInfoList> dicMap, QMap<qint32, CharInfoList> & notFoundMap)
{
    qint32 unicode = 0;
    CharInfoList charInfoList;

    notFoundMap.clear();
    /*遍历字典表中文翻译出现的字符 map*/
    QMap <qint32, CharInfoList>::const_iterator mapIterator;
    for (mapIterator = dicMap.constBegin(); mapIterator != dicMap.constEnd(); ++mapIterator)
    {
        unicode = mapIterator.key();
        /*在GB2312映射表中没有找到这个unicode*/
        if (0 == this->unicodeToGb2312Map.value(unicode))
        {
            /*asiic码，字库中肯定有*/
            if (unicode < 0x7f)
            {
                continue;
            }
            //qDebug () << unicode;
            charInfoList = mapIterator.value();
            notFoundMap.insert(unicode, charInfoList);
        }
    }
}

/***************************************************************
*Description:  在字典总表中找到GBK中没有的字符
*Input: dicMap 字典总表
*Output: notFoundMap GB2312中没有的字符
*Return: 无
****************************************************************/
void CharacterStatistics::cmpDicWidthGbk(QMap<qint32, CharInfoList> dicMap, QMap<qint32, CharInfoList> & notFoundMap)
{
    qint32 unicode = 0;
    CharInfoList charInfoList;

    notFoundMap.clear();
    /*遍历字典表中文翻译出现的字符 map*/
    QMap <qint32, CharInfoList>::const_iterator mapIterator;
    for (mapIterator = dicMap.constBegin(); mapIterator != dicMap.constEnd(); ++mapIterator)
    {
        unicode = mapIterator.key();
        /*在GB2312映射表中没有找到这个unicode*/
        if (0 == this->unicodeToGbkMap.value(unicode))
        {
            /*asiic码，字库中肯定有*/
            if (unicode < 0x7f)
            {
                continue;
            }
            charInfoList = mapIterator.value();
            notFoundMap.insert(unicode, charInfoList);
        }
    }
}

/***************************************************************
*Description: 字典表中中文翻译项，字符的种类数
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
qint32 CharacterStatistics::charTypeCount()
{
    return this->charChineseMap.count();
}

/***************************************************************
*Description:  不匹配字符
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
#define RETURNCHAR "\r\n"
#define SEPARATOR "-----------------------------------------"
#define SHOWSTYLE1 1
QString CharacterStatistics::notFoundStrInfo(QMap<qint32, CharInfoList> notFoundMap)
{
    qint32 unicode;
    CharInfoList charInfoList;
    QString resultString;

    resultString += tr("字符类数目：%1").arg(this->charChineseMap.count());
    resultString += RETURNCHAR;
    resultString += tr("不匹配数目：%1").arg(notFoundMap.count());
    resultString += RETURNCHAR;

#if SHOWSTYLE1
    resultString += tr("字:\tunicode,\t行数\t文本\r\n");
#else
    resultString += SEPARATOR;
    resultString += RETURNCHAR;
    resultString += tr("字符");
    resultString += RETURNCHAR;
    resultString += tr("unicode编码");
    resultString += RETURNCHAR;
    resultString += tr("行号 + 文本");
    resultString += RETURNCHAR;
#endif

    /*遍历字典表中文翻译出现的字符 map*/
    QMap <qint32, CharInfoList>::const_iterator mapIterator;
    for (mapIterator = notFoundMap.constBegin(); mapIterator != notFoundMap.constEnd(); ++mapIterator)
    {

#if SHOWSTYLE1
        unicode = mapIterator.key();
        charInfoList = mapIterator.value();

        /*字符*/
        resultString += QString("%1").arg(QChar(unicode), 1, ' ');
        resultString += ":\t";

        /*unicode编码*/
        resultString += QString("0x%1").arg(unicode, 5, 16, QLatin1Char('0'));//, 0, '0'); //长度最少为5，16进制，小数点后精确0位， 不足5位用0补齐

        resultString += ",\t";
        for (int i = 0; i < charInfoList.count(); ++i)
        {
            if (i > 0)
            {
                resultString += "\r\n\t\t\t\t";
            }
            resultString += QString("%1").arg(charInfoList.at(i).line);
            resultString += "\t";
            resultString += charInfoList.at(i).text;

            if (i > 0 && i == charInfoList.count() - 1)
            {
                //resultString += "\n";
            }
        }
#else
        unicode = mapIterator.key();
        charInfoList = mapIterator.value();

        resultString += RETURNCHAR;
        resultString += SEPARATOR;
        /*字符*/
        resultString += RETURNCHAR;
        resultString += QString("%1").arg(QChar(unicode), 1, ' ');

        /*unicode编码*/
        resultString += RETURNCHAR;
        resultString += QString("0x%1").arg(unicode,0,16);

        for (int i = 0; i < charInfoList.count(); ++i)
        {
            resultString += RETURNCHAR;
            resultString += QString("%1").arg(charInfoList.at(i).line);
            resultString += "\t";
            resultString += charInfoList.at(i).text;
        }
#endif
        resultString += "\r\n";
    }

    return resultString;
}
/***************************************************************
*Description:  字典总表和GB2312表比较结果
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
QString CharacterStatistics::resultCompareWithGb2312()
{
    QMap<qint32, CharInfoList> notFoundMap;

    cmpDicWidthGb2312(this->charChineseMap, notFoundMap);

    return notFoundStrInfo(notFoundMap);
}

/***************************************************************
*Description:  字典总表和GBK表比较结果
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
QString CharacterStatistics::resultCompareWithGbk()
{
    QMap<qint32, CharInfoList> notFoundMap;

    cmpDicWidthGbk(this->charChineseMap, notFoundMap);

    return notFoundStrInfo(notFoundMap);
}
