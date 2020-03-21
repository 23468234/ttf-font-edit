#ifndef TTFTABLE_H
#define TTFTABLE_H

#include <QObject>
#include <QFile>
#include <QMap>
#include <ttfmodify.h>

/*字库文件头*/
#define TTF_HEAD_SIZE 12
typedef struct _ttf_head
{
    quint32   sfntversion;   //0x10000   for   version   1.0
    quint16   numTables;
    quint16   searchRange;
    quint16   entrySelector;
    quint16   rangeShift;
    bool combineTheByteToStruct(QByteArray data, struct _ttf_head * ttfHeadStruct);
}TTF_FILE_HEAD_S;


/*各个表的公共部分*/
#define TABLE_ENTYR_SIZE 16
class TableEntryAbstract
{
public:
    qint8 tag[4];
    quint32 checkSum;
    quint32 offset;
    quint32 length;
    TableEntryAbstract(){}
    virtual ~TableEntryAbstract(){}
    virtual void combineBytesToStruct(QByteArray & data) = 0;
    virtual QByteArray getDataForWriteFile() = 0;
    virtual void setDataOffsetInfile(qint32 offset) = 0;
};


class CmapTable; //cmap
class GlyfTable; //glyf
class HeadTable; //head
class HheaTable; //hhea
class HmtxTable; //hmtx
class LocaTable; //loca
class MaxpTable; //maxp
class NameTable; //name
class PostTable; //post
class Os2Table; //os/2
class HdmxTable; //hdmx
class VheaTable; //vhea
class VmtxTable; //vmtx
class UnknowTable; //others


/*camp表*/
class CmapTable : public TableEntryAbstract
{
public:
    typedef struct
    {
        quint16 PlatformID;
        quint16 encodingID; //Platform-specific encoding ID.
        quint32 byteOffset; // from beginning of table to thesubtable for this encoding.
        quint16 format;     // Format number is set to 0.
        quint16 length;     // This is the length in bytes of the subtable.
        quint16 version;    // Version number (starts at 0).
    }FORMAT_ENCODING_BASE_S;

    typedef struct
    {
        FORMAT_ENCODING_BASE_S base;
        qint8 glyphIdArray[256]; // An array that maps character codes to glyph index values.
    }FORMAT_0_ENCODING_S;

    typedef struct
    {
        FORMAT_ENCODING_BASE_S base;

        typedef struct
        {
            quint16 firstCode;  //First valid low byte for this subHeader.
            quint16 entryCount; //Number of valid low bytes for this subHeader.
            qint16 idDelta;     //See text below.
            quint16 idRangeOffset;// See text below.
        }SUB_HEAD_S;

        quint16 subHeaderKeys[256]; // Array that maps high bytes to
                                    //subHeaders: value is subHeader index * 8.
        QByteArray subHeaderAndGlyhArray;
        /*
        SUB_HEAD_S *subHeaders; // Variable-length array of subHeader structures.
        quint16 *glyphIndexArray;*/ // Variable-length array containing
                                 //subarrays used for mapping

    }FORMAT_2_ENCODING_S;

    typedef struct
    {
        FORMAT_ENCODING_BASE_S base;

        quint16 segCountX2; //2 x segCount.
        quint16 searchRange; //2 x (2**floor(log2(segCount)))
        quint16 entrySelector; //log2(searchRange/2)
        quint16 rangeShift; //2 x segCount - searchRange
        quint16 *endCount; //[segCount]; //End characterCode for each segment,last =0xFFFF.
        quint16 reservedPad; //Set to 0.
        quint16 *startCount; //[segCount]; //Start character code for each segment.
        qint16 *idDelta; //[segCount]; //Delta for all character codes in segment.

        QByteArray idRangeOffsetAndglyphIdArray; //两部分地址要连续存储
       // quint16 *idRangeOffset; //[segCount]; //Offsets into glyphIdArray or 0
        //quint16 *glyphIdArray;//Glyph index array (arbitrary length)

    }FORMAT_4_ENCODING_S;

    typedef struct
    {
        FORMAT_ENCODING_BASE_S base;
        quint16 firstCode; //First character code of subrange.
        quint16 entryCount; //Number of character codes in subrange.
        quint16 *glyphIdArray; //[entryCount]; //Array of glyph index values for character codes in the range.
    }FORMAT_6_ENCODING_S;

    CmapTable(){}
    virtual ~CmapTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    quint16 tableVersionNumber; // (0).
    quint16 numberOfEncodingTables; //Number of encoding tables, n.
    QList <FORMAT_ENCODING_BASE_S*> encodeList;

    CmapTable & operator= (CmapTable & table);
    FORMAT_ENCODING_BASE_S* createEncodeList(QByteArray encodeBase, QByteArray rawData);
    QMap<quint16, quint16> getUnicodeGlyfIndexMap();
    void createFormat4AndAddToList(QMap<quint16, qint32> & unicodeAndGlyf);

}; //camp


/*lgyf*/
class GlyfTable: public TableEntryAbstract
{
public:
    #define ARG_1_AND_2_ARE_WORDS       (1 << 0) //If this is set, the arguments are words; otherwise, they are bytes.
    #define ARGS_ARE_XY_VALUES          (1 << 1) //If this is set, the arguments are xy values; otherwise, they are points.
    #define ROUND_XY_TO_GRID            (1 << 2) //For the xy values if the preceding is true.
    #define WE_HAVE_A_SCALE             (1 << 3) //This indicates that there is a simple scale for the component. Otherwise, scale = 1.0.
    #define RESERVED                    (1 << 4) //This bit is reserved. Set it to 0.
    #define MORE_COMPONENTS             (1 << 5) //Indicates at least one more glyph after this one.
    #define WE_HAVE_AN_X_AND_Y_SCALE    (1 << 6) //The x direction will use a different scale from the y direction.
    #define WE_HAVE_A_TWO_BY_TWO        (1 << 7) //There is a 2 by 2 transformation that will be used to scale the component.
    #define WE_HAVE_INSTRUCTIONS        (1 << 8) //Following the last component are instructions for the composite character.
    #define USE_MY_METRICS              (1 << 9) //If set, this forces the aw and lsb (and rsb) for the composite to be equal to those from this original glyph. This works for hinted and unhinted characters.
    #define OVERLAP_COMPOUND            (1 << 10) //Used by Apple in GX fonts.
    #define SCALED_COMPONENT_OFFSET     (1 << 11) //Composite designed to have the component offset scaled (designed for Apple rasterizer).
    #define UNSCALED_COMPONENT_OFFSET   (1 << 12) //Composite designed not to have the component offset scaled (designed for the Microsoft TrueType rasterizer).

    QByteArray glyfData;
    GlyfTable(){}
    virtual ~GlyfTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    GlyfTable & operator= (GlyfTable & table);
    quint32 addGlyf(QByteArray  & data);
    QByteArray getOneGlyfData(quint32 startAddr, quint32 endAddr);
}; //glyf


/*head*/
class HeadTable : public TableEntryAbstract
{
public:
    qint32 tableVersionNumber;  // 0x00010000 for version 1.0.
    quint32 fontRevision;       // Set by font manufacturer.
    quint32 checkSumAdjustment; //To compute: set it to 0, sum the entire font
                                //as ULONG, then store 0xB1B0AFBA - sum.
    quint32 magicNumber; //Set to 0x5F0F3CF5
    quint16 flags; //Bit 0 - baseline for font at y=0;
                    //Bit 1 - left sidebearing at x=0;
                    //Bit 2 - instructions may depend on point size;
                    //Bit 3 - force ppem to integer values for all
                    //internal scaler math; may use fractional ppem
                    //sizes if this bit is clear;
                    //Bit 4 - instructions may alter advance width
                    //(the advance widths might not scale linearly);
                    //Note: All other bits must be zero.
    quint16 unitsPerEm; //Valid range is from 16 to 16384
    qint64 createdInternationalDate;    //(8-byte field).
    qint64 modifiedInternationalDate;   //(8-byte field).
    qint16 xMin; // For all glyph bounding boxes.
    qint16 yMin; // For all glyph bounding boxes.
    qint16 xMax; // For all glyph bounding boxes.
    qint16 yMax; // For all glyph bounding boxes.
    quint16 macStyle;   // Bit 0 bold (if set to 1); Bit 1 italic (if set to 1)
                        //Bits 2-15 reserved (set to 0).
    quint16 lowestRecPPEM;      //Smallest readable size in pixels.
    qint16 fontDirectionHint;   //0 Fully mixed directional glyphs;
                                //1 Only strongly left to right;
                                //2 Like 1 but also contains neutrals1;
                                //-1 Only strongly right to left;
                                //-2 Like -1 but also contains neutrals.
    qint16 indexToLocFormat;    // 0 for short offsets, 1 for long.
    qint16 glyphDataFormat;     // 0 for current format.

    HeadTable(){}
    virtual ~HeadTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    HeadTable & operator= (HeadTable & table);
}; //head


class HheaTable : public TableEntryAbstract
{
public:
    qint32 tableVersionNumber;  // 0x00010000 for version 1.0.
    qint16 Ascender;    //Typographic ascent.
    qint16 Descender;   //Typographic descent.
    qint16 LineGap;     //Typographic line gap. Negative
                        //LineGap values are treated as zero
                        //in Windows 3.1, System 6, and System 7.
    quint16 advanceWidthMax;    //Maximum advance width value in ‘hmtx’ table.
    qint16 minLeftSideBearing;  //Minimum left sidebearing value in ‘hmtx’ table.
    qint16 minRightSideBearing; //Minimum right sidebearing value;
                                //calculated as Min(aw - lsb - (xMax - xMin)).
    qint16 xMaxExtent;      //Max(lsb + (xMax - xMin)).
    qint16 caretSlopeRise;  //Used to calculate the slope of the cursor (rise/run); 1 for vertical.
    qint16 caretSlopeRun;   //0 for vertical.
    qint16 reserved1; //set to 0
    qint16 reserved2; //set to 0
    qint16 reserved3; //set to 0
    qint16 reserved4; //set to 0
    qint16 reserved5; //set to 0
    qint16 metricDataFormat;    //0 for current format.
    quint16 numberOfHMetrics;   //Number of hMetric entries in
                                //‘hmtx’ table; may be smaller than the
                                //total number of glyphs in the font.
    HheaTable(){}
    virtual ~HheaTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    HheaTable & operator= (HheaTable & table);
}; //hhea


class HmtxTable:public TableEntryAbstract
{
public:
    typedef struct _longHorMertric
    {
        quint16 advanceWidth;
        qint16 lsb;
    } LONG_HOR_MERTRIC_S;

    QList<LONG_HOR_MERTRIC_S*> metricList;  //numOfHMetrics comes from the ‘hhea’ table.
                                            //If the font is monospaced, only one entry
                                            //need be in the array, but that entry is required.
                                            //The last entry applies to all subsequent glyphs.
    HmtxTable(){}
    virtual ~HmtxTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    HmtxTable & operator= (HmtxTable & table);
    LONG_HOR_MERTRIC_S getOneMertric(quint16 glyfIndex);
    void addOneMertric(LONG_HOR_MERTRIC_S mertric);

}; //hmtx


class LocaTable:public TableEntryAbstract
{
public:
    QByteArray glyfOffsets;    //There are two versions of this table, the short and the long. The version is
                        //specified in the indexToLocFormat entry in the ‘head’ table.
                        //value of n is numGlyphs + 1. The value for numGlyphs is found in the ‘maxp’ table

    LocaTable(){}
    virtual ~LocaTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    LocaTable & operator= (LocaTable & table);
    quint32 getGlyfOffset(quint8 indexToLocFormat,quint16 glyphIndex); //head 表中indexToLocFormat确定数据的格式
    void addGlyfOffset(quint8 indexToLocFormat, quint32 offset); //head 表中indexToLocFormat确定数据的格式
    void debugInfo();
}; //loca


class MaxpTable:public TableEntryAbstract
{
public:
    qint32 tableVersionNumber;// 0x00010000 for version 1.0.
    quint16 numGlyphs;      // The number of glyphs in the font.
    quint16 maxPoints;      // Maximum points in a non-composite glyph.
    quint16 maxContours;    // Maximum contours in a noncompositeglyph.
    quint16 maxCompositePoints;     // Maximum points in a composite glyph.
    quint16 maxCompositeContours;   // Maximum contours in a composite glyph.
    quint16 maxZones;       // 1 if instructions do not use the twilight zone (Z0),
                            //or 2 if instructions do use Z0; should be set to 2 in most cases.
    quint16 maxTwilightPoints;  //Maximum points used in Z0.
    quint16 maxStorage;         //Number of Storage Area locations.
    quint16 maxFunctionDefs;    //Number of FDEFs.
    quint16 maxInstructionDefs; //Number of IDEFs.
    quint16 maxStackElements;   //Maximum stack depth2.
    quint16 maxSizeOfInstructions;  //Maximum byte count for glyph instructions.
    quint16 maxComponentElements;   //Maximum number of components referenced at “top level”
                                    //for any composite glyph.
    quint16 maxComponentDepth; //Maximum levels of recursion;

    MaxpTable(){}
    virtual ~MaxpTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    MaxpTable & operator= (MaxpTable & table);
}; //maxp


class NameTable:public TableEntryAbstract
{
public:
    QByteArray nameArray;

    NameTable(){}
    virtual ~NameTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    NameTable & operator= (NameTable & table);
}; //name


class PostTable:public TableEntryAbstract
{
public:
    #define FORMAT_1_0 0x10000
    #define FORMAT_2_0 0x20000
    #define FORMAT_2_5 0x25000
    #define FORMAT_3_0 0x30000
    struct
    {
        qint16 numberOfGlyphs;  // (this is the same as numGlyphs in ‘maxp’ table).
        QByteArray glyphNameIndex;
        QByteArray glyphNames;

        qint32 namesCount;
        /*
        qint16* glyphNameIndex;  //[numGlyphs].
        qint8* glyphNames;      // with length bytes [variable] (a Pascal string).
        */
    }format2p0;

    struct
    {
        qint16 numberOfGlyphs;
        QByteArray offset;
    }format2p5;

    quint32 Format;         //Type 0x00010000 for format 1.0, 0x00020000 for format 2.0, and so on...
    quint32 italicAngle;    //Italic angle in counter-clockwise degrees from the
                            //vertical. Zero for upright text, negative for text that leans to the right (forward)
    qint16 underlinePosition;   //Suggested values for the underline position
                                //(negative values indicate below baseline).
    qint16 underlineThickness;  //Suggested values for the underline thickness.
    quint32 isFixedPitch;   //Set to 0 if the font is proportionally spaced, nonzero
                            //if the font is not proportionally spaced (i.e.monospaced).
    quint32 minMemType42;   //Minimum memory usage when a TrueType font is downloaded.
    quint32 maxMemType42;   //Maximum memory usage when a TrueType font is downloaded.
    quint32 minMemType1;    //Minimum memory usage when a TrueType font is
                            //downloaded as a Type 1 font.
    quint32 maxMemType1;    //Maximum memory usage when a TrueType font is
                            //downloaded as a Type 1 font.

    PostTable(){}
    virtual ~PostTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);   


    PostTable & operator= (PostTable & table);
    void changeToFormat2p0(quint16 numberGlyphs);
    void changeNumberGlyphs(quint16 numberGlyphs);
    QByteArray getGlyphName(quint16 glyfIndex);
    qint32 getGlyphNameIndex(quint16 glyfIndex);
    QByteArray format2p0GetGlyphName(quint16 index);

    void addFormat2p0NameArray(quint16 glyfIndex, QByteArray & name);
    void addFormat2p0NameIndex(quint16 glyfIndex, qint16 index);

    void clear();
    void debugOutAllInfo();

}; //post


class Os2Table : public TableEntryAbstract
{
public:
    quint16 version;// 0x0001
    qint16 xAvgCharWidth;
    quint16 usWeightClass;
    quint16 usWidthClass;
    qint16 fsType;
    qint16 ySubscriptXSize;
    qint16 ySubscriptYSize;
    qint16 ySubscriptXOffset;
    qint16 ySubscriptYOffset;
    qint16 ySuperscriptXSize;
    qint16 ySuperscriptYSize;
    qint16 ySuperscriptXOffset;
    qint16 ySuperscriptYOffset;
    qint16 yStrikeoutSize;
    qint16 yStrikeoutPosition;
    qint16 sFamilyClass;
    quint8 panose[10];
    quint32 ulUnicodeRange1;// Bits 0–31
    quint32 ulUnicodeRange2;// Bits 32–63
    quint32 ulUnicodeRange3;// Bits 64–95
    quint32 ulUnicodeRange4;// Bits 96–127
    qint8 achVendID[4];
    quint16 fsSelection;
    quint16 usFirstCharIndex;
    quint16 usLastCharIndex;
    quint16 sTypoAscender;
    quint16 sTypoDescender;
    quint16 sTypoLineGap;
    quint16 usWinAscent;
    quint16 usWinDescent;

    /*version1 以及 以上*/
    quint32 ulCodePageRange1; //Bits 0-31
    quint32 ulCodePageRange2;// Bits 32-63

    /*下面的verson2 以及 以上才有*/
    qint16 sxHeight;
    qint16 sCapHeight;
    quint16 usDefaultChar;
    quint16 usBreakChar;

    /*version5 以及以上*/
    quint16 usMaxContext;
    quint16 usLowerOpticalPointSize;
    quint16 usUpperOpticalPointSize;

    Os2Table(){}
    virtual ~Os2Table();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    Os2Table & operator= (Os2Table & table);
}; //os/2


class HdmxTable : public TableEntryAbstract
{
public:
    typedef struct
    {
        quint8 pixelSize;// Pixel size for following widths (as ppem).
        quint8 maxWidth; // Maximum width.
        QByteArray widths;//[numGlyphs];// Array of widths (numGlyphs is from the 'maxp' table).
    }HDMX_RECORDS_S;

    quint16 version;
    qint16 numRecords;
    qint32 sizeDeviceRecord;
    QList <HDMX_RECORDS_S* > recordsList;

    HdmxTable(){}
    virtual ~HdmxTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    HdmxTable & operator= (HdmxTable & table);
};


class VheaTable : public TableEntryAbstract
{
public:
    quint32 version;    //Version number of the vertical header table; 0x00010000 for version 1.0
    qint16 ascent;      //Distance in FUnits from the centerline to the previous line’s descent.
    qint16 descent;     //Distance in FUnits from the centerline to the next line’s ascent.
    qint16 lineGap;     //Reserved; set to 0
    qint16 advanceHeightMax;// The maximum advance height measurement -in FUnits found in the font. This value must be consistent with the entries in the vertical metrics table.
    qint16 minTop;          //SideBearing The minimum top sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
    qint16 minBottom;       //SideBearing The minimum bottom sidebearing measurement found in the font,
                            //in FUnits. This value must be consistent with the entries in the vertical metrics table.
    qint16 yMaxExtent;      //Defined as yMaxExtent=minTopSideBearing+(yMax-yMin)
    qint16 caretSlopeRise;  //The value of the caretSlopeRise field divided by the value of the caretSlopeRun Field determines the slope of the caret. A value of 0 for the rise and a value of 1 for the run specifies a horizontal caret. A value of 1 for the rise and a value of 0 for the run specifies a vertical caret. Intermediate values are desirable for fonts whose glyphs are oblique or italic. For a vertical font, a horizontal caret is best.
    qint16 caretSlopeRun;   //See the caretSlopeRise field. Value=1 for nonslanted vertical fonts.
    qint16 caretOffset;     // The amount by which the highlight on a slanted glyph needs to be shifted away from the glyph in order to produce the best appearance. Set value equal to 0 for nonslanted fonts.
    qint16 reserved1;        //Set to 0.
    qint16 reserved2;        //Set to 0.
    qint16 reserved3;        //Set to 0.
    qint16 reserved4;        //Set to 0.
    qint16 metricDataFormat;        //Set to 0.
    quint16 numOfLongVerMetrics;    //Number of advance heights in the vertical metrics table.

    VheaTable(){}
    virtual ~VheaTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    VheaTable & operator= (VheaTable & table);
}; //vhea


class VmtxTable : public TableEntryAbstract
{
public:
    QByteArray data;
    QByteArray advanceHeight;
    QByteArray topSideBearing;

    VmtxTable(){}
    virtual ~VmtxTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    VmtxTable & operator= (VmtxTable & table);
    void parseData(quint16 numGlyphs);
};//vmtx


class UnknowTable:public TableEntryAbstract
{
public:
    QByteArray data;
    UnknowTable(){}
    virtual ~UnknowTable();
    virtual void combineBytesToStruct(QByteArray & data);
    virtual QByteArray getDataForWriteFile();
    virtual void setDataOffsetInfile(qint32 offset);

    UnknowTable & operator= (UnknowTable & table);
}; //unknow



/*一个ttf文件的数据*/
typedef struct
{
    TTF_FILE_HEAD_S ttfFileHead;
    QMap <QString, TableEntryAbstract*> tableMap;
}TTF_INFO_S;


class TtfFile
{
    Q_DECLARE_TR_FUNCTIONS(TtfFile);

public:
    TtfFile();
    TtfFile(TtfFile & ttfFile);
    ~TtfFile();
    TtfFile & operator= (TtfFile & ttfile);

    bool ttfFileOpen(QString fileName);
    void ttfFileSave(QString fileName);

    void newTtfFileCreate(QString & fileName, QMap<quint16, bool> & newFileUnicodeMap);
    TableEntryAbstract * getTableEntry(QString tag);
    void clear();

private:
    TTF_INFO_S * ttfInfo;
    void showMessageBoxBadFile(QString info);
    TableEntryAbstract* createEntry(QString tag);
    TableEntryAbstract* copyOneTable(QString tag, TableEntryAbstract * table);
    void createTableMap(QByteArray data, QMap<QString, TableEntryAbstract*> & map);
    qint32 readTableData(QFile & file, quint32 pos, QMap<QString, TableEntryAbstract*> & map);
    void ttfFileHeadSetTableNums(quint16 nums);

    friend class TtfModify;
};

#if defined (__cplusplus)
 extern "C"
 {
#endif
 QByteArray splitU16toByteArray(quint32 data);
 QByteArray splitU32toByteArray(quint32 data);
 quint16 combineByteTo16(quint8 byte0, quint8 byte1);
 quint32 combineByteTo32(quint8 byte0, quint8 byte1, quint8 byte2, quint8 byte3);

#if defined (__cplusplus)
}
#endif

#endif
