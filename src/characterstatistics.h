
#ifndef CHARACTERSTATICS_H
#define CHARACTERSTATICS_H

#include <QObject>
#include <QMap>

class CharacterStatistics : public QObject
{
    Q_OBJECT
public:
    CharacterStatistics();
    ~CharacterStatistics();

    /*unicode 映射表*/
    static QMap<qint32, quint16> unicodeToGb2312Map; //<unicode, gb2312>
    static QMap<qint32, quint16> unicodeToGbkMap; // <unicode, gbk>

    /*打开字典表*/
    bool openDicFile(QString fileName);

    /*获取比较结果*/
    QString resultCompareWithGb2312();
    QString resultCompareWithGbk();

    /*获取数目*/
    qint32 charTypeCount();

private:
    typedef struct
    {
        qint32 line;
        QString text;
    }ONE_CHAR_INFO_S;
    typedef QList<ONE_CHAR_INFO_S> CharInfoList;
    QMap<qint32, CharInfoList> charChineseMap; //unicode -- 行数、文本，中文翻译项字符统计map

    /*将一行中文翻译中的字符，添加到map*/
    void parseUtf8StringChinese(qint32 line, QString tranString, QMap<qint32, CharInfoList> & map);

    /*创建unicode映射表*/
    void createUnicodeMatchGb2312(QMap<qint32, quint16> & map);
    void createUnicodeMatchGbk(QMap<qint32, quint16> & map);

    /*找到映射表中不存在的unicode*/
    void cmpDicWidthGb2312(QMap <qint32, CharInfoList> dicMap,  QMap<qint32, CharInfoList> & notFoundList);
    void cmpDicWidthGbk(QMap <qint32, CharInfoList> dicMap,  QMap<qint32, CharInfoList> & notFoundList);

    /*将映射表中不存在的字符信息，转换成QString，用来显示*/
    QString notFoundStrInfo(QMap<qint32, CharInfoList> notFoundMap);
};
#endif
