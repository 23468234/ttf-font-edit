#include "ttfmodify.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QtMath>
#include <QMap>

#define UNUSED(x) (x = x)

UNICODE_TABLE_EX_S unicodeTable[] =
{
    {0, "Basic, Latin",                             0x0000, 0x007F},
    {1, "Latin, 0x1, Supplement",                   0x0080, 0x00FF},
    {2, "Latin, Extended, 0xA",                     0x0100, 0x017F},
    {3, "Latin, Extended, 0xB",                     0x0180, 0x024F},
    {4, "IPA, Extensions",                          0x0250, 0x02AF},
    {4, "Phonetic, Extensions",                     0x1D00, 0x1D7F},
    {4, "Phonetic, Extensions, Supplement",         0x1D80, 0x1DBF},
    {5, "Spacing, Modifier, Letters", 0x02B0, 0x02FF},
    {5, "Modifier, Tone, Letters", 0xA700, 0xA71F},
    {6, "Combining, Diacritical, Marks", 0x0300, 0x036F},
    {6, "Combining, Diacritical, MarksX, Supplement", 0x1DC0, 0x1DFF},
    {7, "Greek, and, Coptic", 0x0370, 0x03FF},
    {8, "Coptic", 0x2C80, 0x2CFF},
    {9, "Cyrillic", 0x0400, 0x04FF},
    {9, "Cyrillic, Supplement", 0x0500, 0x052F},
    {9, "Cyrillic, Extended, 0xA", 0x2DE0, 0x2DFF},
    {9, "Cyrillic, Extended, 0xB", 0xA640, 0xA69F},
    {10,"Armenian", 0x0530, 0x058F},
    {11, "Hebrew", 0x0590, 0x05FF},
    {12, "Vai", 0xA500, 0xA63F},
    {13, "Arabic", 0x0600, 0x06FF},
    {13, "Arabic, Supplement", 0x0750, 0x077F},
    {14, "NKo", 0x07C0, 0x07FF},
    {15, "Devanagari", 0x0900, 0x097F},
    {16, "Bengali", 0x0980, 0x09FF},
    {17, "Gurmukhi", 0x0A00, 0x0A7F},
    {18, "Gujarati", 0x0A80, 0x0AFF},
    {19, "Oriya", 0x0B00, 0x0B7F},
    {20, "Tamil", 0x0B80, 0x0BFF},
    {21, "Telugu", 0x0C00, 0x0C7F},
    {22, "Kannada", 0x0C80, 0x0CFF},
    {23, "Malayalam", 0x0D00, 0x0D7F},
    {24, "Thai", 0x0E00, 0x0E7F},
    {25, "Lao", 0x0E80, 0x0EFF},
    {26, "Georgian", 0x10A0, 0x10FF},
    {26, "Georgian, Supplement", 0x2D00, 0x2D2F},
    {27, "Balinese", 0x1B00, 0x1B7F},
    {28, "Hangul, Jamo", 0x1100, 0x11FF},
    {29, "Latin, Extended, Additional", 0x1E00, 0x1EFF},
    {29, "Latin, Extended, 0xC", 0x2C60, 0x2C7F},
    {29, "Latin, Extended, 0xD", 0xA720, 0xA7FF},
    {30, "Greek, Extended", 0x1F00, 0x1FFF},
    {31, "General, Punctuation", 0x2000, 0x206F},
    {31, "Supplemental, Punctuation", 0x2E00, 0x2E7F},
    {32, "Superscripts, And, Subscripts", 0x2070, 0x209F},
    {33, "Currency, Symbols", 0x20A0, 0x20CF},
    {34, "Combining, Diacritical, Marks, For, Symbols", 0x20D0, 0x20FF},
    {35, "Letterlike, Symbols", 0x2100, 0x214F},
    {36, "Number, Forms", 0x2150, 0x218F},
    {37, "Arrows", 0x2190, 0x21FF},
    {37, "Supplemental, Arrows, 0xA", 0x27F0, 0x27FF},
    {37, "Supplemental, Arrows, 0xB", 0x2900, 0x297F},
    {37, "Miscellaneous, Symbols, and, Arrows", 0x2B00, 0x2BFF},
    {38, "Mathematical, Operators", 0x2200, 0x22FF},
    {38, "Supplemental, Mathematical, Operators", 0x2A00, 0x2AFF},
    {38, "Miscellaneous, Mathematical, Symbols, 0xA", 0x27C0, 0x27EF},
    {38, "Miscellaneous, Mathematical, Symbols, 0xB", 0x2980, 0x29FF},
    {39, "Miscellaneous, Technical", 0x2300, 0x23FF},
    {40, "Control, Pictures", 0x2400, 0x243F},
    {41, "Optical, Character, Recognition", 0x2440, 0x245F},
    {42, "Enclosed, Alphanumerics", 0x2460, 0x24FF},
    {43, "Box, Drawing", 0x2500, 0x257F},
    {44, "Block, Elements", 0x2580, 0x259F},
    {45, "Geometric, Shapes", 0x25A0, 0x25FF},
    {46, "Miscellaneous, Symbols", 0x2600, 0x26FF},
    {47, "Dingbats", 0x2700, 0x27BF},
    {48, "CJK, Symbols, And, Punctuation", 0x3000, 0x303F},
    {49, "Hiragana", 0x3040, 0x309F},
    {50, "Katakana", 0x30A0, 0x30FF},
    {50, "Katakana, Phonetic, Extensions", 0x31F0, 0x31FF},
    {51, "Bopomofo", 0x3100, 0x312F},
    {51, "Bopomofo, Extended", 0x31A0, 0x31BF},
    {52, "Hangul, Compatibility, Jamo", 0x3130, 0x318F},
    {53, "Phags, 0xpa", 0xA840, 0xA87F},
    {54, "Enclosed, CJK, Letters, And, Months", 0x3200, 0x32FF},
    {55, "CJK, Compatibility", 0x3300, 0x33FF},
    {56, "Hangul, Syllables", 0xAC00, 0xD7AF},
    {57, "Non, 0xPlane, 0, *", 0xD800, 0xDFFF},
    {58, "Phoenician", 0x10900, 0x1091F},
    {59, "CJK, Unified, Ideographs", 0x4E00, 0x9FFF},
    {59, "CJK, Radicals, Supplement", 0x2E80, 0x2EFF},
    {59, "Kangxi, Radicals", 0x2F00, 0x2FDF},
    {59, "Ideographic, Description, Characters", 0x2FF0, 0x2FFF},
    {59, "CJK, Unified, Ideographs, Extension, A", 0x3400, 0x4DBF},
    {59, "CJK, Unified, Ideographs, Extension, B", 0x20000, 0x2A6DF},
    {59, "Kanbun", 0x3190, 0x319F},
    {60, "Private, Use, Area, (plane, 0)", 0xE000, 0xF8FF},
    {61, "CJK, Strokes", 0x31C0, 0x31EF},
    {61, "CJK, Compatibility, Ideographs", 0xF900, 0xFAFF},
    {61, "CJK, Compatibility, Ideographs, Supplement", 0x2F800, 0x2FA1F},
    {62, "Alphabetic, Presentation, Forms", 0xFB00, 0xFB4F},
    {63, "Arabic, Presentation, Forms, 0xA", 0xFB50, 0xFDFF},
    {64, "Combining, Half, Marks", 0xFE20, 0xFE2F},
    {65, "Vertical, Forms", 0xFE10, 0xFE1F},
    {65, "CJK, Compatibility, Forms", 0xFE30, 0xFE4F},
    {66, "Small, Form, Variants", 0xFE50, 0xFE6F},
    {67, "Arabic, Presentation, Forms, 0xB", 0xFE70, 0xFEFF},
    {68, "Halfwidth, And, Fullwidth, Forms", 0xFF00, 0xFFEF},
    {69, "Specials", 0xFFF0, 0xFFFF},
    {70, "Tibetan", 0x0F00, 0x0FFF},
    {71, "Syriac", 0x0700, 0x074F},
    {72, "Thaana", 0x0780, 0x07BF},
    {73, "Sinhala", 0x0D80, 0x0DFF},
    {74, "Myanmar", 0x1000, 0x109F},
    {75, "Ethiopic", 0x1200, 0x137F},
    {75, "Ethiopic, Supplement", 0x1380, 0x139F},
    {75, "Ethiopic, Extended", 0x2D80, 0x2DDF},
    {76, "Cherokee", 0x13A0, 0x13FF},
    {77, "Unified, Canadian, Aboriginal, Syllabics", 0x1400, 0x167F},
    {78, "Ogham", 0x1680, 0x169F},
    {79, "Runic", 0x16A0, 0x16FF},
    {80, "Khmer", 0x1780, 0x17FF},
    {80, "Khmer, Symbols", 0x19E0, 0x19FF},
    {81, "Mongolian", 0x1800, 0x18AF},
    {82, "Braille, Patterns", 0x2800, 0x28FF},
    {83, "Yi, Syllables", 0xA000, 0xA48F},
    {83, "Yi, Radicals", 0xA490, 0xA4CF},
    {84, "Tagalog", 0x1700, 0x171F},
    {84, "Hanunoo", 0x1720, 0x173F},
    {84, "Buhid", 0x1740, 0x175F},
    {84, "Tagbanwa", 0x1760, 0x177F},
    {85, "Old, Italic", 0x10300, 0x1032F},
    {86, "Gothic", 0x10330, 0x1034F},
    {87, "Deseret", 0x10400, 0x1044F},
    {88, "Byzantine, Musical, Symbols", 0x1D000, 0x1D0FF},
    {88, "Musical, Symbols", 0x1D100, 0x1D1FF},
    {88, "Ancient, Greek, Musical, Notation", 0x1D200, 0x1D24F},
    {89, "Mathematical, Alphanumeric, Symbols", 0x1D400, 0x1D7FF},
    {90, "Private, Use, (plane, 15)", 0xFF000, 0xFFFFD},
    {90, "Private, Use, (plane, 16)", 0x100000, 0x10FFFD},
    {91, "Variation, Selectors", 0xFE00, 0xFE0F},
    {91, "Variation, Selectors, Supplement", 0xE0100, 0xE01EF},
    {92, "Tags", 0xE0000, 0xE007F},
    {93, "Limbu", 0x1900, 0x194F},
    {94, "Tai, Le", 0x1950, 0x197F},
    {95, "New, Tai, Lue", 0x1980, 0x19DF},
    {96, "Buginese", 0x1A00, 0x1A1F},
    {97, "Glagolitic", 0x2C00, 0x2C5F},
    {98, "Tifinagh", 0x2D30, 0x2D7F},
    {99, "Yijing, Hexagram, Symbols", 0x4DC0, 0x4DFF},
    {100, "Syloti, Nagri", 0xA800, 0xA82F},
    {101, "Linear, B, Syllabary", 0x10000, 0x1007F},
    {101, "Linear, B, Ideograms", 0x10080, 0x100FF},
    {101, "Aegean, Numbers", 0x10100, 0x1013F},
    {102, "Ancient, Greek, Numbers", 0x10140, 0x1018F},
    {103, "Ugaritic", 0x10380, 0x1039F},
    {104, "Old, Persian", 0x103A0, 0x103DF},
    {105, "Shavian", 0x10450, 0x1047F},
    {106, "Osmanya", 0x10480, 0x104AF},
    {107, "Cypriot, Syllabary", 0x10800, 0x1083F},
    {108, "Kharoshthi", 0x10A00, 0x10A5F},
    {109, "Tai, Xuan, Jing, Symbols", 0x1D300, 0x1D35F},
    {110, "Cuneiform", 0x12000, 0x123FF},
    {110, "Cuneiform, Numbers, and, Punctuation",   0x12400, 0x1247F},
    {111, "Counting, Rod, Numerals",                0x1D360, 0x1D37F},
    {112, "Sundanese",                              0x1B80, 0x1BBF},
    {113, "Lepcha",                                 0x1C00, 0x1C4F},
    {114, "Ol, Chiki",                              0x1C50, 0x1C7F},
    {115, "Saurashtra",                             0xA880, 0xA8DF},
    {116, "Kayah, Li",                              0xA900, 0xA92F},
    {117, "Rejang",                                 0xA930, 0xA95F},
    {118, "Cham",                                   0xAA00, 0xAA5F},
    {119, "Ancient, Symbols",                       0x10190, 0x101CF},
    {120, "Phaistos, Disc",                         0x101D0, 0x101FF},
    {121, "Carian",                                 0x102A0, 0x102DF},
    {121, "Lycian",                                 0x10280, 0x1029F},
    {121, "Lydian", 0x10920, 0x1093F},
    {122, "Domino, Tiles",      0x1F030, 0x1F09F},
    {122, "Mahjong, Tiles",     0x1F000, 0x1F02F},
        //123-0x127,Reserved, for, process, 0xinternal usage

};

#define UNICODETABLECOUNT (sizeof(unicodeTable) / sizeof(unicodeTable[0]))




TtfModify::TtfModify()
{
    ttfFile = new TtfFile;
}

TtfModify::~TtfModify()
{
    delete ttfFile;
}

bool TtfModify::ttfFileOpen(QString & fileName)
{
    if (this->ttfFile->ttfFileOpen(fileName))
    {
        this->unicodeGlyfIndexMap.clear();
        this->charsetMap.clear();
        this->ttfFileCal();
        return true;
    }
    return false;
}

void TtfModify::fillListView(QStandardItemModel * standardMode1)
{
    standardMode1->clear();

    QMap<quint8, QMap<quint16, bool>>::const_iterator unicodeTableMap;
    for (unicodeTableMap = this->charsetMap.begin(); unicodeTableMap != this->charsetMap.end(); ++unicodeTableMap)
    {
        //qDebug () << "count" << unicodeTableMap.value().count();
        QStandardItem * item = new QStandardItem(QString(unicodeTable[unicodeTableMap.key()].name));
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
        standardMode1->appendRow(item);
        //qDebug ("charsetStartBk:0x%x", charsetStartBk);
    }
}

/***************************************************************
*Description: 获取打开的ttf文件中所有字符集
*Input:    无
*Output:    无
*Return: 字符集编号对应该字符集中的所有unicode
****************************************************************/
QMap<quint8, QMap<quint16, bool>> & TtfModify::ttfGetCharsetMap()
{
    return this->charsetMap;
}
/***************************************************************
*Description: 获取打开的ttf文件中所有的unicode
*Input:    无
*Output:    无
*Return:  unicode对应索引
****************************************************************/
QMap<quint16, quint16> & TtfModify::getUnicodeToGlyfMap()
{
    return this->unicodeGlyfIndexMap;
}

/***************************************************************
*Description: 获取新打开的ttf文件中所有的unicode、将uicode分字符集
*Input:    无
*Output:    无
*Return:    true：成功，false:失败
****************************************************************/
bool TtfModify::ttfFileCal()
{
    CmapTable *cmap = (CmapTable*)this->ttfFile->getTableEntry("cmap");

    if (NULL == cmap)
    {
        return false;
    }
    else
    {
        this->unicodeGlyfIndexMap = cmap->getUnicodeGlyfIndexMap();
    }

    //for (int)

    /*<charset表索引， unicode>*/
    qint32 charsetStartBk = -1;
    quint8 tableIndexBk = -1;
    QMap<quint16, quint16>::const_iterator codeToGlyphIterator;
    QMap<quint16, bool> unicodeMap;

    /*循环所有的字库中包含的所有的unicode*/
    for (codeToGlyphIterator = this->unicodeGlyfIndexMap.begin(); codeToGlyphIterator != this->unicodeGlyfIndexMap.end(); ++codeToGlyphIterator)
    {
        qint32 curUnicode = codeToGlyphIterator.key();

        /*找到该unicode在unicode表中的区间*/
        for (quint32 i = 0; i < UNICODETABLECOUNT; ++i)
        {
            if(curUnicode >= unicodeTable[i].rangeStart && curUnicode < unicodeTable[i].rangeEnd)
            {
                //qDebug ("curUnicode:0x%x", curUnicode);
                qint32 charsetStart = unicodeTable[i].rangeStart;
                if (-1 != charsetStartBk && charsetStart != charsetStartBk)
                {
                    this->charsetMap.insert(tableIndexBk, unicodeMap);
                    unicodeMap.clear();
                   //qDebug() << "list count" << this->charsetMap.value(tableIndexBk).count();
                    //qDebug ("charsetStartBk:0x%x", charsetStartBk);
                }

                unicodeMap.insert(curUnicode, true);
                charsetStartBk = charsetStart;
                tableIndexBk = i;
            }
        }
    }

    if (!unicodeMap.isEmpty())
    {
        this->charsetMap.insert(tableIndexBk, unicodeMap);
    }

    return true;
}

/***************************************************************
*Description: 新建字库中的复合字形，字形索引处理
*Input: glyfData----复合字形数据
*       unicodeAndGlyf---新字库 unicode对应索引
*       olDunicodeAndGlyf---老子库中unicode对应索引
*Output: outMissedGlyf---新字库中缺少的图形，在老老字库中的索引
*Return: 无
****************************************************************/
void TtfModify::modifyCompositeGlyf(QByteArray & glyfData, QMap<quint16, qint32> & unicodeAndGlyf,
                                    QMap<quint16, quint16> & olDunicodeAndGlyf, QList<quint16> & outMissedGlyf)
{

    //qDebug () << "data len" << glyfData.count();
    qint32 index = 0;

    qint16 numberOfContours = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//If the number of contours is greater than or equal to zero, this is a single glyph; if negative, this is a composite glyph.
    UNUSED(numberOfContours);
    index += 2;

    qint16 xMin = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//Minimum x for coordinate data.
    UNUSED(xMin);
    index += 2;

    qint16 yMin = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//Minimum y for coordinate data.
    UNUSED(yMin);
    index += 2;

    qint16 xMax = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//Maximum x for coordinate data.
    UNUSED(xMax);
    index += 2;

    qint16 yMax = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//Maximum y for coordinate data.
    UNUSED(yMax);
    index += 2;

    quint16 flags;
    //int len = 0;
    do
    {
        flags = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//()str.GetWord();
        index +=2;

/**************前面没什么用，用来占字节的*******************/
        quint16 glyphIndex = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();
        //qDebug() << "glyphIndex" << glyphIndex;

        qint32 curUnicode = 0;
        bool findFlag = false;
        int i = 0;
        QMap<quint16, quint16>::const_iterator iterator;
        for (iterator = olDunicodeAndGlyf.begin(); iterator != olDunicodeAndGlyf.end(); ++iterator)
        {
            if (glyphIndex == iterator.value())
            {
                curUnicode = iterator.key();
                findFlag = true;
                break;
            }
        }

        if (findFlag)
        {
            if (0 == unicodeAndGlyf.value(curUnicode)) //old file中有unicode映射，但是new file中没有
            {
                //glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.value(0x20)));
                for (i = 0; i < outMissedGlyf.count(); ++i)
                {
                    if (outMissedGlyf.at(i) == glyphIndex)
                    {
                        glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.count() + i));
                        break;
                    }
                }

                if ( i == outMissedGlyf.count())
                {
                    outMissedGlyf.append(glyphIndex);
                    glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.count() + i));
                }
            }
            else
            {
                glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.value(curUnicode)));
            }
        }
        else //old file中没有映射unicode 的字符
        {
            //glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.value(0x20)));
            /*统计丢失的字符，准备把这些字符添加都字库后面*/
            for (i = 0; i < outMissedGlyf.count(); ++i)
            {
                if (outMissedGlyf.at(i) == glyphIndex)
                {
                    glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.count() + i));
                    break;
                }
            }

            if ( i == outMissedGlyf.count())
            {
                outMissedGlyf.append(glyphIndex);
                glyfData.replace(index, 2, splitU16toByteArray(unicodeAndGlyf.count() + i));
            }
        }

        index +=2;
/************************后面的没什么用，用来占字节的***********************************/
        qint16 argument1;
        qint16 argument2;
        if ( flags & ARG_1_AND_2_ARE_WORDS)
        {
            argument1 = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();   //   (SHORT   or   FWord)   argument1;
            index +=2;

            argument2 = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();   //   (SHORT   or   FWord)   argument2;
            index +=2;
        }
        else
        {
            argument1 = (qint8)glyfData.at(index);//str.GetByte();
            index++;

            argument2 = (qint8)glyfData.at(index);//str.GetByte();
            index++;
        }

        qint16 xscale, yscale, scale01, scale10;
        xscale = 1;
        yscale = 1;
        scale01 = 0;
        scale10 = 0;
        UNUSED(scale01);
        UNUSED(yscale);
        UNUSED(scale10);

        if (flags & WE_HAVE_A_SCALE)
        {
            xscale = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();
            index +=2;

            yscale = xscale; //   Format   2.14
        }
        else if(flags & WE_HAVE_AN_X_AND_Y_SCALE)
        {
            xscale = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();
            index +=2;

            yscale = combineByteTo16(glyfData.at(index), glyfData.at(index + 1));//str.GetWord();
            index +=2;
        }
        else if(flags & WE_HAVE_A_TWO_BY_TWO)
        {
            xscale =  combineByteTo16(glyfData.at(index), glyfData.at(index + 1)); //str.GetWord();
            scale01 = combineByteTo16(glyfData.at(index), glyfData.at(index + 1)); //str.GetWord();
            scale10 = combineByteTo16(glyfData.at(index), glyfData.at(index + 1)); //str.GetWord();
            yscale = combineByteTo16(glyfData.at(index), glyfData.at(index + 1)); //str.GetWord();
        }

        if (flags & ARGS_ARE_XY_VALUES)
        {

            UNUSED(argument1);
            UNUSED(argument2);
#if 0
            XFORM xm;
            xm.eDx =  (float)argument1;
            xm.eDy =  (float)argument2;
            xm.eM11 = xscale /  float)16384.0;
            xm.eM12 = scale01 / (float)16384.0;
            xm.eM21 = scale10 / (float)16384.0;
            xm.eM22 = yscale / (float)16384.0;

            len   +=   DecodeGlyph(glyphIndex, curve, &xm);
#endif
        }
        else
        {
            qDebug () << "assert(false);";
        }

    }while(flags & MORE_COMPONENTS);

    if (flags & WE_HAVE_INSTRUCTIONS)
    {
        quint16 numInstr = glyfData.at(index);
        index++;

        for (quint16 i=0; i < numInstr; i++)
        {
            //str.GetByte();
            qDebug () << glyfData.at(index);
            index++;
        }
    }

}

void TtfModify::modifyHdmxTable(HdmxTable * hdmx, HdmxTable * oldHdmx, quint16 oldGlyf)
{
    /*创建 多个records*/
    if (hdmx->recordsList.isEmpty())
    {
        for (int i = 0; i < oldHdmx->recordsList.count(); ++i)
        {
            HdmxTable::HDMX_RECORDS_S *newRecords = new HdmxTable::HDMX_RECORDS_S;
            newRecords->pixelSize = oldHdmx->recordsList.at(i)->pixelSize;
            newRecords->maxWidth = oldHdmx->recordsList.at(i)->maxWidth;
            hdmx->recordsList.append(newRecords);
        }
        hdmx->version = oldHdmx->version;
        hdmx->numRecords = oldHdmx->numRecords;
    }

    for (int i = 0; i < hdmx->numRecords; ++i)
    {
        HdmxTable::HDMX_RECORDS_S *record = hdmx->recordsList.at(i);
        record->widths.append(oldHdmx->recordsList.at(i)->widths.at(oldGlyf));
    }

    hdmx->sizeDeviceRecord = hdmx->recordsList.at(0)->widths.count() + 2;

}

void TtfModify::modifyVmtxTable(VmtxTable * vmtx, VmtxTable * oldVmtx, quint16 oldGlyf)
{
    qint32 index = oldGlyf * 2;


    if (oldVmtx->topSideBearing.isEmpty())
    {
        qDebug () << "degerous bug" << __FUNCTION__;
        return;
    }

    if ((!oldVmtx->topSideBearing.isEmpty()) && (!oldVmtx->advanceHeight.isEmpty()))
    {
        QByteArray tempData = oldVmtx->topSideBearing.mid(index, 2);
        vmtx->topSideBearing.append(tempData);

        tempData = oldVmtx->advanceHeight.mid(index, 2);
        vmtx->advanceHeight.append(tempData);
    }
    else if((!oldVmtx->topSideBearing.isEmpty()) && oldVmtx->advanceHeight.isEmpty())
    {
        QByteArray tempData = oldVmtx->topSideBearing.mid(index, 2);
        vmtx->topSideBearing.append(tempData);
    }

}


QMap<quint16, bool> TtfModify::calculateExistUnicode(QByteArray & listViewMask, QMap < quint16, bool> & targetUnicodMap)
{
    QMap<quint16, bool> outMap;

    if (!targetUnicodMap.isEmpty())
    {
        QMap<quint16, bool>::const_iterator targetUnicodMapIterator;
        for (int i = 0; i < this->charsetMap.count(); ++i)
        {
            if (0 != listViewMask.at(i))
            {
                /*第几项*/
                QMap<quint16, bool> curCharset = this->charsetMap.value(charsetMap.keys().at(i));

                for (targetUnicodMapIterator = targetUnicodMap.begin(); targetUnicodMapIterator != targetUnicodMap.end();
                     ++targetUnicodMapIterator)
                {
                    qint32 curUnicode = targetUnicodMapIterator.key();
                    //qDebug ("first unicode:0x%x, last unicode:0x%x", curCharsetList.at(0), curCharsetList.at(curCharsetList.count() - 1 ));
                    if (curCharset.value(curUnicode))
                    {
                        outMap.insert(curUnicode, true);
                    }
                }
            }
        }
    }
    else //字符集裁剪
    {
        for (int i = 0; i < this->charsetMap.count(); ++i)
        {
            if (0 != listViewMask.at(i))
            {
                /*第几项*/
                QMap<quint16, bool> curCharset = this->charsetMap.value(charsetMap.keys().at(i));
                QMap<quint16, bool>::const_iterator curCharsetMapIterator;

                for (curCharsetMapIterator = curCharset.begin();
                     curCharsetMapIterator != curCharset.end();
                     ++curCharsetMapIterator)
                {
                    qint32 curUnicode = curCharsetMapIterator.key();
                    outMap.insert(curUnicode, true);
                }
            }
        }
    }

    return outMap;

}

void TtfModify::ttfFileCreate(QString fileName, QByteArray & listViewMask, QMap < quint16, bool> & targetUnicodMap)
{
    if (listViewMask.count() != this->charsetMap.count())
    {
        qDebug() << "hehe" << __FUNCTION__;
        return;
    }

    QMap<quint16, bool> existUnicodeMap = calculateExistUnicode(listViewMask, targetUnicodMap);
    this->fileCreate(fileName, existUnicodeMap);
}
/***************************************************************
*Description: 创建新的字库
*Input: fileName---新建文件的文件名，带路径
*       newFileUnicodeMap---新字库中的所有unicode
*Output: 无
*Return: 无
****************************************************************/
void TtfModify::fileCreate(QString & fileName, QMap<quint16, bool> & newFileUnicodeMap)
{
/*深度拷贝，因为数据后来都要释放*/
    TtfFile newTtfFile(*this->ttfFile);

    if (newFileUnicodeMap.isEmpty())
    {
        newTtfFile.showMessageBoxBadFile("save empty unicode!");
        return;
    }

#if 1
/*要改造的几个table，重新创建*/
    QMap <QString, TableEntryAbstract*>::iterator tableMapIterator;
    for (tableMapIterator = newTtfFile.ttfInfo->tableMap.begin(); tableMapIterator != newTtfFile.ttfInfo->tableMap.end(); ++tableMapIterator)
    {
        //qDebug() << tableMapIterator.key();
        QString tag = tableMapIterator.key();
        if (tag.contains("cmap") ||
            tag.contains("loca") ||
            tag.contains("glyf") ||
            tag.contains("hmtx") ||
            tag.contains("hdmx") ||
            tag.contains("vmtx"))
        {
            //qDebug() << tableMapIterator.key();
            delete tableMapIterator.value();

            TableEntryAbstract * newTableEntry = newTtfFile.createEntry(tag);
            if (NULL != newTableEntry)
            {
                memcpy(newTableEntry->tag, tag.toLatin1(), 4 );
                newTtfFile.ttfInfo->tableMap.insert(tableMapIterator.key(), newTableEntry);
            }
        }
    }
#endif

/*新建的ttf <unicode, glyfIndex> ，从1开始，从小到大依次排列
    0的位置是缺失字符*/
    qint32 index = 1;
    QMap<quint16, qint32> unicodeAndGlyf;
    QMap<quint16, bool>::const_iterator unicodeIterator;

    /*缺失的字符，方框*/
    unicodeAndGlyf.insert(0, 0);
    for (unicodeIterator = newFileUnicodeMap.begin(); unicodeIterator != newFileUnicodeMap.end(); ++unicodeIterator)
    {
        qint32 curUnicode = unicodeIterator.key();
        //qDebug () << "aaa" << curUnicode;
        unicodeAndGlyf.insert(curUnicode, index);
        index++;
    }

/*cmap*/
    CmapTable *cmap = (CmapTable*)newTtfFile.getTableEntry("cmap");
    CmapTable *oldCmap = (CmapTable*)this->ttfFile->getTableEntry("cmap");

    /*添加一个 unicode 的format4*/
    cmap->tableVersionNumber = oldCmap->tableVersionNumber;
    cmap->createFormat4AndAddToList(unicodeAndGlyf);

/*glyf*/
    GlyfTable *glyf = (GlyfTable*)newTtfFile.getTableEntry("glyf");
    GlyfTable *oldGlyf = (GlyfTable*)this->ttfFile->getTableEntry("glyf");

/*loca*/
    LocaTable *loca = (LocaTable*)newTtfFile.getTableEntry("loca");
    LocaTable *oldLoca = (LocaTable*)this->ttfFile->getTableEntry("loca");

    //oldLoca->debugInfo();
/*head*/
    HeadTable *head = (HeadTable*)newTtfFile.getTableEntry("head");
    HeadTable *oldHead = (HeadTable*)this->ttfFile->getTableEntry("head");

/*post*/
    PostTable *post = (PostTable*)newTtfFile.getTableEntry("post");
    PostTable *oldPost = (PostTable*)this->ttfFile->getTableEntry("post");
    post->changeToFormat2p0(unicodeAndGlyf.count());

/*maxp*/
    MaxpTable *maxp = (MaxpTable*)newTtfFile.getTableEntry("maxp");
    MaxpTable *oldMaxp = (MaxpTable*)this->ttfFile->getTableEntry("maxp");
    maxp->numGlyphs = unicodeAndGlyf.count();

/*hhea*/
    HheaTable *hhea = (HheaTable*)newTtfFile.getTableEntry("hhea");
    //HheaTable *oldHhea = (HheaTable*)this->ttfFile->getTableEntry("hhea");
    hhea->numberOfHMetrics = unicodeAndGlyf.count();

/*hmtx*/
    HmtxTable *hmtx = (HmtxTable*)newTtfFile.getTableEntry("hmtx");
    HmtxTable *oldHmtx = (HmtxTable*)this->ttfFile->getTableEntry("hmtx");

/*hdmx*/
    HdmxTable *hdmx = (HdmxTable*)newTtfFile.getTableEntry("hdmx");
    HdmxTable *oldHdmx = (HdmxTable*)this->ttfFile->getTableEntry("hdmx");

/*vhea*/
    VheaTable *vhea = (VheaTable*)newTtfFile.getTableEntry("vhea");

/*vmtx*/
    VmtxTable *vmtx = (VmtxTable*)newTtfFile.getTableEntry("vmtx");
    VmtxTable *oldVmtx = (VmtxTable*)this->ttfFile->getTableEntry("vmtx");

    if (NULL != oldVmtx)
    {
        oldVmtx->parseData(oldMaxp->numGlyphs);
    }

    /*地址起始*/
    loca->addGlyfOffset(head->indexToLocFormat, 0);

    QList<quint16> outMissGlyf;
    QMap<quint16, qint32>::const_iterator unicodeAndGlyfIterator;
    for (unicodeAndGlyfIterator = unicodeAndGlyf.begin(); unicodeAndGlyfIterator != unicodeAndGlyf.end(); ++unicodeAndGlyfIterator)
    {
        qint32 curUnicode = unicodeAndGlyfIterator.key();
        qint32 curGlyfIndex = unicodeAndGlyfIterator.value();

        quint16 oldGlyfIndex = this->unicodeGlyfIndexMap.value(curUnicode);
        //qDebug () << "curUnicode" << curUnicode << "oldglyfIndex" << glyfIndex << unicodeAndGlyfIterator.value();
        quint32 startAddr = oldLoca->getGlyfOffset(oldHead->indexToLocFormat, oldGlyfIndex);
        quint32 endAddr = oldLoca->getGlyfOffset(oldHead->indexToLocFormat, oldGlyfIndex + 1);
        //qDebug () << "startAddr" << startAddr << "endAddr"<< endAddr;

    /*字形数据*/
        QByteArray oneGlyf = oldGlyf->getOneGlyfData(startAddr, endAddr); //字形数据   
        /*复合字形处理*/
        if (!oneGlyf.isEmpty())
        {
            qint32 numberOfContours =  (qint16)combineByteTo16(oneGlyf.at(0), oneGlyf.at(1));
            if (-1 == numberOfContours) //复合字形
            {
                modifyCompositeGlyf(oneGlyf, unicodeAndGlyf, this->unicodeGlyfIndexMap, outMissGlyf);
            }
        }
        quint32 offsets = glyf->addGlyf(oneGlyf);

    /*loca数据*/
        loca->addGlyfOffset(head->indexToLocFormat, offsets);

    /*post*/
        QByteArray nameArray = oldPost->getGlyphName(oldGlyfIndex);
        if (nameArray.isEmpty())
        {
            quint32 nameIndex = oldPost->getGlyphNameIndex(oldGlyfIndex);
            post->addFormat2p0NameIndex(curGlyfIndex, nameIndex);
                    //qDebug () << "curGlyfIndex" << curGlyfIndex << "nameIndex" << nameIndex;
        }
        else
        {
            post->addFormat2p0NameArray(curGlyfIndex, nameArray);
           // qDebug("%x", curUnicode);
           // qDebug() << nameArray;
        }

    /*hmtx*/
        hmtx->addOneMertric(oldHmtx->getOneMertric(oldGlyfIndex));

     /*hdmx*/
        if (NULL != hdmx && NULL != oldHdmx)
        {
            modifyHdmxTable(hdmx, oldHdmx, oldGlyfIndex);
        }

    /*vmtx*/
        if (NULL != vmtx && NULL != oldVmtx)
        {
            modifyVmtxTable(vmtx, oldVmtx, oldGlyfIndex);
        }
    }

/**组合字形缺少的字形，再添加**/

    maxp->numGlyphs = unicodeAndGlyf.count() + outMissGlyf.count();
    if (NULL != hhea)
    {
        hhea->numberOfHMetrics = maxp->numGlyphs;
    }

    if (NULL != vhea)
    {
        vhea->numOfLongVerMetrics = maxp->numGlyphs;
    }

    post->changeNumberGlyphs(maxp->numGlyphs);

    for (int i = 0; i < outMissGlyf.count(); ++i)
    {
        quint16 oldGlyfIndex = outMissGlyf.at(i);
        quint16 curGlyfIndex = unicodeAndGlyf.count() + i;
        quint32 startAddr = oldLoca->getGlyfOffset(oldHead->indexToLocFormat, oldGlyfIndex);
        quint32 endAddr = oldLoca->getGlyfOffset(oldHead->indexToLocFormat, oldGlyfIndex + 1);
        QByteArray oneGlyf = oldGlyf->getOneGlyfData(startAddr, endAddr); //字形数据
    /*字形数据*/
        quint32 offsets = glyf->addGlyf(oneGlyf);

    /*loca数据*/
        loca->addGlyfOffset(head->indexToLocFormat, offsets);

    /*post*/
        QByteArray nameArray = oldPost->getGlyphName(oldGlyfIndex);
        if (nameArray.isEmpty())
        {
            quint32 nameIndex = oldPost->getGlyphNameIndex(oldGlyfIndex);
            post->addFormat2p0NameIndex(curGlyfIndex, nameIndex);
            //qDebug () << "curGlyfIndex" << curGlyfIndex << "nameIndex" << nameIndex;
        }
        else
        {
            post->addFormat2p0NameArray(curGlyfIndex, nameArray);
            //qDebug("%x", curUnicode);
            //qDebug() << nameArray;
        }

    /*hmtx*/
        hmtx->addOneMertric(oldHmtx->getOneMertric(oldGlyfIndex));

    /*hdmx*/
       if (NULL != hdmx && NULL != oldHdmx)
       {
           modifyHdmxTable(hdmx, oldHdmx, oldGlyfIndex);
       }
   /*vmtx*/
       if (NULL != vmtx && NULL != oldVmtx)
       {
           modifyVmtxTable(vmtx, oldVmtx, oldGlyfIndex);
       }
    }

    newTtfFile.ttfFileSave(fileName);
}
