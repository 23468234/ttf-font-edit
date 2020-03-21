#ifndef TTFMODIFY_H
#define TTFMODIFY_H


#include <QCoreApplication>
#include <QObject>
#include <QMap>
#include <QLinkedList>
#include <QFile>
#include <QStandardItemModel>


#define MAXCHARSETNAME 128

typedef struct unicode_table
{
    quint8 rangeBit;
    char name[MAXCHARSETNAME];
    qint32 rangeStart;
    qint32 rangeEnd;
}UNICODE_TABLE_EX_S;

#if defined (__cplusplus)
 extern "C"
 {
#endif

extern UNICODE_TABLE_EX_S unicodeTable[];

#if defined (__cplusplus)
}
#endif

class TtfFile;
class HdmxTable;
class VmtxTable;
#include "ttftable.h"

class TtfModify
{
public:
    TtfModify();
    ~TtfModify();
    bool ttfFileOpen(QString & fileName);
    void fillListView(QStandardItemModel * standardMode1);
    QMap<quint8, QMap<quint16, bool>> &ttfGetCharsetMap();
    QMap<quint16, quint16> & getUnicodeToGlyfMap(); //<unicode, glyfIndex
    void ttfFileCreate(QString fileName, QByteArray & listViewMask, QMap < quint16, bool> & targetUnicodMap);


private:
    TtfFile * ttfFile;

    QMap<quint16, quint16> unicodeGlyfIndexMap; //<unicode, oldGlyfIndex>
    QMap<quint16, QList<quint16>> glyfToUnicodeMap; //<old glyf, unicode list>

    QMap<quint8, QMap<quint16, bool>> charsetMap; //<unicodeTable index, unicode>
    bool ttfFileCal();
    QMap<quint16, bool> calculateExistUnicode(QByteArray & listViewMask, QMap < quint16, bool> & targetUnicodMap);
    void fileCreate(QString & fileName, QMap<quint16, bool> & newFileUnicodeMap);

    void modifyCompositeGlyf(QByteArray & glyfData, QMap<quint16, qint32> & unicodeAndGlyf, QMap<quint16, quint16> & olDunicodeAndGlyf, QList<quint16> & outMissedGlyf);
    void modifyHdmxTable(HdmxTable * hdmx, HdmxTable * oldHdmx, quint16 oldGlyf);
    void modifyVmtxTable(VmtxTable * vmtx, VmtxTable * oldvmtx, quint16 oldGlyf);
};

#endif
