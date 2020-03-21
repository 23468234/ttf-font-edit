

#include "bmpparse.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QObject>
#include <QDebug>


#define BMP_RGB 0 //没有压缩
#define RGBQUAD_SIZE 4 //调色板一个颜色字节数
#define DEFULT_THRESHOLD 156 //灰度阈值，越小黑点越多,构造函数中初始化

BmpParse::BmpParse()
{
    memset(&this->bmpHead, 0 , sizeof(BmpHeadInfo));
    this->shapeInfo.clear();
    this->resetThreshold();
}

BmpParse::~BmpParse()
{
    this->shapeInfo.clear();
}

/***************************************************************
*Description:
    用uchr 组成无符号整形
*Input:
    byte0 --- 第0字节
    byte1 --- 第1字节
    byte2 --- 第2字节
    byte2 --- 第3字节
*Output:
    无
*Return:
    组成的无符号整形值
****************************************************************/
inline quint32 makeuint32(quint8 byte0, quint8 byte1, quint8 byte2, quint8 byte3)
{
    return (byte0 + (byte1<<8) + (byte2<<16) + (byte3<<24));
}

/***************************************************************
*Description:
   GetColorNum：取得颜色表的个数
*Input:
    nBitCount --- 位图的象素颜色位数
    nClrUsed --- 位图信息中的biClrUsed参数值
*Output:
    无
*Return:
    颜色表的个数
****************************************************************/
inline qint32 GetColorNum(const qint16 nBitCount,const qint16 nClrUsed)
{
    if (nClrUsed)//颜色表参数不为0
    {
        return nClrUsed;
    }
    if (nBitCount < 16)//如果位图位数小于16且使用的颜色表参数为0
    {
        return 1 << nBitCount;
    }
    return 0;
}
/***************************************************************
*Description:
   bmp文件解析成灰度像素保存
*Input:
  fileName --- bmp图片的路径
*Output:
  无
*Return:
  <0 ---失败     其他---成功
****************************************************************/
qint32 BmpParse::openBmp(QString & fileName)
{
    qint64 readSize = 0;
    QFile file;

    if(fileName.isEmpty())
    {
        qDebug()<<("file name null\n");
        return FAIL;
    }

    file.setFileName(fileName);
    if (false == file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL, tr("File open"), tr("Bmp file open fail!"), \
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    readSize = file.read((char*)&this->bmpHead, sizeof(BmpHeadInfo));
    if (readSize < sizeof(BmpHeadInfo))
    {
        qDebug() << "Bad file!";
        QMessageBox::information(NULL, tr("Bmp parse"), tr("Bad file!"), \
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    file.close();

    //qDebug()<<"size:"<<sizeof(BmpHeadInfo);
    //qDebug()<<"width:"<<this->bmpHead.biWidth;
   // qDebug()<<"height:"<<this->bmpHead.biHeight;
   // qDebug()<<"bitcount"<<this->bmpHead.biBitCount;

    this->shapeInfo.clear();
    switch (this->bmpHead.biBitCount)
    {
        case 1:     //单色图像
        case 4:
        case 8:
        {
            return fromLess8To256Gray(fileName);
            break;
        }

        case 16:
        {
            return from16ToGray(fileName);
            break;
        }

        case 24:
        case 32:
        {
            return from24or32ToGray(fileName);
        }
        break;

        default:
        {
            QMessageBox::information(NULL, tr("biBitCount"), tr("Bmp bitcount not support yet!"), \
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return FAIL;
            break;
        }
     }
    return SUCCESS;

}
/***************************************************************
*Description:
    将8位及8位以下位图片转换成灰度
*Input:
    无
*Output:
    无
*Return:
    <0 --- 失败 其他 --- 成功
****************************************************************/
quint32 BmpParse::fromLess8To256Gray(QString &fileName)
{
    quint8 clrR = 0;
    quint8 clrG = 0;
    quint8 clrB = 0;
    quint8 nShift = 0;
    quint8 curValue = 0;
    quint32 grayLevel;
    quint32 row = 0;
    quint32 column = 0;
    qint64 readSize = 0;
    QByteArray clrBuf;
    QByteArray clrData;
    QByteArray pixData;
    QFile file;

    const qint32 nBytePel = 8 / this->bmpHead.biBitCount; //每字节的象素数
    const qint32 nBitCount = this->bmpHead.biBitCount; //每象素的位数
    const qint32 nSrcRowLen = ((this->bmpHead.biWidth + nBytePel -1)/nBytePel + 3) & (~3); //源图像每行象素字节数，每行像素数据对其为4的倍数
    const quint32 nClrNum = GetColorNum(this->bmpHead.biBitCount,this->bmpHead.biClrUsed); //颜色表个数

    if(fileName.isEmpty())
    {
        qDebug()<<("file name null\n");
        return FAIL;
    }

    file.setFileName(fileName);
    if (false == file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File open";
        return FAIL;
    }

    /*跳过文件头*/
    readSize = file.read((char*)&this->bmpHead, sizeof(BmpHeadInfo));
    if (readSize < sizeof(BmpHeadInfo))
    {
        qDebug() << "Bad file!";
        file.close();
        return FAIL;
    }

    if (this->bmpHead.bfType != 'B' + ('M' << 8))
    {
        QMessageBox::information(NULL, "File type", "Not bmp picture!", \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    /*读取调色板,并转化*/
    clrData.clear();
    for (row = 0; row < nClrNum; ++row)
    {
        clrBuf = file.read(RGBQUAD_SIZE);

        if (clrBuf.count() < RGBQUAD_SIZE)
        {
            qDebug()<<"Badfile"<<__FILE__<<"--->"<<__LINE__;
            file.close();
            return FAIL;
        }
        /*Gray = (R*299 + G*587 + B*114 + 500) / 1000*/
        clrR = clrBuf[0];
        clrG = clrBuf[1];
        clrB = clrBuf[2];
        grayLevel =  (clrR * 299 + clrG * 587 + clrB * 114 + 500)  / 1000;

        clrData.append((char)grayLevel);
    }

    for (row = 0; row < this->bmpHead.biHeight; ++row)
    {

        pixData.clear();
        pixData = file.read(nSrcRowLen);
        if (pixData.count() < nSrcRowLen)
        {
            qDebug()<<"Badfile"<<__FILE__<<"--->"<<__LINE__;
            file.close();
            return FAIL;
        }

        for (column = 0; column < this->bmpHead.biWidth; ++column)
        {
            curValue = pixData[column / nBytePel];			  //当前像素数据，在字符串中的字节

            nShift = 8 - (column % nBytePel +1) * nBitCount;  //相对该字节的偏移，
                                                              //4位时，偶；偏移4   奇 :偏移0
            curValue = ( ( ( (1 << nBitCount) - 1) << nShift ) & curValue) >> nShift; //8 位的是0XFF & data
                                                                                    //4位的偶 是((0xF << 4)&data)>>4
            this->shapeInfo.append(clrData[curValue]);
        }

    }
    file.close();
    return SUCCESS;
}
/***************************************************************
*Description:
    将16位图片转换成灰度
*Input:
    无
*Output:
    无
*Return:
    <0 --- 失败   其他 --- 成功
****************************************************************/
quint32 BmpParse::from16ToGray(QString &fileName)
{
    quint32 row = 0;
    quint32 column = 0;
    quint16 curPixel = 0;
    quint8 clrR = 0;
    quint8 clrG = 0;
    quint8 clrB = 0;
    quint8 grayLevel = 0;
    qint64 readSize = 0;
    QByteArray pixData;
    QFile file;

    const qint32 nPelBytes = this->bmpHead.biBitCount/8;           //每象素的字节数
    const qint32 nSrcRowLen = (nPelBytes * this->bmpHead.biWidth + 3) & (~3);//源图像每行字节数，每行像素数据对其为4的倍数

    //qDebug()<< "nSrcRowLen "<<nSrcRowLen;

    if(fileName.isEmpty())
    {
        qDebug()<<("file name null\n");
        return FAIL;
    }

    file.setFileName(fileName);
    if (false == file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File open";
        return FAIL;
    }

    /*跳过文件头*/
    readSize = file.read((char*)&this->bmpHead, sizeof(BmpHeadInfo));
    if (readSize < sizeof(BmpHeadInfo))
    {
        qDebug() << "Bad file!";
        file.close();
        return FAIL;
    }

    if (this->bmpHead.bfType != 'B' + ('M' << 8))
    {
        QMessageBox::information(NULL, "File type", "Not bmp picture!", \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    if (BMP_RGB != this->bmpHead.biCompression)
    {
        QMessageBox::information(NULL, "Compression", "Compressed bmp.Not support yet!", \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }


    for (row =0; row < this->bmpHead.biHeight; ++row)
    {
        pixData.clear();
        pixData = file.read(nSrcRowLen);
        if (pixData.count() < nSrcRowLen)
        {
            qDebug()<<"Badfile"<<__FILE__<<"--->"<<__LINE__;
            file.close();
            return FAIL;
        }

        for (column  = 0; column < this->bmpHead.biWidth; ++column)
        {
            curPixel = pixData[column * 2] + (pixData[column * 2 +1] << 8);
            clrB = curPixel & 0x001F;
            clrG = (curPixel & 0x03E0) >> 5;
            clrR = (curPixel & 0x7C00) >> 10;
            qDebug() << curPixel<< clrR <<clrG <<clrB;

            grayLevel = ((clrR << 3) * 299 + (clrG << 3) * 587 +  (clrB<< 3) * 114 + 500) /1000;

            this->shapeInfo.append(grayLevel);
        }
    }

    file.close();
    return SUCCESS;
}
/***************************************************************
*Description:
    24或32位图片转换成灰度
*Input:
    无
*Output:
    无
*Return:
    <0 --- 失败   其他 --- 成功
****************************************************************/
quint32 BmpParse::from24or32ToGray(QString &fileName)
{
    quint8 clrR = 0;
    quint8 clrG = 0;
    quint8 clrB = 0;
    quint8 grayLevel = 0;
    quint32 row = 0;
    quint32 column = 0;
    qint64 readSize = 0;
    QByteArray pixData;
    QFile file;
    const qint32 nPelBytes = this->bmpHead.biBitCount / 8; //每象素的字节数
    const qint32 nSrcRowLen = (nPelBytes * this->bmpHead.biWidth + 3) & (~3); //源图像每行象素字节数，每行像素数据对其为4的倍数

    if(fileName.isEmpty())
    {
        qDebug()<<("file name null\n");
        return FAIL;
    }

    file.setFileName(fileName);
    if (false == file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File open";
        return FAIL;
    }

    /*跳过文件头*/
    readSize = file.read((char*)&this->bmpHead, sizeof(BmpHeadInfo));

    if (this->bmpHead.bfType != 'B' + ('M' << 8))
    {
        QMessageBox::information(NULL, "File type", "Not bmp picture!", \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    if (BMP_RGB != this->bmpHead.biCompression)
    {
       QMessageBox::information(NULL, "Compression", "Compressed bmp.Not support yet!", \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return FAIL;
    }

    if (readSize < sizeof(BmpHeadInfo))
    {
        qDebug() << "Bad file!";
        file.close();
        return FAIL;
    }

    /*无压缩bmp，跳过了文件头，就是像素数据*/
    for (row = 0; row < this->bmpHead.biHeight; ++row)
    {

        pixData.clear();
        pixData = file.read(nSrcRowLen);
        if (pixData.count() < nSrcRowLen)
        {
            qDebug()<<"Badfile"<<__FILE__<<"--->"<<__LINE__;
            file.close();
            return FAIL;
        }

        for (column = 0; column < this->bmpHead.biWidth; ++column)
        {
            clrR = pixData[column * nPelBytes+2];
            clrG = pixData[column * nPelBytes+1];
            clrB = pixData[column * nPelBytes];
            grayLevel = (114 * clrB + 587 * clrG + 299 * clrR + 500) / 1000;//转换为灰度象素值

            this->shapeInfo.append(grayLevel);
        }
    }
    file.close();
    return SUCCESS;
}

/***************************************************************
*Description:
    讲bmp图片灰度值二值化，转换成unicode字库数据存储形式
*Input:
    无
*Output:
    无
*Return:
    QList<quint8> 转换后的unicode字库数据形式
****************************************************************/
QList<quint8>  BmpParse::get8BitModel()
{
    quint8 curByte = 0;
    quint8 mask = 0;
    quint32 curPixel = 0;
    quint32 curIndex = 0;
    qint32 row = 0;
    quint32 column = 0;
    quint32 oneLineByte = 0;
    QList<quint8> blackAndWhite;
    QList<quint8> outArray;

    if (this->shapeInfo.isEmpty())
    {
        return outArray;
    }

    for (row = 0; row < this->shapeInfo.count(); ++row)
    {
        if ( this->shapeInfo.at(row) > this->grayThreshold)
        {
            blackAndWhite.append(0);
        }
        else
        {
            blackAndWhite.append(1);
        }
    }

    oneLineByte = ((this->bmpHead.biWidth + 7) & (~7)) / 8;
    for (row = this->bmpHead.biHeight - 1; row >= 0; row--)
    {
        for (column = 0; column < oneLineByte; ++column)
        {
            curByte = 0;
            for (mask = 0; mask < 8; ++mask)
            {
                if (8 * column + mask >= this->bmpHead.biWidth)
                {
                    break;
                }

                curIndex = bmpHead.biWidth * row + 8 * column + mask;
                //qDebug() <<"row"<<row<< "column "<<column<< "mask "<<mask << "index "<<curIndex;
                curPixel = blackAndWhite[curIndex];
                curByte = curByte|(curPixel << (7 - mask));
            }
            outArray.append(curByte);
        }
    }

    return outArray;
}

/***************************************************************
*Description:
    获取图片像素宽度
*Input:
    无
*Output:
    无
*Return:
    图片像素宽度
****************************************************************/
quint32 BmpParse::getWidth()
{
    return this->bmpHead.biWidth;
}

quint32 BmpParse::getHeight()
{
    return this->bmpHead.biHeight;
}

quint32 BmpParse::getDrawWidth()
{
    return this->bmpHead.biWidth;//this->drawWidth;
}

QList<quint8> BmpParse::getShapeData()
{
    return this->shapeInfo;
}

void BmpParse::resetThreshold()
{
    this->grayThreshold = DEFULT_THRESHOLD;
}

void BmpParse::setGrayThreshold(quint8 grayThreshold)
{
    this->grayThreshold = grayThreshold;
}

quint8 BmpParse::getGrayThreshold()
{
    return this->grayThreshold;
}

quint8 BmpParse::getBmpBitCount()
{
    return this->bmpHead.biBitCount;
}




















