
#ifndef BMPPARSE_H
#define BMPPARSE_H

#include "common.h"
#include <QCoreApplication>

class BmpParse
{
    Q_DECLARE_TR_FUNCTIONS(BmpParse);

public:
    BmpParse();
    ~BmpParse();
    typedef struct
    {
        quint16  bfType;
        quint32  bfSize;
        quint16  bfReserved1;
        quint16  bfReserved2;
        quint32  bfOffBits;

        quint32  biSize;
        quint32  biWidth;
        quint32  biHeight;
        quint16  biPlanes;
        quint16  biBitCount;
        quint32  biCompression;
        quint32  biSizeImage;
        quint32  biXPelsPerMeter;
        quint32  biYPelsPerMeter;
        quint32  biClrUsed;
        quint32  biClrImportant;
    }__attribute__((gcc_struct, packed))BmpHeadInfo, *PBmpHeadInfo;

    qint32 openBmp(QString & fileName);
    quint32 getWidth();
    quint32 getHeight();
    quint32 getDrawWidth();
    quint8 getGrayThreshold();
    quint8 getBmpBitCount();
    void resetThreshold();
    void setGrayThreshold(quint8 );
    QList <quint8> getShapeData();
    QList <quint8> get8BitModel(); //一位表示八个像素， unicode字库模式

private:
    BmpHeadInfo bmpHead;
    quint8 grayThreshold;
    quint8 drawWidth; //字符绘制宽度
    quint16 unicode; //该字符的unicode
    QList <quint8> shapeInfo; //字符形状数据

private:
    quint32 fromLess8To256Gray(QString &fileName);
    quint32 from16ToGray(QString &fileName);
    quint32 from24or32ToGray(QString &fileName);

};

#endif
