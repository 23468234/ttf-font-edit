
#include "ttftable.h"
#include <QtMath>
#include <qDebug>
#include <QMessageBox>
/***************************************************************
*Description:  大端数据组成16位数
*Input: line: byte0 最高位字节
*Output: map：
*Return: 组合的32位数
****************************************************************/
quint16 combineByteTo16(quint8 byte0, quint8 byte1)
{
    return (quint16)((byte0 << 8) + byte1);
}

quint32 combineByteTo32(quint8 byte0, quint8 byte1, quint8 byte2, quint8 byte3)
{
    return (quint32)((byte0 << 24) + (byte1 << 16) + (byte2 << 8) + byte3);
}
/***************************************************************
*Description:  小端数据转成大段顺序
*Input: line: byte0 最高位字节
*Output: map：
*Return: 组合的32位数
****************************************************************/
QByteArray splitU16toByteArray(quint32 data)
{
    QByteArray outArray;
    quint8 tmp = 0;
    for (int i = 1; i >= 0 ; --i)
    {
        tmp = (data >> 8*i) & 0xFF;
        outArray.append(tmp);
    }

    return outArray;
}

QByteArray splitU32toByteArray(quint32 data)
{

    QByteArray outArray;
    quint8 tmp = 0;
    for (int i = 3; i >= 0; --i)
    {
        tmp = (data >> 8*i) & 0xFF;
        outArray.append(tmp);
    }

    return outArray;
}

QByteArray splitU64toByteArray(quint64 data)
{

    QByteArray outArray;
    quint8 tmp = 0;
    for (int i = 7; i >= 0; --i)
    {
        tmp = (data >> 8*i) & 0xFF;
        outArray.append(tmp);
    }

    return outArray;
}

/*填充QByteArray为4的整数倍*/
void addUpArrayTo4IntegerTimes(QByteArray & array)
{
    qint32 count = 0;
    count = array.count();
    if (0 != (count % 4))
    {
        array.append(4 - (count % 4),(char)0);
    }
}

QByteArray qlist16ToBigEdianArray(QList<quint16> &list)
{
    QByteArray outArray;
    for (int i = 0; i < list.count(); ++i)
    {
        outArray.append(splitU16toByteArray(list.at(i)));
    }

    return outArray;
}

/***************************************************************
*Description: 计算小于某个数的最大的2的N次方
*Input: line: num---目标数
*Output:
*Return: 无
****************************************************************/
qint32 mySimpleLog(qint32 num1, qint32 num2)
{
    int i = 0;
    while(1)
    {
        if (qPow(num1, i) == num2)
        {
            return i;
        }
        else if(qPow(num1, i) > num2)
        {
            return (i - 1);
        }
        i++;
    }
}

quint32 calcTableChecksum(const QByteArray & data)
{
    quint32 sum = 0L;
    qint32 index = 0;

    for (int i = 0; i < (data.count() / 4); ++i)
    {
        sum += combineByteTo32(data.at(index), data.at(index + 1), data.at(index + 2), data.at(index + 3));
        index += 4;
    }

    return (quint32)sum;
}

/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
bool TTF_FILE_HEAD_S::combineTheByteToStruct(QByteArray data, struct _ttf_head * ttfHeadStruct)
{
    quint32 index = 0;

    ttfHeadStruct->sfntversion = combineByteTo32(data.at(index + 0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;

    ttfHeadStruct->numTables = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

    ttfHeadStruct->searchRange = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

    ttfHeadStruct->entrySelector = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

    ttfHeadStruct->rangeShift = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

 #if 1
    qDebug () << "sfntversion" << ttfHeadStruct->sfntversion
              << "numTables" << ttfHeadStruct->numTables
              << "searchRange" << ttfHeadStruct->searchRange
              << "entrySelector" << ttfHeadStruct->entrySelector
              << "rangeShift" << ttfHeadStruct->rangeShift;
#endif

    quint16 curSearchRange = qNextPowerOfTwo(ttfHeadStruct->numTables) / 2 * 16;
    quint16 curEntrySelector = mySimpleLog(2, qNextPowerOfTwo(ttfHeadStruct->numTables) / 2);
    quint16 curRangeShift = ttfHeadStruct->numTables * 16 - curSearchRange;

    if (curSearchRange != ttfHeadStruct->searchRange ||
        curEntrySelector != ttfHeadStruct->entrySelector ||
        curRangeShift != ttfHeadStruct->rangeShift)
    {
        return false;
    }

    return true;
}

/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
CmapTable::~CmapTable()
{
    for (int i = 0; i < this->encodeList.count(); ++i)
    {
        FORMAT_ENCODING_BASE_S* base = this->encodeList.at(i);
        quint16 format = base->format;

        if (0 == format)
        {

        }
        else if(2 == format)
        {
        }
        else if(4 == format)
        {
            FORMAT_4_ENCODING_S * format4Encoding = (FORMAT_4_ENCODING_S *)base;
            delete format4Encoding->endCount;
            delete format4Encoding->startCount;
            delete format4Encoding->idDelta;
        }
        else if(6 == format)
        {
            FORMAT_6_ENCODING_S * format6Encoding = (FORMAT_6_ENCODING_S*)base;
            delete format6Encoding->glyphIdArray;
        }
        delete base;
    }
    this->encodeList.clear();
}

CmapTable & CmapTable::operator= (CmapTable & table)
{
    //qDebug () << "cmap cpy";
    memcpy(this->tag, table.tag, 4);
    return *this;
}

CmapTable::FORMAT_ENCODING_BASE_S* CmapTable::createEncodeList(QByteArray encodeBase, QByteArray rawData)
{
#define CMPDBG 0
    quint32 index = 0;

#if CMPDBG
    qDebug() << "---------------cmap start-----------";
#endif
    quint16 platformID = combineByteTo16(encodeBase.at(index + 0), encodeBase.at(index + 1));
    index += 2;

    quint16 encodeID = combineByteTo16(encodeBase.at(index + 0), encodeBase.at(index + 1));
    index += 2;

    quint32 offset = combineByteTo32(encodeBase.at(index + 0), encodeBase.at(index+1), encodeBase.at(index+2), encodeBase.at(index+3));
    index += 4;

    /*从cmap总数据中找数据*/
    index = offset;
    quint16 format = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
    index += 2;
#if CMPDBG
    qDebug () <<"paltform id" << platformID << "encode id" << encodeID<<"format" \
              << format << "offset" << offset;//<< "rawData len" << rawData.count();
#endif

    //
    /*format 0*/
    if (0 == format)
    {
        FORMAT_0_ENCODING_S * format0Encoding = new FORMAT_0_ENCODING_S;
        format0Encoding->base.PlatformID = platformID;
        format0Encoding->base.encodingID = encodeID;
        format0Encoding->base.byteOffset = offset;
        format0Encoding->base.format = format;
        format0Encoding->base.length = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug() << "len" <<  format0Encoding->base.length;
#endif
        format0Encoding->base.version = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;

        for (int i = 0; i < 256; ++i)
        {
            format0Encoding->glyphIdArray[i] = rawData.at(index);
            index++;
        }
        return (CmapTable::FORMAT_ENCODING_BASE_S*)format0Encoding;
    }
    else if(2 == format)
    {
        const qint32 startIndex = index - 2; //format 两个字节
        FORMAT_2_ENCODING_S * format2Encoding = new FORMAT_2_ENCODING_S;
        format2Encoding->base.PlatformID = platformID;
        format2Encoding->base.encodingID = encodeID;
        format2Encoding->base.byteOffset = offset;
        format2Encoding->base.format = format;
        format2Encoding->base.length = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug() << "len" <<  format2Encoding->base.length;
#endif
        format2Encoding->base.version = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;

        for (int i = 0; i < 256; ++i)
        {
            format2Encoding->subHeaderKeys[i] = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
            index += 2;
        }

        /*
        UInt16 * 4 subHeaders[variable]//Variable length array of subHeader structures
        UInt16 glyphIndexArray[variable]//Variable length array containing subarrays
        */


        qint32 glyfIndexArrayLen = format2Encoding->base.length - (index - startIndex);
        format2Encoding->subHeaderAndGlyhArray = rawData.mid(index, glyfIndexArrayLen);

    }
    else if(4 == format)
    {
        const qint32 startIndex = index - 2; //format 两个字节
        FORMAT_4_ENCODING_S * format4Encoding = new FORMAT_4_ENCODING_S;
        format4Encoding->base.PlatformID = platformID;
        format4Encoding->base.encodingID = encodeID;
        format4Encoding->base.byteOffset = offset;
        format4Encoding->base.format = format;
        format4Encoding->base.length = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug() << "format 4 len" <<  format4Encoding->base.length;
#endif
        format4Encoding->base.version = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug("base.version:0x%X", format4Encoding->base.version);
#endif

        format4Encoding->segCountX2 = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug("segCountX2:0x%X", format4Encoding->segCountX2);
#endif

        format4Encoding->searchRange = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug("searchRange:0x%X", format4Encoding->searchRange);
#endif

        format4Encoding->entrySelector = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug("entrySelector:0x%X", format4Encoding->entrySelector);
#endif
        format4Encoding->rangeShift = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug("rangeShift:0x%X", format4Encoding->rangeShift);
#endif

        const qint16 segCount = format4Encoding->segCountX2 / 2;
        format4Encoding->endCount = new quint16[segCount];
        for (int i = 0 ;i < segCount; ++i)
        {
            format4Encoding->endCount[i] = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
            index += 2;
#if 0//CMPDBG
        qDebug("endCount:%x", format4Encoding->endCount[i]);
#endif
        }

        format4Encoding->reservedPad = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
#if CMPDBG
        qDebug("reservedPad:0x%X", format4Encoding->reservedPad);
#endif
        index += 2;

        format4Encoding->startCount = new quint16[segCount];
        for (int i = 0 ;i < segCount; ++i)
        {
            format4Encoding->startCount[i] = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
            index += 2;
        }

        format4Encoding->idDelta = new qint16[segCount];
        for (int i = 0 ;i < segCount; ++i)
        {
            format4Encoding->idDelta[i] = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
            index += 2;
        }

        qint32 idROffsetAndGlyphIdLen = format4Encoding->base.length - (index - startIndex);
        format4Encoding->idRangeOffsetAndglyphIdArray = rawData.mid(index, idROffsetAndGlyphIdLen);
        index += idROffsetAndGlyphIdLen;

        return (CmapTable::FORMAT_ENCODING_BASE_S*)format4Encoding;
    }
    else if(6 == format)
    {
        FORMAT_6_ENCODING_S * format6Encoding = new FORMAT_6_ENCODING_S;
        format6Encoding->base.PlatformID = platformID;
        format6Encoding->base.encodingID = encodeID;
        format6Encoding->base.byteOffset = offset;
        format6Encoding->base.format = format;
        format6Encoding->base.length = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;
#if CMPDBG
        qDebug() << "len" <<  format6Encoding->base.length;
#endif
        format6Encoding->base.version = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;


        format6Encoding->firstCode = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;

        format6Encoding->entryCount = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
        index += 2;

        format6Encoding->glyphIdArray = new quint16[format6Encoding->entryCount];
        for (int i = 0; i < format6Encoding->entryCount; ++i)
        {
            format6Encoding->glyphIdArray[i] = combineByteTo16(rawData.at(index + 0), rawData.at(index + 1));
            index += 2;
        }
        return (CmapTable::FORMAT_ENCODING_BASE_S*)format6Encoding;
    }

    return NULL;
}


QMap<quint16, quint16> CmapTable::getUnicodeGlyfIndexMap()
{
    QMap<quint16, quint16> codeToGlyphMap;
    for (int i = 0; i < this->encodeList.count(); ++i)
    {
        FORMAT_ENCODING_BASE_S * base = this->encodeList.at(i);

        if (1 != base->encodingID)
        {
            continue;
        }

        if (0 == base->format)
        {
            QByteArray format0Array;
            FORMAT_0_ENCODING_S * format0Encoding = (FORMAT_0_ENCODING_S *)base;
            //qDebug () << "foramt 0 glyphindex:";
            for (int j = 0; j < 256; ++j)
            {
                //qDebug () << (qint32)format0Encoding->glyphIdArray[j];
            }
            qDebug () << "foramt 0 glyphindex:";
        }
        else if(2 == base->format)
        {
            QByteArray format2Array;
            FORMAT_2_ENCODING_S * format2Encoding = (FORMAT_2_ENCODING_S *)base;

            /*计算subHeaders 的个数*/
            FORMAT_2_ENCODING_S::SUB_HEAD_S *subHeadsArray = (FORMAT_2_ENCODING_S::SUB_HEAD_S *)format2Encoding->subHeaderAndGlyhArray.data();
            int sum = 0;
            for (int j = 0; j < 256; ++j)
            {
                if (format2Encoding->subHeaderKeys[i] / 8 > 0 )
                {
                    sum ++;
                }
            }
            qDebug () << sum;
            quint8 * glyphIndexArray = (quint8*)(subHeadsArray + sum);


            for (int j = 0; j < 256; ++j)
            {
                if (format2Encoding->subHeaderKeys[i] / 8 > 0 )
                {
                    int index = format2Encoding->subHeaderKeys[i] / 8; //subHeaderKeys索引
                    int rem = format2Encoding->subHeaderKeys[i] % 8;
                    FORMAT_2_ENCODING_S::SUB_HEAD_S *curSubHeads = subHeadsArray + index;

                    if (rem >= curSubHeads->firstCode && rem <= curSubHeads->firstCode + curSubHeads->entryCount)
                    {
                        quint16 *subArray = curSubHeads->idRangeOffset + &(curSubHeads->idRangeOffset);

                        quint8 * charIndex = (quint8*)(subArray + rem - curSubHeads->firstCode);

                        quint16 glyphIndex = combineByteTo16(*charIndex, *(charIndex + 1));
                        if (0 == glyphIndex)
                        {
                            codeToGlyphMap.insert(j, 0); //不在范围内，返回0
                        }
                        else
                        {
                            codeToGlyphMap.insert(j, (glyphIndex + curSubHeads->idDelta) % 65536);
                        }
                    }
                    else
                    {
                         codeToGlyphMap.insert(j, 0); //不在范围内，返回0
                    }

                }
                else
                {
                    int index = j*2;
                    codeToGlyphMap.insert(j, combineByteTo16(glyphIndexArray[index], glyphIndexArray[index + 1]));
                }
            }

            return codeToGlyphMap;
        }
        else if(4 == base->format)
        {
            FORMAT_4_ENCODING_S * format4Encoding = (FORMAT_4_ENCODING_S *)base;
            quint16 segCount = format4Encoding->segCountX2 / 2;

            quint8 * offsetArray = (quint8 *)format4Encoding->idRangeOffsetAndglyphIdArray.data();

            for (int i = 0; i < segCount; ++i)
            {
                quint16 startCode = format4Encoding->startCount[i];
                quint16 endCode = format4Encoding->endCount[i];
                quint16 ideDelta = format4Encoding->idDelta[i];
                //qDebug() << startCode << endCode;

                //for (quint16 curCode = startCode; curCode < endCode; curCode++)
                quint16 curCode = startCode;
                while(1)
                {
                    qint32 curOffsetIndex = 2 * i;

                    /*idRangeOffset值*/
                    if (combineByteTo16(offsetArray[curOffsetIndex], offsetArray[curOffsetIndex + 1]) == 0)
                    {
                        codeToGlyphMap.insert(curCode, curCode + ideDelta);
                        //qDebug() << "glyf1" << curCode + ideDelta << ideDelta;
                    }
                    else
                    {
                        quint8* glyphIndex = &offsetArray[curOffsetIndex] + \
                                                combineByteTo16(offsetArray[curOffsetIndex], offsetArray[curOffsetIndex+1]) +\
                                                (curCode - startCode) * 2;

                        codeToGlyphMap.insert(curCode, combineByteTo16(*glyphIndex, *(glyphIndex + 1)));
                        //qDebug() << "glyf2" << combineByteTo16(*glyphIndex, *(glyphIndex + 1));
                    }

                    if (curCode >= endCode)
                    {
                        break;
                    }
                    curCode ++;
                }
            }
            return codeToGlyphMap;
        }
        else if(6 == base->format)
        {
            FORMAT_6_ENCODING_S * format6Encoding = (FORMAT_6_ENCODING_S *)base;
            quint16 startCode = format6Encoding->firstCode;
            quint16 endCode = format6Encoding->firstCode + format6Encoding->entryCount;
            for (quint16 curCode = startCode; curCode < endCode ; ++curCode)
            {
                codeToGlyphMap.insert(curCode, format6Encoding->glyphIdArray[curCode - startCode]);
            }

            return codeToGlyphMap;
        }
        else
        {
            qDebug() << "Dengerous bug" << __FUNCTION__;
        }
    }

    return codeToGlyphMap;
}



void CmapTable::combineBytesToStruct(QByteArray & data)
{
    quint32 index = 0;
    this->tableVersionNumber = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

    this->numberOfEncodingTables = combineByteTo16(data.at(index + 0), data.at(index + 1));
    index += 2;

    //qDebug() << "tableVersionNumber" << tableVersionNumber;
    for (int num = 0; num < numberOfEncodingTables; num++)
    {
        QByteArray byteBuf;
        for (int i = 0; i < 8; ++i)
        {
            byteBuf.append(data.at(index));

            index++;
        }
        encodeList.append(createEncodeList(byteBuf, data));
    }
} //camp


void CmapTable::createFormat4AndAddToList(QMap<quint16, qint32> & unicodeAndGlyf)
{
    if (unicodeAndGlyf.isEmpty())
    {
        return;
    }

    FORMAT_4_ENCODING_S * format4Encoding = new FORMAT_4_ENCODING_S;

    format4Encoding->base.format = 4; //format4
    format4Encoding->base.PlatformID = 3; //pc
    format4Encoding->base.encodingID = 1; //unicode
    format4Encoding->base.version = 0; //version


    quint16 previousCode = 0xFFFF;
    quint16 startCode = unicodeAndGlyf.begin().key();
    quint16 endCode = 0;


    QList<quint16> startCodeArray;
    QList<quint16> endCodeArray;
    QList<qint32> ideltaArray;
    QList<quint16> offsetsArray;
    QList<quint16> glyphIndexArray;

/*计算startCode 和 endCode*/
    QMap<quint16, qint32>::const_iterator iteratorGlyf;
    for (iteratorGlyf = unicodeAndGlyf.begin(); iteratorGlyf != unicodeAndGlyf.end(); ++iteratorGlyf)
    {
        quint16 curCode = iteratorGlyf.key();
        if (0xFFFF != previousCode && curCode != previousCode + 1)
        {
            /*创建段*/
            endCode = previousCode;

            //to..
            startCodeArray.append(startCode);
            endCodeArray.append(endCode);

            /*记录新一段的起始*/
            startCode = curCode;
        }

        previousCode = iteratorGlyf.key();
    }

    /*最后一段肯定是没有添加的*/
    endCode = previousCode;
    startCodeArray.append(startCode);
    endCodeArray.append(endCode);

    /*结束用 0xFFFF填充*/
    startCodeArray.append(0xFFFF);
    endCodeArray.append(0xFFFF);
#if 0
    for (int i = 0; i < startCodeArray.count(); ++i)
    {
        qDebug () << "start code" << startCodeArray.at(i) << "end code" << endCodeArray.at(i);
    }
#endif
/*计算iDelta*/
    for (int i = 0; i < startCodeArray.count() - 1; ++i)
    {
        qint32 idelta = 0;

        idelta = unicodeAndGlyf.value(startCodeArray.at(i)) - startCodeArray.at(i); //glyphIndex - startCode
        //qDebug () << "idelta" << idelta;
        ideltaArray.append(idelta);
    }

    /*结束 idelta 为1, 指向索引0*/
    ideltaArray.append(1);
#if 0
    for (int i = 0; i < ideltaArray.count(); ++i)
    {
        qDebug ()<< "index"<< unicodeAndGlyf.value(startCodeArray.at(i)) << "start code" << startCodeArray.at(i)<< "idelta" << ideltaArray.at(i);
    }
#endif
/*计算offsets, 不用考虑最后一个 0xFFFF的值*/
    for (int i = 0; i < startCodeArray.count() - 1; ++i)
    {
        /*idelta 计算出的索引不匹配，就要用到offsets*/
        startCode = startCodeArray.at(i);
        endCode = endCodeArray.at(i);
        quint16 curCode = startCode;

        offsetsArray.append(0);

        /*处理一段编码*/
        while(1)
        {
            /*不是按顺序的，或者qint16 无法表示的。 一段中有一个不能用idelta表示，那么整个段要改为offset形式*/
            if (curCode + ideltaArray.at(i) != unicodeAndGlyf.value(curCode) || ideltaArray.at(i) < (-0x7FFF))
            {
                /*偏移地址等于， offset 剩下的长度，加上glyphIndexArray 当前长度， 因为新的偏移量是加在glyphIndexArray后面*/
                quint16 offsetsArrayCount = startCodeArray.count(); // offsetsArray 的最终长度应该和startCodeArray 一样，都是segment
                quint16 addrOffset = (glyphIndexArray.count() + offsetsArrayCount - i) * 2;
                ideltaArray.replace(i,0);

                /*整段的 index 都要加入到后面的 glyphIndexArray*/
                offsetsArray.replace(i, addrOffset);
                {
                    startCode = startCodeArray.at(i);
                    endCode = endCodeArray.at(i);
                    while(1)
                    {
                        glyphIndexArray.append(unicodeAndGlyf.value(curCode));
                        if (curCode >= endCode)
                        {
                            break;
                        }
                        curCode ++;
                    }
                }

                //qDebug() << "offset " << addrOffset << "index" << unicodeAndGlyf.value(curCode);
                break; //这一段都放到了glyphIndexArray
            }
            else
            {
                /*相等的话offset置0*/
                offsetsArray.replace(i,0);
            }

            if (curCode >= endCode)
            {
                break;
            }
            curCode ++;
        }
    }
    /*最后的 0xffff*/
    offsetsArray.append(0);

/*拷贝数据到结构体*/
    format4Encoding->segCountX2 = endCodeArray.count() * 2;
    format4Encoding->searchRange = qNextPowerOfTwo(format4Encoding->segCountX2 / 2) / 2; //2 x (2**floor(log2(segCount)))
    format4Encoding->entrySelector = mySimpleLog(2, format4Encoding->segCountX2 / 2); //log2(searchRange/2)
    format4Encoding->rangeShift = format4Encoding->segCountX2 - format4Encoding->searchRange; //2 x segCount - searchRange

#if 0
    qDebug () << "segCountX2" <<  format4Encoding->segCountX2
              << "searchRange" <<  format4Encoding->searchRange
              << "entrySelector" <<  format4Encoding->entrySelector
              << "rangeShift" <<  format4Encoding->rangeShift ;

#endif
    format4Encoding->endCount = new quint16[format4Encoding->segCountX2 / 2];
    for (int i = 0; i < endCodeArray.count(); ++i)
    {
        format4Encoding->endCount[i] = endCodeArray.at(i);
    }

    format4Encoding->startCount = new quint16[format4Encoding->segCountX2 / 2];
    for (int i = 0; i < endCodeArray.count(); ++i)
    {
        format4Encoding->startCount[i] = startCodeArray.at(i);
    }

    format4Encoding->idDelta = new qint16[format4Encoding->segCountX2 / 2];
    for (int i = 0; i < endCodeArray.count(); ++i)
    {
        format4Encoding->idDelta[i] = ideltaArray.at(i);
        //qDebug () << "delta" << ideltaArray.at(i);
    }

    format4Encoding->idRangeOffsetAndglyphIdArray = qlist16ToBigEdianArray(offsetsArray);
    format4Encoding->idRangeOffsetAndglyphIdArray.append(qlist16ToBigEdianArray(glyphIndexArray));

    format4Encoding->base.length = 2 * 8 //8个quint16 的
                                    + format4Encoding->segCountX2 * 4 //startCount、 endCount 、idDelta、idRangeOffset, quint16型
                                    + format4Encoding->idRangeOffsetAndglyphIdArray.count();

    this->encodeList.append((FORMAT_ENCODING_BASE_S* )format4Encoding);

    this->numberOfEncodingTables = this->encodeList.count();
    //qDebug () << " this->numberOfEncodingTables" << this->numberOfEncodingTables;
}

QByteArray CmapTable::getDataForWriteFile()
{
    QByteArray outArray;
    QList<qint32> offsetList; //用于记录各个字符集表的数据长度

    outArray.append(splitU16toByteArray(this->tableVersionNumber));
    outArray.append(splitU16toByteArray(this->numberOfEncodingTables));

    //encode table
    for (int i = 0; i < this->encodeList.count(); ++i)
    {
        FORMAT_ENCODING_BASE_S * base = this->encodeList.at(i);
        outArray.append(splitU16toByteArray(base->PlatformID));
        outArray.append(splitU16toByteArray(base->encodingID));
        outArray.append(splitU32toByteArray(base->byteOffset)); //占个位置先
    }

    //encode table
    for (int i = 0; i < this->encodeList.count(); ++i)
    {
        FORMAT_ENCODING_BASE_S * base = this->encodeList.at(i);
        if (0 == base->format)
        {
            QByteArray format0Array;
            FORMAT_0_ENCODING_S * format0Encoding = (FORMAT_0_ENCODING_S *)base;
            format0Array.append(splitU16toByteArray(format0Encoding->base.format));
            format0Array.append(splitU16toByteArray(format0Encoding->base.length));
            format0Array.append(splitU16toByteArray(format0Encoding->base.version));

            format0Array.append((const char*)format0Encoding->glyphIdArray, 256);
            //qDebug() << format0Array.count();
            outArray.append(format0Array);
            offsetList.append(format0Array.count());
        }
        else if(2 == base->format)
        {
            QByteArray format2Array;
            FORMAT_2_ENCODING_S * format2Encoding = (FORMAT_2_ENCODING_S *)base;
            format2Array.append(splitU16toByteArray(format2Encoding->base.format));
            format2Array.append(splitU16toByteArray(format2Encoding->base.length));
            format2Array.append(splitU16toByteArray(format2Encoding->base.version));

            format2Array.append((const char*)format2Encoding->subHeaderKeys, 256);
            format2Array.append(format2Encoding->subHeaderAndGlyhArray);

            outArray.append(format2Array);
            offsetList.append(format2Array.count());
            //qDebug() << format2Array.count();

        }
        else if(4 == base->format)
        {
            QByteArray format4Array;
            FORMAT_4_ENCODING_S * format4Encoding = (FORMAT_4_ENCODING_S *)base;
            format4Array.append(splitU16toByteArray(format4Encoding->base.format));
            format4Array.append(splitU16toByteArray(format4Encoding->base.length));
            format4Array.append(splitU16toByteArray(format4Encoding->base.version));

            format4Array.append(splitU16toByteArray(format4Encoding->segCountX2));
            format4Array.append(splitU16toByteArray(format4Encoding->searchRange));
            format4Array.append(splitU16toByteArray(format4Encoding->entrySelector));
            format4Array.append(splitU16toByteArray(format4Encoding->rangeShift));

            for (int i = 0; i < format4Encoding->segCountX2 / 2; ++i)
            {
                format4Array.append(splitU16toByteArray(format4Encoding->endCount[i]));
                //qDebug() << "end code" << format4Encoding->endCount[i];
            }

            format4Array.append(splitU16toByteArray(format4Encoding->reservedPad));

            for (int i = 0; i < format4Encoding->segCountX2 / 2; ++i)
            {
                format4Array.append(splitU16toByteArray(format4Encoding->startCount[i]));
                //qDebug() << "startCount code" << format4Encoding->startCount[i];
            }

            for (int i = 0; i < format4Encoding->segCountX2 / 2; ++i)
            {
                format4Array.append(splitU16toByteArray(format4Encoding->idDelta[i]));
                //qDebug() << "idDelta code" << format4Encoding->idDelta[i];
            }
            format4Array.append(format4Encoding->idRangeOffsetAndglyphIdArray);

            outArray.append(format4Array);
            offsetList.append(format4Array.count());
            //qDebug() << format4Array.count();

        }
        else if(6 == base->format)
        {
            QByteArray format6Array;
            FORMAT_6_ENCODING_S * format6Encoding = (FORMAT_6_ENCODING_S *)base;
            format6Array.append(splitU16toByteArray(format6Encoding->base.format));
            format6Array.append(splitU16toByteArray(format6Encoding->base.length));
            format6Array.append(splitU16toByteArray(format6Encoding->base.version));

            format6Array.append(splitU16toByteArray(format6Encoding->firstCode));
            format6Array.append(splitU16toByteArray(format6Encoding->entryCount));
            format6Array.append((const char*)format6Encoding->glyphIdArray, format6Encoding->entryCount*2);

            outArray.append(format6Array);
            offsetList.append(format6Array.count());
            //qDebug() << format6Array.count();
        }
        else
        {
            qDebug() << "Dengerous bug" << __FUNCTION__;
        }
    }

    /*填充offsets*/
    int dataOffset =  4 + 8*this->encodeList.count(); //前面的头和格式编码表
    for (int i = 0; i < this->encodeList.count(); ++i)
    {
        int pos = 4 + i*8 + 4; //编码表的offset位置，前四个字节是cmap数据头，一个编码表索引八个字节
        outArray.replace(pos, 4, splitU32toByteArray(dataOffset));
        dataOffset += offsetList.at(i);
    }

    //
    return outArray;
}
void CmapTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
GlyfTable::~GlyfTable()
{

}

GlyfTable & GlyfTable::operator= (GlyfTable & table)
{
    memcpy(this->tag, table.tag, 4);
    return *this;
}
void GlyfTable::combineBytesToStruct(QByteArray & data)
{
    this->glyfData = data;
} //lgyf

QByteArray GlyfTable::getDataForWriteFile()
{
    return this->glyfData;
}

void GlyfTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}


quint32 GlyfTable::addGlyf(QByteArray & data)
{
    this->glyfData.append(data);

    return this->glyfData.count();
}

QByteArray GlyfTable::getOneGlyfData(quint32 startAddr, quint32 endAddr)
{
    quint32 len = endAddr - startAddr;

    return (this->glyfData.mid(startAddr, len));
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
HeadTable::~HeadTable()
{

}

HeadTable & HeadTable::operator= (HeadTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->tableVersionNumber = table.tableVersionNumber;  // 0x00010000 for version 1.0.
    this->fontRevision = table.fontRevision;       // Set by font manufacturer.
    this->checkSumAdjustment = table.checkSumAdjustment; //To compute: set it to 0, sum the entire font
                                //as ULONG, then store 0xB1B0AFBA - sum.
    this->magicNumber = table.magicNumber; //Set to 0x5F0F3CF5
    this->flags = table.flags; //Bit 0 - baseline for font at y=0;
                    //Bit 1 - left sidebearing at x=0;
                    //Bit 2 - instructions may depend on point size;
                    //Bit 3 - force ppem to integer values for all
                    //internal scaler math; may use fractional ppem
                    //sizes if this bit is clear;
                    //Bit 4 - instructions may alter advance width
                    //(the advance widths might not scale linearly);
                    //Note: All other bits must be zero.
    this->unitsPerEm = table.unitsPerEm; //Valid range is from 16 to 16384
    this->createdInternationalDate = table.createdInternationalDate;    //(8-byte field).
    this->modifiedInternationalDate = table.modifiedInternationalDate;   //(8-byte field).
    this->xMin = table.xMin; // For all glyph bounding boxes.
    this->yMin = table.yMin; // For all glyph bounding boxes.
    this->xMax = table.xMax; // For all glyph bounding boxes.
    this->yMax = table.yMax; // For all glyph bounding boxes.
    this->macStyle = table.macStyle;   // Bit 0 bold (if set to 1); Bit 1 italic (if set to 1)
                        //Bits 2-15 reserved (set to 0).
    this->lowestRecPPEM = table.lowestRecPPEM;      //Smallest readable size in pixels.
    this->fontDirectionHint = table.fontDirectionHint;   //0 Fully mixed directional glyphs;
                                //1 Only strongly left to right;
                                //2 Like 1 but also contains neutrals1;
                                //-1 Only strongly right to left;
                                //-2 Like -1 but also contains neutrals.
    this->indexToLocFormat = table.indexToLocFormat;    // 0 for short offsets, 1 for long.
    this->glyphDataFormat = table.glyphDataFormat;     // 0 for current format.


    return *this;
}

void HeadTable::combineBytesToStruct(QByteArray & data)
{
    #define HEADDEBUG 0
    qint32 index = 0;

    this->tableVersionNumber = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("tableVersionNumber : 0x%X", this->tableVersionNumber);
#endif

    this->fontRevision = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("fontRevision : 0x%X", this->fontRevision);
#endif

    this->checkSumAdjustment = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("checkSumAdjustment : 0x%X", this->checkSumAdjustment);
#endif

    this->magicNumber = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("magicNumber : 0x%X", this->magicNumber);
#endif

    this->flags = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("flag : 0x%X", this->flags);
#endif

    this->unitsPerEm = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("unitsPerEm : 0x%X", this->unitsPerEm);
#endif

    this->createdInternationalDate = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
    this->createdInternationalDate = (this->createdInternationalDate << 32) + \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("createdInternationalDate : 0x%X", this->createdInternationalDate);
#endif

    this->modifiedInternationalDate = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
    this->modifiedInternationalDate = (this->modifiedInternationalDate << 32) + \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HEADDEBUG
    qDebug("modifiedInternationalDate : 0x%X", this->modifiedInternationalDate);
#endif

    this->xMin = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("xMin : %d", this->xMin);
#endif

    this->yMin = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("yMin : %d", this->yMin);
#endif
    this->xMax = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("xMax : %d", this->xMax);
#endif

    this->yMax = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("yMax : %d", this->yMax);
#endif

    this->macStyle = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("macStyle : 0x%X", this->macStyle);
#endif

    this->lowestRecPPEM = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("lowestRecPPEM : 0x%X", this->lowestRecPPEM);
#endif

    this->fontDirectionHint = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("fontDirectionHint : 0x%X", this->fontDirectionHint);
#endif

    this->indexToLocFormat = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("indexToLocFormat : 0x%X", this->indexToLocFormat);
#endif

    this->glyphDataFormat = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HEADDEBUG
    qDebug("glyphDataFormat : 0x%X", this->glyphDataFormat);
#endif
} //haed

QByteArray HeadTable::getDataForWriteFile()
{
    QByteArray outArray;
    outArray.append(splitU32toByteArray(this->tableVersionNumber));
    outArray.append(splitU32toByteArray(this->fontRevision));
    outArray.append(splitU32toByteArray(this->checkSumAdjustment));
    outArray.append(splitU32toByteArray(this->magicNumber));
    outArray.append(splitU16toByteArray(this->flags));
    outArray.append(splitU16toByteArray(this->unitsPerEm));
    outArray.append(splitU64toByteArray(this->createdInternationalDate));
    outArray.append(splitU64toByteArray(this->modifiedInternationalDate));

    outArray.append(splitU16toByteArray(this->xMin));
    outArray.append(splitU16toByteArray(this->yMin));
    outArray.append(splitU16toByteArray(this->xMax));
    outArray.append(splitU16toByteArray(this->yMax));
    outArray.append(splitU16toByteArray(this->macStyle));
    outArray.append(splitU16toByteArray(this->lowestRecPPEM));
    outArray.append(splitU16toByteArray(this->fontDirectionHint));
    outArray.append(splitU16toByteArray(this->indexToLocFormat));
    outArray.append(splitU16toByteArray(this->glyphDataFormat));

    return outArray;
}

void HeadTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
HheaTable::~HheaTable()
{
    //tableEntry = tableEntry;
}

HheaTable & HheaTable::operator= (HheaTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->tableVersionNumber = table.tableVersionNumber;  // 0x00010000 for version 1.0.
    this->Ascender = table.Ascender;    //Typographic ascent.
    this->Descender = table.Descender;   //Typographic descent.
    this->LineGap = table.LineGap;     //Typographic line gap. Negative
                        //LineGap values are treated as zero
                        //in Windows 3.1, System 6, and System 7.
    this->advanceWidthMax = table.advanceWidthMax;    //Maximum advance width value in ‘hmtx’ table.
    this->minLeftSideBearing = table.minLeftSideBearing;  //Minimum left sidebearing value in ‘hmtx’ table.
    this->minRightSideBearing = table.minRightSideBearing; //Minimum right sidebearing value;
                                //calculated as Min(aw - lsb - (xMax - xMin)).
    this->xMaxExtent = table.xMaxExtent;      //Max(lsb + (xMax - xMin)).
    this->caretSlopeRise = table.caretSlopeRise;  //Used to calculate the slope of the cursor (rise/run); 1 for vertical.
    this->caretSlopeRun = table.caretSlopeRun;   //0 for vertical.
    this->reserved1 = table.reserved1; //set to 0
    this->reserved2 = table.reserved2; //set to 0
    this->reserved3 = table.reserved3; //set to 0
    this->reserved4 = table.reserved4; //set to 0
    this->reserved5 = table.reserved5; //set to 0
    this->metricDataFormat = table.metricDataFormat;    //0 for current format.
    this->numberOfHMetrics = table.numberOfHMetrics;   //Number of hMetric entries in
                                //‘hmtx’ table; may be smaller than the
                                //total number of glyphs in the font.
    return *this;
}

void HheaTable::combineBytesToStruct(QByteArray & data)
{
    #define HHEADOUT 0
    qint32 index = 0;
    //qDebug () << data.count();

    this->tableVersionNumber = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if HHEADOUT
    qDebug("tableVersionNumber : 0x%X", this->tableVersionNumber);
#endif

    this->Ascender = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("Ascender : 0x%X", this->Ascender);
#endif

    this->Descender = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("Descender : 0x%X", this->Descender);
#endif

    this->LineGap = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("LineGap : 0x%X", this->LineGap);
#endif

    this->advanceWidthMax = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("advanceWidthMax : 0x%X", this->advanceWidthMax);
#endif

    this->minLeftSideBearing = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("minLeftSideBearing : 0x%X", this->minLeftSideBearing);
#endif

    this->minRightSideBearing = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("minRightSideBearing : 0x%X", this->minRightSideBearing);
#endif

    this->xMaxExtent = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("xMaxExtent : 0x%X", this->xMaxExtent);
#endif

    this->caretSlopeRise = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("caretSlopeRise : 0x%X", this->caretSlopeRise);
#endif

    this->caretSlopeRun = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("caretSlopeRun : 0x%X", this->caretSlopeRun);
#endif

    this->reserved1 = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("reserved1 : 0x%X", this->reserved1);
#endif

    this->reserved2 = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("reserved2 : 0x%X", this->reserved2);
#endif

    this->reserved3 = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("reserved3 : 0x%X", this->reserved3);
#endif

    this->reserved4 = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("reserved4 : 0x%X", this->reserved4);
#endif

    this->reserved5 = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("reserved5 : 0x%X", this->reserved5);
#endif

    this->metricDataFormat = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("metricDataFormat : 0x%X", this->metricDataFormat);
#endif

    this->numberOfHMetrics = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if HHEADOUT
    qDebug("numberOfHMetrics : 0x%X", this->numberOfHMetrics);
#endif

    //qDebug () << index;
} //hhea

QByteArray HheaTable::getDataForWriteFile()
{

    QByteArray outArray;
    outArray.append(splitU32toByteArray(this->tableVersionNumber));
    outArray.append(splitU16toByteArray(this->Ascender));
    outArray.append(splitU16toByteArray(this->Descender));
    outArray.append(splitU16toByteArray(this->LineGap));
    outArray.append(splitU16toByteArray(this->advanceWidthMax));
    outArray.append(splitU16toByteArray(this->minLeftSideBearing));
    outArray.append(splitU16toByteArray(this->minRightSideBearing));
    outArray.append(splitU16toByteArray(this->xMaxExtent));
    outArray.append(splitU16toByteArray(this->caretSlopeRise));
    outArray.append(splitU16toByteArray(this->caretSlopeRun));
    outArray.append(splitU16toByteArray(this->reserved1));
    outArray.append(splitU16toByteArray(this->reserved2));
    outArray.append(splitU16toByteArray(this->reserved3));
    outArray.append(splitU16toByteArray(this->reserved4));
    outArray.append(splitU16toByteArray(this->reserved5));
    outArray.append(splitU16toByteArray(this->metricDataFormat));
    outArray.append(splitU16toByteArray(this->numberOfHMetrics));

    return outArray;
}
void HheaTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
HmtxTable::~HmtxTable()
{
    for (int i = 0 ; i < metricList.count(); ++i)
    {
        delete metricList.at(i);
    }
    metricList.clear();
}

HmtxTable & HmtxTable::operator= (HmtxTable & table)
{
    memcpy(this->tag, table.tag, 4);
    /*清除*/
    for (int i = 0 ; i < metricList.count(); ++i)
    {
        delete metricList.at(i);
    }
    metricList.clear();

    /*复制*/
    for (int i = 0 ; i < table.metricList.count(); ++i)
    {
        LONG_HOR_MERTRIC_S *newMetic = new LONG_HOR_MERTRIC_S;
        memcpy(newMetic, table.metricList.at(i), sizeof(LONG_HOR_MERTRIC_S));
        this->metricList.append(newMetic);
    }
    return *this;
}

void HmtxTable::combineBytesToStruct(QByteArray & data)
{
    #define HMTXOUT 0
    qint32 index = 0;

    for (int i = 0; i < (data.count() / 4); ++i)
    {
        LONG_HOR_MERTRIC_S * newMetric = new LONG_HOR_MERTRIC_S;

        newMetric->advanceWidth = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;
        #if HMTXOUT
            qDebug("advanceWidth : 0x%X", newMetric->advanceWidth);
        #endif

        newMetric->lsb = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;
        #if HMTXOUT
            qDebug("lsb : 0x%X", newMetric->lsb);
        #endif

        this->metricList.append(newMetric);
    }

} //hmtx

QByteArray HmtxTable::getDataForWriteFile()
{
    QByteArray outArray;

    for (int i = 0; i < this->metricList.count(); ++i)
    {
        LONG_HOR_MERTRIC_S * metric = this->metricList.at(i);
        outArray.append(splitU16toByteArray(metric->advanceWidth));
        outArray.append(splitU16toByteArray(metric->lsb));
    }

    return outArray;
}

void HmtxTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}

HmtxTable::LONG_HOR_MERTRIC_S HmtxTable::getOneMertric(quint16 glyfIndex)
{
    return *(this->metricList.at(glyfIndex));
}

void HmtxTable::addOneMertric(LONG_HOR_MERTRIC_S mertric)
{
    LONG_HOR_MERTRIC_S * newMetric = new LONG_HOR_MERTRIC_S;

    newMetric->advanceWidth = mertric.advanceWidth;
    newMetric->lsb = mertric.lsb;

    this->metricList.append(newMetric);

}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
LocaTable::~LocaTable()
{

}

LocaTable & LocaTable::operator= (LocaTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->glyfOffsets = table.glyfOffsets;
    return *this;
}

void LocaTable::combineBytesToStruct(QByteArray & data)
{
    this->glyfOffsets = data;
}

QByteArray LocaTable::getDataForWriteFile()
{
    return this->glyfOffsets;
}
void LocaTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}

quint32 LocaTable::getGlyfOffset(quint8 indexToLocFormat, quint16 glyphIndex)
{
     //0 for short offsets, 1 for long.
    quint8 * locaData = (quint8 *)this->glyfOffsets.data();
    if (0 == indexToLocFormat)
    {
        qint32 locaIndex = glyphIndex * 2;
        return combineByteTo16(locaData[locaIndex], locaData[locaIndex + 1]);
    }
    else if(1 == indexToLocFormat)
    {
        qint32 locaIndex = glyphIndex * 4;

        return combineByteTo32(locaData[locaIndex], locaData[locaIndex + 1], \
                            locaData[locaIndex + 2], locaData[locaIndex + 3]);
    }
    else
    {
        qDebug() <<"呵呵"<< __FUNCTION__;
    }




    return 0;

}

void LocaTable::addGlyfOffset(quint8 indexToLocFormat, quint32 offset)
{

    if (0 == indexToLocFormat)
    {
        this->glyfOffsets.append(splitU16toByteArray(offset));
    }
    else
    {
        this->glyfOffsets.append(splitU32toByteArray(offset));
    }

}

void LocaTable::debugInfo()
{
    //quint8 * abc = (quint8*)this->glyfOffsets.data();
    qDebug () <<  __FUNCTION__ <<this->glyfOffsets.count();
    for (int index = 0; index < this->glyfOffsets.count(); ++index)
    {
        //qDebug () << combineByteTo32(abc[index * 4], abc[index* 4+1], abc[index* 4+2], abc[index* 4+3]);
        index++;
    }
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/

MaxpTable::~MaxpTable()
{

}

MaxpTable & MaxpTable::operator= (MaxpTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->tableVersionNumber = table.tableVersionNumber;// 0x00010000 for version 1.0.
    this->numGlyphs = table.numGlyphs;      // The number of glyphs in the font.
    this->maxPoints = table.maxPoints;      // Maximum points in a non-composite glyph.
    this->maxContours = table.maxContours;    // Maximum contours in a noncompositeglyph.
    this->maxCompositePoints = table.maxCompositePoints;     // Maximum points in a composite glyph.
    this->maxCompositeContours = table.maxCompositeContours;   // Maximum contours in a composite glyph.
    this->maxZones = table.maxZones;       // 1 if instructions do not use the twilight zone (Z0),
                            //or 2 if instructions do use Z0; should be set to 2 in most cases.
    this->maxTwilightPoints = table.maxTwilightPoints;  //Maximum points used in Z0.
    this->maxStorage = table.maxStorage;         //Number of Storage Area locations.
    this->maxFunctionDefs = table.maxFunctionDefs;    //Number of FDEFs.
    this->maxInstructionDefs = table.maxInstructionDefs; //Number of IDEFs.
    this->maxStackElements = table.maxStackElements;   //Maximum stack depth2.
    this->maxSizeOfInstructions = table.maxSizeOfInstructions;  //Maximum byte count for glyph instructions.
    this->maxComponentElements = table.maxComponentElements;   //Maximum number of components referenced at “top level”
                                    //for any composite glyph.
    this->maxComponentDepth = table.maxComponentDepth; //Maximum levels of recursion;

    return *this;
}
void MaxpTable::combineBytesToStruct(QByteArray & data)
{
    #define MAXPOUT 0
    qint32 index = 0;

    this->tableVersionNumber = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if MAXPOUT
    qDebug("tableVersionNumber : 0x%X", this->tableVersionNumber);
#endif

    this->numGlyphs = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("numGlyphs : 0x%X", this->numGlyphs);
#endif

    this->maxPoints = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxPoints : 0x%X", this->maxPoints);
#endif

    this->maxContours = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxContours : 0x%X", this->maxContours);
#endif

    this->maxCompositePoints = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxCompositePoints : 0x%X", this->maxCompositePoints);
#endif

    this->maxCompositeContours = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxCompositeContours : 0x%X", this->maxCompositeContours);
#endif

    this->maxZones = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxZones : 0x%X", this->maxZones);
#endif

    this->maxTwilightPoints = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxTwilightPoints : 0x%X", this->maxTwilightPoints);
#endif

    this->maxStorage = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxStorage : 0x%X", this->maxStorage);
#endif

    this->maxFunctionDefs = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxFunctionDefs : 0x%X", this->maxFunctionDefs);
#endif

    this->maxInstructionDefs = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxInstructionDefs : 0x%X", this->maxInstructionDefs);
#endif

    this->maxStackElements = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxStackElements : 0x%X", this->maxStackElements);
#endif

    this->maxSizeOfInstructions = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxSizeOfInstructions : 0x%X", this->maxSizeOfInstructions);
#endif

    this->maxComponentElements = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxComponentElements : 0x%X", this->maxComponentElements);
#endif

    this->maxComponentDepth = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if MAXPOUT
    qDebug("maxComponentDepth : 0x%X", this->maxComponentDepth);
#endif

}//maxp

QByteArray MaxpTable::getDataForWriteFile()
{
    QByteArray outArray;

    outArray.append(splitU32toByteArray(this->tableVersionNumber));
    outArray.append(splitU16toByteArray(this->numGlyphs));
    outArray.append(splitU16toByteArray(this->maxPoints));
    outArray.append(splitU16toByteArray(this->maxContours));
    outArray.append(splitU16toByteArray(this->maxCompositePoints));
    outArray.append(splitU16toByteArray(this->maxCompositeContours));
    outArray.append(splitU16toByteArray(this->maxZones));
    outArray.append(splitU16toByteArray(this->maxTwilightPoints));
    outArray.append(splitU16toByteArray(this->maxStorage));
    outArray.append(splitU16toByteArray(this->maxFunctionDefs));
    outArray.append(splitU16toByteArray(this->maxInstructionDefs));
    outArray.append(splitU16toByteArray(this->maxStackElements));
    outArray.append(splitU16toByteArray(this->maxSizeOfInstructions));
    outArray.append(splitU16toByteArray(this->maxComponentElements));
    outArray.append(splitU16toByteArray(this->maxComponentDepth));

    return outArray;
}


void MaxpTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
NameTable::~NameTable()
{

}

NameTable & NameTable::operator= (NameTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->nameArray = table.nameArray;
    return *this;
}

void NameTable::combineBytesToStruct(QByteArray & data)
{
    this->nameArray = data;
} //name

QByteArray NameTable::getDataForWriteFile()
{
    return this->nameArray;
}

void NameTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/

PostTable::~PostTable()
{
}

PostTable & PostTable::operator= (PostTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->Format = table.Format;  //Type 0x00010000 for format 1.0, 0x00020000 for format
                    //2.0, and so on...
    this->italicAngle = table.italicAngle; //Italic angle in counter-clockwise degrees from the
                        //vertical. Zero for upright text, negative for text that
                        //leans to the right (forward)
    this->underlinePosition = table.underlinePosition;   //Suggested values for the underline position
                                //(negative values indicate below baseline).
    this->underlineThickness = table.underlineThickness;  //Suggested values for the underline thickness.
    this->isFixedPitch = table.isFixedPitch;   //Set to 0 if the font is proportionally spaced, nonzero
                            //if the font is not proportionally spaced (i.e.monospaced).
    this->minMemType42 = table.minMemType42;   //Minimum memory usage when a TrueType font is downloaded.
    this->maxMemType42 = table.maxMemType42;   //Maximum memory usage when a TrueType font is downloaded.
    this->minMemType1 = table.minMemType1;    //Minimum memory usage when a TrueType font is
                            //downloaded as a Type 1 font.
    this->maxMemType1 = table.maxMemType1;    //Maximum memory usage when a TrueType font is
                            //downloaded as a Type 1 font.

    this->format2p0.numberOfGlyphs = table.format2p0.numberOfGlyphs;
    this->format2p0.glyphNameIndex = table.format2p0.glyphNameIndex;
    this->format2p0.glyphNames = table.format2p0.glyphNames;

    this->format2p5.numberOfGlyphs = table.format2p5.numberOfGlyphs;
    this->format2p5.offset = table.format2p5.offset;


    return *this;
}

void PostTable::combineBytesToStruct(QByteArray & data)
{
#define POSTOUT 0
    qint32 index = 0;

#if POSTOUT
    qDebug() << "--------------post start-------------";
#endif
    this->Format = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("Format : 0x%X", this->Format);
#endif

    this->italicAngle = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("italicAngle : 0x%X", this->italicAngle);
#endif

    this->underlinePosition = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if POSTOUT
    qDebug("underlinePosition : 0x%X", this->underlinePosition);
#endif

    this->underlineThickness = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;
#if POSTOUT
    qDebug("underlineThickness : 0x%X", this->underlineThickness);
#endif
    this->isFixedPitch = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("isFixedPitch : 0x%X", this->isFixedPitch);
#endif

    this->minMemType42 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("minMemType42 : 0x%X", this->minMemType42);
#endif

    this->maxMemType42 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("maxMemType42 : 0x%X", this->maxMemType42);
#endif

    this->minMemType1 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("minMemType1 : 0x%X", this->minMemType1);
#endif

    this->maxMemType1 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;
#if POSTOUT
    qDebug("maxMemType1 : 0x%X", this->maxMemType1);
#endif

    const qint32 nameDataLen = data.count() - index;

    if ( FORMAT_1_0== this->Format|| FORMAT_3_0 == this->Format) //1.0 3.0 没有后面Glyph names
    {
        return;
    }
    else if(FORMAT_2_0 == this->Format)
    {
        /*glyph 的数目*/
        this->format2p0.numberOfGlyphs = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->format2p0.glyphNameIndex = data.mid(index, this->format2p0.numberOfGlyphs * 2);
        index +=  this->format2p0.numberOfGlyphs * 2;

        this->format2p0.glyphNames = data.mid(index, nameDataLen - 2 - this->format2p0.numberOfGlyphs * 2);
        index += nameDataLen - 2 - this->format2p0.numberOfGlyphs * 2;

#if POSTOUT
        qDebug () << index << data.count();
        qDebug () << this->format2p0.glyphNameIndex.count()<< "format 2.0";
        qDebug () << "glyph count" << this->format2p0.numberOfGlyphs;
#endif

    }
    else if(FORMAT_2_5 == this->Format)
    {
        this->format2p5.numberOfGlyphs = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->format2p5.offset =data.mid(index, nameDataLen - 2) ;
#if POSTOUT
        qDebug () << this->format2p5.offset.count()<< "format 2.5";
        qDebug () << "glyph count" << this->format2p5.numberOfGlyphs;
#endif
    }


#if POSTOUT
    qDebug() << "--------------post end-------------";
#endif

} //post

QByteArray PostTable::format2p0GetGlyphName(quint16 index)
{
    QByteArray outByteArray;

    quint8* nameArray = (quint8*)this->format2p0.glyphNames.data();
    quint16 curIndex = 0;
    int i = 0;
    for (i = 0; i < this->format2p0.glyphNames.count(); ++i)
    {
        quint8 nameLen = nameArray[i];

        if (curIndex == index)
        {
            return this->format2p0.glyphNames.mid(i+1, nameLen);
        }
        i += nameLen;
        curIndex++;
    }

    if (i >= this->format2p0.glyphNames.count())
    {
        return QByteArray(1, (char)0);
    }

    return outByteArray;
}

QByteArray PostTable::getGlyphName(quint16 glyphIndex)
{
    QByteArray outArray;
    if (FORMAT_1_0 == this->Format)
    {
        //qDebug () << "format 1.0";
    }
    else if(FORMAT_2_0 == this->Format)
    {
       //qDebug () << "format 2.0";
       quint8* nameData = (quint8*)this->format2p0.glyphNameIndex.data();
       quint16 arrayIndex = glyphIndex * 2;
       /*根据 cmap 中的glyph 索引， 获取name索引*/
       quint16 nameIndex = combineByteTo16(nameData[arrayIndex], nameData[arrayIndex + 1]);

       /*大于258的索引才在后面的glyphNames 表中*/
       if (nameIndex > 257)
       {
           outArray = format2p0GetGlyphName(nameIndex - 258);
       }
    }
    else if(FORMAT_2_5 == this->Format)
    {
        //qDebug () << "format 2.5";
        outArray.append(this->format2p5.offset.at(glyphIndex));
    }
    else if(FORMAT_3_0 == this->Format)
    {
       // qDebug () << "format 3.0";
    }

    return outArray;
}

qint32 PostTable::getGlyphNameIndex(quint16 glyfIndex)
{
    if (FORMAT_1_0 == this->Format)
    {
        //qDebug () << "format 1.0";
    }
    else if(FORMAT_2_0 == this->Format)
    {
       //qDebug () << "format 2.0";
       quint8* nameData = (quint8*)this->format2p0.glyphNameIndex.data();
       quint16 arrayIndex = glyfIndex * 2;
       /*根据 cmap 中的glyph 索引， 获取name索引*/
       quint16 nameIndex = combineByteTo16(nameData[arrayIndex], nameData[arrayIndex + 1]);

       return nameIndex;
    }
    else if(FORMAT_2_5 == this->Format)
    {
        //qDebug () << "format 2.5";
        //outArray.append(this->format2p5.offset.at(glyphIndex));
    }
    else if(FORMAT_3_0 == this->Format)
    {
        //qDebug () << "format 3.0";
    }

    return 0;
}
void PostTable::changeToFormat2p0(quint16 numberGlyphs)
{
    this->clear();
    this->Format = FORMAT_2_0;
    this->format2p0.numberOfGlyphs = numberGlyphs;
    this->format2p0.glyphNameIndex.resize(numberGlyphs * 2);
    this->format2p0.namesCount = 258; //258以前的名称没有在glyphNames中
}

void PostTable::changeNumberGlyphs(quint16 numberGlyphs)
{

    if (FORMAT_2_0 == this->Format)
    {
        qDebug () << numberGlyphs;
        qint32 addCount = numberGlyphs - this->format2p0.numberOfGlyphs;
        this->format2p0.numberOfGlyphs = numberGlyphs;

        if (addCount > 0)
        {
            this->format2p0.glyphNameIndex.append(QByteArray(addCount, (char)0 ));

        }
        else
        {
            this->format2p0.glyphNameIndex.resize(numberGlyphs);
        }

    }
    else if (FORMAT_2_5 == this->Format)
    {

    }
}


void PostTable::addFormat2p0NameArray(quint16 glyfIndex, QByteArray & name)
{
    qint32 indexAddr = glyfIndex * 2;
    this->format2p0.glyphNameIndex.replace(indexAddr, 1, QByteArray(1, char((format2p0.namesCount >> 8) & 0xFF)) );
    this->format2p0.glyphNameIndex.replace(indexAddr + 1, 1, QByteArray(1, char(format2p0.namesCount & 0xFF)) );

    this->format2p0.glyphNames.append(name.count());
    this->format2p0.glyphNames.append(name);

    format2p0.namesCount++;
}

void PostTable::addFormat2p0NameIndex(quint16 glyfIndex, qint16 index)
{
    qint32 indexAddr = glyfIndex * 2;
    //this->format2p0.glyphNameIndex.replace(indexAddr, ((index >> 8) & 0xFF) );
    //this->format2p0.glyphNameIndex.replace(indexAddr + 1, (index & 0xFF) );
    this->format2p0.glyphNameIndex.replace(indexAddr, 1, QByteArray(1, char(((index >> 8) & 0xFF))));
    this->format2p0.glyphNameIndex.replace(indexAddr + 1, 1, QByteArray(1, char((index & 0xFF))) );

    //qDebug() << "indexAddr" << indexAddr << "index" << index;

}


void PostTable::clear()
{
    this->format2p0.glyphNames.clear();
    this->format2p0.glyphNameIndex.clear();
    this->format2p0.numberOfGlyphs = 0;
    this->format2p0.namesCount = 0;

    this->format2p5.numberOfGlyphs = 0;
    this->format2p5.offset.clear();

}

void PostTable::debugOutAllInfo()
{

    if (FORMAT_1_0 == this->Format)
    {
        qDebug () << "format 1.0";
    }
    else if(FORMAT_2_0 == this->Format)
    {
       qDebug () << "format 2.0";
       quint8* nameData = (quint8*)this->format2p0.glyphNameIndex.data();
       for (int i = 0; i < this->format2p0.numberOfGlyphs; i++)
       {
            qDebug("i:0x%x, 0x%x", i, combineByteTo16(nameData[2*i], nameData[2*i+1]));

            if (combineByteTo16(nameData[2*i], nameData[2*i+1]) > 257)
            {
               qDebug() << this->getGlyphName(combineByteTo16(nameData[2*i], nameData[2*i+1]) - 258);
            }


       }
    }
    else if(FORMAT_2_5 == this->Format)
    {
        qDebug () << "format 2.5";
        for (int i = 0; i < this->format2p5.numberOfGlyphs; i++)
        {

        }
    }
    else if(FORMAT_3_0 == this->Format)
    {
        qDebug () << "format 3.0";
    }

}


QByteArray PostTable::getDataForWriteFile()
{
    QByteArray outArray;

    outArray.append(splitU32toByteArray(this->Format));
    outArray.append(splitU32toByteArray(this->italicAngle));
    outArray.append(splitU16toByteArray(this->underlinePosition));
    outArray.append(splitU16toByteArray(this->underlineThickness));
    outArray.append(splitU32toByteArray(this->isFixedPitch));
    outArray.append(splitU32toByteArray(this->minMemType42));
    outArray.append(splitU32toByteArray(this->maxMemType42));
    outArray.append(splitU32toByteArray(this->minMemType1));
    outArray.append(splitU32toByteArray(this->maxMemType1));

    if (FORMAT_1_0 == this->Format)
    {
    }
    else if(FORMAT_2_0 == this->Format)
    {
        outArray.append(splitU16toByteArray(this->format2p0.numberOfGlyphs));
        outArray.append(this->format2p0.glyphNameIndex);
        outArray.append(this->format2p0.glyphNames);
    }
    else if(FORMAT_2_5 == this->Format)
    {
         outArray.append(this->format2p5.offset);
    }
    else if(FORMAT_3_0 == this->Format)
    {
    }

    return outArray;
}
void PostTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
Os2Table::~Os2Table()
{
}

Os2Table & Os2Table::operator= (Os2Table & table)
{
    memcpy(this->tag, table.tag, 4);
    this->version = table.version;// 0x0001
    this->xAvgCharWidth = table.xAvgCharWidth;
    this->usWeightClass = table.usWeightClass;
    this->usWidthClass = table.usWidthClass;
    this->fsType = table.fsType;
    this->ySubscriptXSize = table.ySubscriptXSize;
    this->ySubscriptYSize = table.ySubscriptYSize;
    this->ySubscriptXOffset = table.ySubscriptXOffset;
    this->ySubscriptYOffset = table.ySubscriptYOffset;
    this->ySuperscriptXSize = table.ySuperscriptXSize;
    this->ySuperscriptYSize = table.ySuperscriptYSize;
    this->ySuperscriptXOffset = table.ySuperscriptXOffset;
    this->ySuperscriptYOffset = table.ySuperscriptYOffset;
    this->yStrikeoutSize = table.yStrikeoutSize;
    this->yStrikeoutPosition = table.yStrikeoutPosition;
    this->sFamilyClass = table.sFamilyClass;
    //this->panose[10];
    memcpy(this->panose, table.panose, 10);
    this->ulUnicodeRange1 = table.ulUnicodeRange1;// Bits 0–31
    this->ulUnicodeRange2 = table.ulUnicodeRange2;// Bits 32–63
    this->ulUnicodeRange3 = table.ulUnicodeRange3;// Bits 64–95
    this->ulUnicodeRange4 = table.ulUnicodeRange4;// Bits 96–127
    //this->achVendID[4];
    memcpy(this->achVendID, table.achVendID, 4);
    this->fsSelection = table.fsSelection;
    this->usFirstCharIndex = table.usFirstCharIndex;
    this->usLastCharIndex = table.usLastCharIndex;
    this->sTypoAscender = table.sTypoAscender;
    this->sTypoDescender = table.sTypoDescender;
    this->sTypoLineGap = table.sTypoLineGap;
    this->usWinAscent = table.usWinAscent;
    this->usWinDescent = table.usWinDescent;

    /*version1 以及 以上*/
    this->ulCodePageRange1 = table.ulCodePageRange1; //Bits 0-31
    this->ulCodePageRange2 = table.ulCodePageRange2;// Bits 32-63

    /*下面的verson2 以及 以上才有*/
    this->sxHeight = table.sxHeight;
    this->sCapHeight = table.sCapHeight;
    this->usDefaultChar = table.usDefaultChar;
    this->usBreakChar = table.usBreakChar;

    /*version5 以及以上*/
    this->usMaxContext = table.usMaxContext;
    this->usLowerOpticalPointSize = table.usLowerOpticalPointSize;
    this->usUpperOpticalPointSize = table.usUpperOpticalPointSize;

    return *this;
}

void Os2Table::combineBytesToStruct(QByteArray & data)
{
    quint32 index = 0;

    //qDebug () << "data len" << data.count()<<__FUNCTION__;

    this->version = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->xAvgCharWidth = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usWeightClass = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usWidthClass = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->fsType = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySubscriptXSize = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySubscriptYSize = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySubscriptXOffset = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySubscriptYOffset = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySuperscriptXSize = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySuperscriptYSize = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySuperscriptXOffset = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->ySuperscriptYOffset = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->yStrikeoutSize = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->yStrikeoutPosition = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->sFamilyClass = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    /*PANOSE*/
    for (int i = 0; i < 10; ++i)
    {
        this->panose[i] = data.at(index);
        index++;
    }

    //quint32 ulUnicodeRange1;// Bits 0–31
    this->ulUnicodeRange1 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;

    //quint32 ulUnicodeRange2;// Bits 32–63
    this->ulUnicodeRange2 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;

    //quint32 ulUnicodeRange3;// Bits 64–95
    this->ulUnicodeRange3 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;

    //quint32 ulUnicodeRange4;// Bits 96–127
    this->ulUnicodeRange4 = \
            combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
    index += 4;

    for (int i = 0 ; i < 4; ++i)
    {
        this->achVendID[i] = data.at(index);
        index++;
    }

    this->fsSelection = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usFirstCharIndex = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usLastCharIndex = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->sTypoAscender = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->sTypoDescender = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->sTypoLineGap = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usWinAscent = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

    this->usWinDescent = combineByteTo16(data.at(index+0), data.at(index+1));
    index += 2;

   // qDebug () << this->version<< "total len " << data.count();

    if (this->version >= 0x0001 && data.count() >= 0x56)
    {
        //quint32 ulCodePageRange1; //Bits 0-31
        this->ulCodePageRange1 = \
                combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
        index += 4;

        //quint32 ulCodePageRange2;// Bits 32-63
        this->ulCodePageRange2 = \
                combineByteTo32(data.at(index+0), data.at(index+1), data.at(index+2), data.at(index+3));
        index += 4;

    }

    /*下面的verson 2 以上才有*/
    if (this->version >= 0x0002 && data.count() >= 0x60)
    {
        this->sxHeight = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->sCapHeight = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->usDefaultChar = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->usBreakChar = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->usMaxContext = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

    }

    if (this->version >= 0x005&& data.count() >= 0x64)
    {

        this->usLowerOpticalPointSize = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;

        this->usUpperOpticalPointSize = combineByteTo16(data.at(index+0), data.at(index+1));
        index += 2;
    }


    //qDebug() << "index" <<index << "data len" << data.count();

} //os/2

QByteArray Os2Table::getDataForWriteFile()
{
    QByteArray outArray;

    outArray.append(splitU16toByteArray(this->version));
    outArray.append(splitU16toByteArray(this->xAvgCharWidth));
    outArray.append(splitU16toByteArray(this->usWeightClass));
    outArray.append(splitU16toByteArray(this->usWidthClass));
    outArray.append(splitU16toByteArray(this->fsType));

    outArray.append(splitU16toByteArray(this->ySubscriptXSize));
    outArray.append(splitU16toByteArray(this->ySubscriptYSize));
    outArray.append(splitU16toByteArray(this->ySubscriptXOffset));
    outArray.append(splitU16toByteArray(this->ySubscriptYOffset));
    outArray.append(splitU16toByteArray(this->ySuperscriptXSize));
    outArray.append(splitU16toByteArray(this->ySuperscriptYSize));
    outArray.append(splitU16toByteArray(this->ySuperscriptXOffset));
    outArray.append(splitU16toByteArray(this->ySuperscriptYOffset));

    outArray.append(splitU16toByteArray(this->yStrikeoutSize));
    outArray.append(splitU16toByteArray(this->yStrikeoutPosition));
    outArray.append(splitU16toByteArray(this->sFamilyClass));
    outArray.append((const char*)this->panose, 10);

    outArray.append(splitU32toByteArray(this->ulUnicodeRange1));
    outArray.append(splitU32toByteArray(this->ulUnicodeRange2));
    outArray.append(splitU32toByteArray(this->ulUnicodeRange3));
    outArray.append(splitU32toByteArray(this->ulUnicodeRange4));
    outArray.append((const char*)this->achVendID, 4);


    outArray.append(splitU16toByteArray(this->fsSelection));
    outArray.append(splitU16toByteArray(this->usFirstCharIndex));
    outArray.append(splitU16toByteArray(this->usLastCharIndex));
    outArray.append(splitU16toByteArray(this->sTypoAscender));
    outArray.append(splitU16toByteArray(this->sTypoDescender));
    outArray.append(splitU16toByteArray(this->sTypoLineGap));
    outArray.append(splitU16toByteArray(this->usWinAscent));
    outArray.append(splitU16toByteArray(this->usWinDescent));


    if (this->version >= 0x0001)
    {
        outArray.append(splitU32toByteArray(this->ulCodePageRange1));
        outArray.append(splitU32toByteArray(this->ulCodePageRange2));
    }

    if (this->version >= 0x0002)
    {
        outArray.append(splitU16toByteArray(this->sxHeight));
        outArray.append(splitU16toByteArray(this->sCapHeight));
        outArray.append(splitU16toByteArray(this->usDefaultChar));
        outArray.append(splitU16toByteArray(this->usBreakChar));
        outArray.append(splitU16toByteArray(this->usMaxContext));
    }

    if (this->version >= 0x0005)
    {
        outArray.append(splitU16toByteArray(this->usLowerOpticalPointSize));
        outArray.append(splitU16toByteArray(this->usUpperOpticalPointSize));
    }

    return outArray;
}
void Os2Table::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}



/***************************************************************
*Description: HdmxTable
*Input: 无
*Output: 无
*Return: 无
****************************************************************/
HdmxTable::~HdmxTable()
{
    for (int i = 0; i < this->recordsList.count(); ++i)
    {
        delete this->recordsList.at(i);
    }

    recordsList.clear();
}


HdmxTable & HdmxTable::operator= (HdmxTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->checkSum = table.checkSum;
    this->offset = table.offset;
    this->length = table.length;


    for (int i = 0; i < this->recordsList.count(); ++i)
    {
        delete this->recordsList.at(i);
    }
    this->recordsList.clear();


    for (int i = 0; i < table.recordsList.count(); ++i)
    {
        HdmxTable::HDMX_RECORDS_S *newRecords = new HdmxTable::HDMX_RECORDS_S;
        newRecords->pixelSize = table.recordsList.at(i)->pixelSize;
        newRecords->maxWidth = table.recordsList.at(i)->maxWidth;
        newRecords->widths = table.recordsList.at(i)->widths;
        this->recordsList.append(newRecords);
    }
    this->version = table.version;
    this->numRecords = table.numRecords;
    this->sizeDeviceRecord = this->recordsList.at(0)->widths.count() + 2;

    return *this;
}


void HdmxTable::combineBytesToStruct(QByteArray & data)
{

#define HDMXDGB 0
    qint32 index = 0;

#if HDMXDGB
    qDebug() << "--------------hdmx start-------------";
    qDebug("data len: 0x%x", data.count());
#endif

    this->version = combineByteTo16(data.at(index), data.at(index + 1));
    index += 2;

    this->numRecords = combineByteTo16(data.at(index), data.at(index + 1));
    index += 2;

    this->sizeDeviceRecord = combineByteTo32(data.at(index), data.at(index + 1), data.at(index + 2), data.at(index + 3));
    index += 4;

#if HDMXDGB
    qDebug () << "version" << this->version << "numRecords" << this->numRecords << "sizeDeviceRecord" << this->sizeDeviceRecord;
#endif

    qint32 numGlyphs = this->sizeDeviceRecord - 2;

    for (int i = 0; i < this->numRecords; ++i)
    {
        HDMX_RECORDS_S * newRecords = new HDMX_RECORDS_S;
        newRecords->pixelSize = data.at(index);
        index++;

        newRecords->maxWidth = data.at(index);
        index++;

        newRecords->widths = data.mid(index, numGlyphs);
        index += numGlyphs;

        this->recordsList.append(newRecords);
    }

#if HDMXDGB
    qDebug("end index: 0x%x", index);
    qDebug() << "---------------hdmx end-----------------";
#endif

}

QByteArray HdmxTable::getDataForWriteFile()
{
    QByteArray outArray;

    outArray.append(splitU16toByteArray(this->version));
    outArray.append(splitU16toByteArray(this->numRecords));
    outArray.append(splitU32toByteArray(this->sizeDeviceRecord));

    for (int i = 0; i < this->recordsList.count(); ++i)
    {
        HDMX_RECORDS_S * records = this->recordsList.at(i);
        outArray.append(records->pixelSize);
        outArray.append(records->maxWidth);
        outArray.append(records->widths);
    }

    return outArray;
}

void HdmxTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}


/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
VheaTable::~VheaTable()
{

}

VheaTable & VheaTable::operator= (VheaTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->version = table.version;
    this->ascent = table.ascent;

    this->descent = table.descent;

    this->lineGap = table.lineGap;

    this->advanceHeightMax = table.advanceHeightMax;

    this->minTop = table.minTop;

    this->minBottom = table.minBottom;

    this->yMaxExtent = table.yMaxExtent;

    this->caretSlopeRise = table.caretSlopeRise;

    this->caretSlopeRun = table.caretSlopeRun;

    this->caretOffset = table.caretOffset;

    this->reserved1 = table.reserved1;

    this->reserved2 = table.reserved2;

    this->reserved3 = table.reserved3;

    this->reserved4 = table.reserved4;

    this->metricDataFormat = table.metricDataFormat;

    this->numOfLongVerMetrics = table.numOfLongVerMetrics;

    return *this;
}

void VheaTable::combineBytesToStruct(QByteArray & data)
{
    qint32 index = 0;
#define VHEADBG 0

#if VHEADBG
    qDebug() << "----------------vhea start------------";
    qDebug("data len:0x%x", data.count());
#endif

    this->version = combineByteTo32(data.at(index), data.at(index + 1),
                                      data.at(index + 2), data.at(index + 3));    //Version number of the vertical header table; 0x00010000 for version 1.0
    index += 4;

    this->ascent = combineByteTo16(data.at(index), data.at(index + 1));      //Distance in FUnits from the centerline to the previous line’s descent.
    index += 2;

    this->descent = combineByteTo16(data.at(index), data.at(index + 1));     //Distance in FUnits from the centerline to the next line’s ascent.
    index += 2;

    this->lineGap = combineByteTo16(data.at(index), data.at(index + 1));     //Reserved; set to 0
    index += 2;

    this->advanceHeightMax = combineByteTo16(data.at(index), data.at(index + 1));// The maximum advance height measurement -in FUnits found in the font. This value must be consistent with the entries in the vertical metrics table.
    index += 2;

    this->minTop = combineByteTo16(data.at(index), data.at(index + 1));          //SideBearing The minimum top sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
    index += 2;

    this->minBottom = combineByteTo16(data.at(index), data.at(index + 1));       //SideBearing The minimum bottom sidebearing measurement found in the font,
    index += 2;
                            //in FUnits. This value must be consistent with the entries in the vertical metrics table.
    this->yMaxExtent = combineByteTo16(data.at(index), data.at(index + 1));      //Defined as yMaxExtent=minTopSideBearing+(yMax-yMin)
    index += 2;

    this->caretSlopeRise = combineByteTo16(data.at(index), data.at(index + 1));  //The value of the caretSlopeRise field divided by the value of the caretSlopeRun Field determines the slope of the caret. A value of 0 for the rise and a value of 1 for the run specifies a horizontal caret. A value of 1 for the rise and a value of 0 for the run specifies a vertical caret. Intermediate values are desirable for fonts whose glyphs are oblique or italic. For a vertical font, a horizontal caret is best.
    index += 2;

    this->caretSlopeRun = combineByteTo16(data.at(index), data.at(index + 1));   //See the caretSlopeRise field. Value=1 for nonslanted vertical fonts.
    index += 2;

    this->caretOffset = combineByteTo16(data.at(index), data.at(index + 1));     // The amount by which the highlight on a slanted glyph needs to be shifted away from the glyph in order to produce the best appearance. Set value equal to 0 for nonslanted fonts.
    index += 2;

    this->reserved1 = combineByteTo16(data.at(index), data.at(index + 1));        //Set to 0.
    index += 2;

    this->reserved2 = combineByteTo16(data.at(index), data.at(index + 1));        //Set to 0.
    index += 2;

    this->reserved3 = combineByteTo16(data.at(index), data.at(index + 1));        //Set to 0.
    index += 2;

    this->reserved4 = combineByteTo16(data.at(index), data.at(index + 1));        //Set to 0.
    index += 2;

    this->metricDataFormat = combineByteTo16(data.at(index), data.at(index + 1));        //Set to 0.
    index += 2;

    this->numOfLongVerMetrics = combineByteTo16(data.at(index), data.at(index + 1));    //Number of advance heights in the vertical metrics table.
    index += 2;
    //qDebug () << "count of metrics" << this->numOfLongVerMetrics;

#if VHEADBG
    qDebug() << "----------------vhea end------------";
#endif
}

QByteArray VheaTable::getDataForWriteFile()
{
    QByteArray outArray;

    outArray.append(splitU32toByteArray(this->version));
    outArray.append(splitU16toByteArray(this->ascent));
    outArray.append(splitU16toByteArray(this->descent));
    outArray.append(splitU16toByteArray(this->lineGap));
    outArray.append(splitU16toByteArray(this->advanceHeightMax));
    outArray.append(splitU16toByteArray(this->minTop));
    outArray.append(splitU16toByteArray(this->minBottom));
    outArray.append(splitU16toByteArray(this->yMaxExtent));
    outArray.append(splitU16toByteArray(this->caretSlopeRise));
    outArray.append(splitU16toByteArray(this->caretSlopeRun));
    outArray.append(splitU16toByteArray(this->caretOffset));
    outArray.append(splitU16toByteArray(this->reserved1));
    outArray.append(splitU16toByteArray(this->reserved2));
    outArray.append(splitU16toByteArray(this->reserved3));
    outArray.append(splitU16toByteArray(this->reserved4));
    outArray.append(splitU16toByteArray(this->metricDataFormat));
    outArray.append(splitU16toByteArray(this->numOfLongVerMetrics));

    return outArray;
}

void VheaTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}

/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
VmtxTable::~VmtxTable()
{

}

VmtxTable & VmtxTable::operator= (VmtxTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->data = table.data;
    return *this;
}

void VmtxTable::combineBytesToStruct(QByteArray & data)
{
#define VMTXDBG 0

#if VMTXDBG
    qDebug() << "----------------vmtx start------------";
    qDebug("data len:0x%x", data.count());
#endif

    this->data = data;

#if VMTXDBG
    qDebug() << "----------------vmtx end------------";
#endif
}

QByteArray VmtxTable::getDataForWriteFile()
{
    QByteArray outArray;

    if (this->data.isEmpty())
    {
        outArray.append(this->advanceHeight);
        outArray.append(this->topSideBearing);
    }
    else
    {
        return this->data;
    }

    return outArray;
}

void VmtxTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}

void VmtxTable::parseData(quint16 numGlyphs)
{
    if (numGlyphs * 2 == this->data.count())
    {
        this->topSideBearing = this->data.mid(0, numGlyphs*2);
    }
    else if(numGlyphs * 4 == this->data.count())
    {
        this->advanceHeight = this->data.mid(0, numGlyphs*2);
        this->topSideBearing = this->data.mid(numGlyphs, numGlyphs*2);
    }
    else
    {
        qDebug() << "呵呵";
    }

}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
UnknowTable::~UnknowTable()
{

}

UnknowTable & UnknowTable::operator= (UnknowTable & table)
{
    memcpy(this->tag, table.tag, 4);
    this->data = table.data;
    return *this;
}

void UnknowTable::combineBytesToStruct(QByteArray & data)
{
    this->data = data;
} //unknow

QByteArray UnknowTable::getDataForWriteFile()
{
    return this->data;
}
void UnknowTable::setDataOffsetInfile(qint32 offset)
{
    this->offset = offset;
}



/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
TtfFile::TtfFile()
{
    this->ttfInfo = new TTF_INFO_S;
    memset(&ttfInfo->ttfFileHead, 0, sizeof(TTF_FILE_HEAD_S));
}

TtfFile::TtfFile(TtfFile & ttfFile)
{
    this->ttfInfo = new TTF_INFO_S;
    this->ttfInfo->ttfFileHead = ttfFile.ttfInfo->ttfFileHead;

    QMap <QString, TableEntryAbstract*>::const_iterator tableMapIterator;
    for (tableMapIterator = ttfFile.ttfInfo->tableMap.begin(); tableMapIterator != ttfFile.ttfInfo->tableMap.end(); ++tableMapIterator)
    {
        QString tag = tableMapIterator.key();
        this->ttfInfo->tableMap.insert(tag, this->copyOneTable(tag, tableMapIterator.value()));
    }
}

TtfFile::~TtfFile()
{
    this->clear();
    delete this->ttfInfo;
}

TtfFile & TtfFile::operator= (TtfFile & ttfFile)
{
    this->ttfInfo->tableMap.clear();
    QMap <QString, TableEntryAbstract*>::const_iterator tableMapIterator;
    for (tableMapIterator = ttfFile.ttfInfo->tableMap.begin(); tableMapIterator != ttfFile.ttfInfo->tableMap.end(); ++tableMapIterator)
    {
        QString tag = tableMapIterator.key();
        this->ttfInfo->tableMap.insert(tag, this->copyOneTable(tag, tableMapIterator.value()));
    }
    return *this;
}
/***************************************************************
*Description: 解析文件头数据
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
 void TtfFile::showMessageBoxBadFile(QString info)
 {
     QMessageBox::information(NULL, tr("File open"), info, \
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
 }

TableEntryAbstract* TtfFile::copyOneTable(QString tag, TableEntryAbstract * table)
{
    //qDebug () << tag;
    if (tag.contains("cmap"))
    {
        CmapTable * entry = (CmapTable *)table;
        CmapTable * newEntry = new CmapTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("glyf"))
    {
        GlyfTable * entry = (GlyfTable *)table;
        GlyfTable * newEntry = new GlyfTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains( "head"))
    {
        HeadTable * entry = (HeadTable *)table;
        HeadTable * newEntry = new HeadTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("hhea"))
    {
        HheaTable * entry = (HheaTable *)table;
        HheaTable * newEntry = new HheaTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("hmtx"))
    {
        HmtxTable * entry = (HmtxTable *)table;
        HmtxTable * newEntry = new HmtxTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("loca"))
    {
        LocaTable * entry = (LocaTable *)table;
        LocaTable * newEntry = new LocaTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("maxp"))
    {
        MaxpTable * entry = (MaxpTable *)table;
        MaxpTable * newEntry = new MaxpTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("name"))
    {
        NameTable * entry = (NameTable *)table;
        NameTable * newEntry = new NameTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("post"))
    {
        PostTable * entry = (PostTable *)table;
        PostTable * newEntry = new PostTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("OS/2"))
    {
        Os2Table * entry = (Os2Table *)table;
        Os2Table * newEntry = new Os2Table();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("hdmx"))
    {
        HdmxTable * entry = (HdmxTable *)table;
        HdmxTable * newEntry = new HdmxTable();
        *newEntry = *entry;
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("vhea"))
    {
        VheaTable * entry = (VheaTable *)table;
        VheaTable * newEntry = new VheaTable();
         *newEntry = *entry;
        return (TableEntryAbstract*)newEntry;
    }
    else if(tag.contains("vmtx"))
    {
        VmtxTable * entry = (VmtxTable *)table;
        VmtxTable * newEntry = new VmtxTable();
         *newEntry = *entry;
        return (TableEntryAbstract*)newEntry;
    }
    else //if(tag.contains("unknow"))
    {
        UnknowTable * entry = (UnknowTable *)table;
        UnknowTable * newEntry = new UnknowTable();
        *newEntry = *entry;
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)newEntry;
    }

}

TableEntryAbstract* TtfFile:: createEntry(QString tag)
{
    if (4 != tag.count())
    {
        //return NULL;
    }
    //qDebug () << tag;

    if (tag.contains("cmap"))
    {
        CmapTable * entry = new CmapTable();
        //memset(entry, 0, sizeof(CMAP_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("glyf"))
    {
        GlyfTable * entry = new GlyfTable();;
        //memset(entry, 0, sizeof(GLYF_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains( "head"))
    {
        HeadTable * entry = new HeadTable();
        //memset(entry, 0, sizeof(HEAD_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("hhea"))
    {
        HheaTable * entry = new HheaTable();;
        //memset(entry, 0, sizeof(HHEA_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("hmtx"))
    {
        HmtxTable * entry = new HmtxTable();;
        //memset(entry, 0, sizeof(HMTX_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("loca"))
    {
        LocaTable * entry = new LocaTable();;
        //memset(entry, 0, sizeof(LOCA_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("maxp"))
    {
        MaxpTable * entry = new MaxpTable();;
        //memset(entry, 0, sizeof(MAXP_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("name"))
    {
        NameTable * entry = new NameTable();;
        //memset(entry, 0, sizeof(NAME_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("post"))
    {
        PostTable * entry = new PostTable();;
        //memset(entry, 0, sizeof(POST_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("OS/2"))
    {
        Os2Table * entry = new Os2Table();;
        //memset(entry, 0, sizeof(OS_2_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("hdmx"))
    {
        HdmxTable * entry = new HdmxTable();;
        //memset(entry, 0, sizeof(OS_2_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("vhea"))
    {
        VheaTable * entry = new VheaTable();
        return (TableEntryAbstract*)entry;
    }
    else if(tag.contains("vmtx"))
    {
        VmtxTable * entry = new VmtxTable();
        return (TableEntryAbstract*)entry;
    }
    else //if(tag.contains("unknow"))
    {
        UnknowTable * entry = new UnknowTable();;
        //memset(entry, 0, sizeof(UNKNOWN_TABLE_S));
        return (TableEntryAbstract*)entry;
    }
}

/***************************************************************
*Description: table表
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
void TtfFile::createTableMap(QByteArray data, QMap <QString, TableEntryAbstract*> & map)
{
    /*提取tag*/
    QString tag;
    for (int index = 0;index < 4; ++index)
    {
        tag.append(data.at(index));
    }

    /*创建节点*/
    TableEntryAbstract *newEntry = createEntry(tag);
    if (NULL != newEntry)
    {
        qint32 index = 0;
        memcpy(newEntry->tag, tag.toLatin1(), 4 );
        index += 4;

        newEntry->checkSum = combineByteTo32(data.at(index + 0), data.at(index+1), data.at(index+2), data.at(index+3));
        index += 4;

        newEntry->offset = combineByteTo32(data.at(index + 0), data.at(index+1), data.at(index+2), data.at(index+3));
        index += 4;

        newEntry->length = combineByteTo32(data.at(index + 0), data.at(index+1), data.at(index+2), data.at(index+3));
        index += 4;
        map.insert(tag, newEntry);
    }
    else
    {
        qDebug () << "呵呵";
    }
}

/***************************************************************
*Description: table表
*Input: data--- 原始数据， len 长度
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
qint32 TtfFile::readTableData(QFile & file, quint32 pos, QMap<QString, TableEntryAbstract*> & map)
{
    qint32 readSize = 0;
    QByteArray dataBuf;
    QMap<QString, TableEntryAbstract*>::const_iterator tableIterator;

    //qDebug () << "------------------------";
    /*偏移对应的表*/
    for (tableIterator = map.begin(); tableIterator != map.end(); ++tableIterator)
    {
        //qDebug ("pos:%x, offset:0x%x", pos, tableIterator.value()->offset);
        if (pos == tableIterator.value()->offset)
        {
            readSize = (tableIterator.value()->length + 3) & (~3);
            dataBuf = file.read(readSize);
            if (dataBuf.count() < readSize)
            {
                showMessageBoxBadFile(tr("Bad file:%!").arg(__FUNCTION__));
                return 0;
            }
            dataBuf.resize(tableIterator.value()->length);
            tableIterator.value()->combineBytesToStruct(dataBuf);
            //qDebug() << iterator.key();
            //qDebug ("pos:%x, size:%x", pos, readSize);
            //parseTableData(tag, iterator.value(), dataBuf);
            break;
        }
    }

    if (tableIterator == map.end())
    {
        return 0;
    }

    return readSize;
}

/***************************************************************
*Description:  将一项中文翻译信息统计到map
*Input: line: 文本行数， tranString：翻译文本
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
void TtfFile::clear()
{
    memset(&this->ttfInfo->ttfFileHead, 0, sizeof(TTF_FILE_HEAD_S));

    QMap<QString, TableEntryAbstract*>::const_iterator iterator;

    /*偏移对应的表*/
    for (iterator = this->ttfInfo->tableMap.begin(); iterator != this->ttfInfo->tableMap.end(); ++iterator)
    {
        //qDebug () << iterator.key();
        delete iterator.value();
    }
    this->ttfInfo->tableMap.clear();
}

void TtfFile::ttfFileHeadSetTableNums(quint16 nums)
{
    this->ttfInfo->ttfFileHead.numTables = nums;

    this->ttfInfo->ttfFileHead.searchRange = qNextPowerOfTwo(nums) / 2 * 16;

    //logx(y)=ln(y)/ln(x)
    this->ttfInfo->ttfFileHead.entrySelector = mySimpleLog(2, qNextPowerOfTwo(nums) / 2);
    this->ttfInfo->ttfFileHead.rangeShift = this->ttfInfo->ttfFileHead.numTables * 16
                                             - this->ttfInfo->ttfFileHead.searchRange;

}


TableEntryAbstract * TtfFile::getTableEntry(QString tag)
{
    return this->ttfInfo->tableMap.value(tag);
}

/***************************************************************
*Description:  将一项中文翻译信息统计到map
*Input: line: 文本行数， tranString：翻译文本
*Output: map：中文翻译项统计map
*Return: 无
****************************************************************/
bool TtfFile::ttfFileOpen(QString fileName)
{
    QByteArray dataBuf;
    qint32 i = 0;
    QFile file;

    if(fileName.isEmpty())
    {
        qDebug()<<("file name null\n");
        return false;
    }

    file.setFileName(fileName);
    if (false == file.open(QIODevice::ReadOnly))
    {
        showMessageBoxBadFile(file.errorString());
        return false;
    }

    this->clear();

    /*读取文件头*/
    dataBuf = file.read( TTF_HEAD_SIZE);
    if (dataBuf.count() < TTF_HEAD_SIZE)
    {
        showMessageBoxBadFile(tr("File head data not enough"));
        return false;
    }
    if (!ttfInfo->ttfFileHead.combineTheByteToStruct(dataBuf, &ttfInfo->ttfFileHead))
    {
        showMessageBoxBadFile(tr("你打开的可能不是一个字库文件"));
        return false;
    }

    /*读取table*/
    for (i = 0; i < ttfInfo->ttfFileHead.numTables; ++i)
    {
        dataBuf = file.read(TABLE_ENTYR_SIZE);
        if (dataBuf.count() < TABLE_ENTYR_SIZE)
        {
            showMessageBoxBadFile(tr("Table index data not enough"));
            return false;
        }
        createTableMap(dataBuf, ttfInfo->tableMap);
    }

    /*读取table数据*/
    qint32 pos = 0;
    qint32 size = 0;
    pos = file.pos();

    for (i = 0; i < ttfInfo->ttfFileHead.numTables; ++i)
    {
        size = readTableData(file, pos, ttfInfo->tableMap);
        if (size <= 0)
        {
            showMessageBoxBadFile(tr("table index：%1，Table data <= 0").arg(i));
            return false;
        }
        pos += size;
    }

    file.close();

    return true;
}


void TtfFile::ttfFileSave(QString fileName)
{
    QFile file;

    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        showMessageBoxBadFile(tr("Save ttf file:%1").arg(file.errorString()));
        return;
    }

    /*写文件头*/
    this->ttfFileHeadSetTableNums(this->ttfInfo->tableMap.count()); //重新设置表数量相关数据
    file.write(splitU32toByteArray(this->ttfInfo->ttfFileHead.sfntversion));
    file.write(splitU16toByteArray(this->ttfInfo->tableMap.count()));
    file.write(splitU16toByteArray(this->ttfInfo->ttfFileHead.searchRange));
    file.write(splitU16toByteArray(this->ttfInfo->ttfFileHead.entrySelector));
    file.write(splitU16toByteArray(this->ttfInfo->ttfFileHead.rangeShift));

    /*先写空表头*/
    qint64 tableEntryPos = file.pos();
    file.write(QByteArray().append(this->ttfInfo->tableMap.count() * 16, 0));

    /*写数据并计算数据*/
    QMap <QString, TableEntryAbstract*>::iterator mapIterator;
    for (mapIterator = this->ttfInfo->tableMap.begin(); mapIterator != this->ttfInfo->tableMap.end(); ++mapIterator)
    {
        TableEntryAbstract * curTable = mapIterator.value();
        QByteArray data = curTable->getDataForWriteFile();
        mapIterator.value()->offset = file.pos();
        mapIterator.value()->checkSum = calcTableChecksum(data);
        mapIterator.value()->length = data.count();

        addUpArrayTo4IntegerTimes(data);//四字节对其
        file.write(data);
    }

    /*再写头*/
    file.seek(tableEntryPos);
    for (mapIterator = this->ttfInfo->tableMap.begin(); mapIterator != this->ttfInfo->tableMap.end(); ++mapIterator)
    {
        TableEntryAbstract * curTable = mapIterator.value();
        file.write((const char *)curTable->tag, 4);
        file.write(splitU32toByteArray(curTable->checkSum));
        file.write(splitU32toByteArray(curTable->offset));
        file.write(splitU32toByteArray(curTable->length));
    }

    file.close();
}

