

#include "fontwidget.h"
#include <QPainter>
#include <QtDebug>
#include <QDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QMouseEvent>

#define BMP_PIXEL_WIDTH_MAX 32
#define BMP_PIXEL_HEIGHT_MAX 32
#define FONT_VIEW1_WIDTH 64 //64 * 64 预览 宽度
#define FONT_VIEW1_HEIGHT 64
#define FONT_VIEW2_WIDTH 32 //32 * 32预览宽度
#define FONT_VIEW2_HEIGHT 32


FontWidget::FontWidget(QWidget *parent) :
    QWidget(parent)
{
    this->shapeWidth = BMP_PIXEL_WIDTH_MAX;
    this->shapeHeight = BMP_PIXEL_HEIGHT_MAX;
    this->drawWidth = 0;
    this->shapeData.clear();
    this->clearMoveStepDatas();

    this->buttonUp = new QPushButton("↑", this);
    this->buttonDown = new QPushButton("↓", this);
    this->buttonLeft = new QPushButton("←", this);
    this->buttonRight = new QPushButton("→", this);

    validator = new QIntValidator(1, 32, this);
    this->lineEdit = new QLineEdit("0", this);
    this->lineEdit->setValidator(validator);
    this->lineEdit->setAlignment(Qt::AlignCenter);
    this->lineEdit->setDragEnabled(true);

    connect(this->buttonUp, SIGNAL(clicked()), this, SLOT(moveToUp()));
    connect(this->buttonDown, SIGNAL(clicked()), this, SLOT(moveToDown()));
    connect(this->buttonLeft, SIGNAL(clicked()), this, SLOT(moveToLeft()));
    connect(this->buttonRight, SIGNAL(clicked()), this, SLOT(moveToRight()));
    connect(this->lineEdit, SIGNAL(returnPressed()),this, SLOT(loseTheEditFocus()));
    connect(this->lineEdit, SIGNAL(textChanged(QString)),this, SLOT(inputDrawWidth()));

   //qDebug()<< "Mylabel";

}

FontWidget::~FontWidget()
{
    //qDebug()<< "~Mylabel";
    this->shapeData.clear();
    delete this->buttonDown;
    delete this->buttonUp;
    delete this->buttonLeft;
    delete this->buttonRight;
    delete this->validator;
    delete this->lineEdit;
}

/***************************************************************
*Description:
    paintEvent() 字库点阵控件绘制函数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::paintEvent(QPaintEvent * event)
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 posx = 0;
    qint32 posy = 0;

    qint32 matrixWidth = 0;
    qint32 matrixHeight = 0;
    qint32 ctrWidth = 0;
    qint32 ctrHeight = 0;

    qint32 curIndexPixel = 0;
    qint32 view1Posx = 0;
    qint32 view1Posy = 0;
    qint32 view2Posx = 0;
    qint32 view2Posy = 0;
    qint32 onelinebyte = 0;
    qint32 index = 0;


    const QBrush brushDark("black", Qt::SolidPattern);
    const QBrush brushgray("lightgray", Qt::SolidPattern);
    const QBrush brushWhite("white", Qt::SolidPattern);
    const QPen penWhite(brushWhite, qreal(1), Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    const QPen penBlack(brushDark, qreal(1), Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);

    event = event;

    ctrWidth = (this->size().rwidth() / BMP_PIXEL_WIDTH_MAX) * BMP_PIXEL_WIDTH_MAX;
    ctrHeight = (this->size().rheight() - 20) / BMP_PIXEL_HEIGHT_MAX * BMP_PIXEL_HEIGHT_MAX;

    /*找一个最大正方形区域， heighnt = width + 64*/
    if (ctrHeight > ctrWidth + 64)
    {
        matrixWidth = ctrWidth;
        matrixHeight = matrixWidth;//ctrWidth + 64;
    }
    else
    {
        matrixWidth = ctrHeight - 64;
        matrixHeight = matrixWidth;
    }

    /*计算点阵每个点的长宽，正方形*/
    this-> dotWidth = matrixWidth / BMP_PIXEL_WIDTH_MAX;
    this-> dotHeight= this-> dotWidth;

    /*外框*/
    QPainter painter;
    painter.begin(this);
    painter.setPen(penBlack);
    painter.drawRect(0,0, matrixWidth + 1, matrixHeight + 64 + 2);

    /*画预览边框*/
    painter.setPen(penBlack);
    painter.drawLine(0, matrixHeight + 1, matrixWidth, matrixHeight + 1);
    painter.drawRect(0, matrixHeight + 1, FONT_VIEW1_WIDTH + 1, FONT_VIEW1_HEIGHT + 1);
    painter.drawRect(FONT_VIEW1_WIDTH + 1, matrixHeight + 1, FONT_VIEW2_WIDTH + 1, FONT_VIEW2_WIDTH + 1);

    painter.setPen(penWhite);

    if (0 != this->shapeHeight)
    {
        onelinebyte = this->shapeData.count() / this->shapeHeight;;
    }

    /*画点阵*/
    for (row = 0; row < this->shapeHeight; row++)
    {
        if (row >= BMP_PIXEL_HEIGHT_MAX)
        {
            break;
        }

        for (column = 0; column < this->shapeWidth; column++)
        {

            if (column >= BMP_PIXEL_WIDTH_MAX)
            {
                break;
            }

            if (this->shapeData.isEmpty())  //数据是空的
            {
                curIndexPixel = 0;//painter.setBrush(brushgray);
            }
            else
            {

                index = column / 8 + row * onelinebyte;
                if (index > shapeData.count())
                {
                    qDebug()<<"Dangerous Bug!!!!!!";
                     break;
                }
                if(this->shapeData.at(index) & (1 << (7 - (column % 8) ) ))
                {
                    curIndexPixel = 1;//painter.setBrush(brushDark);    // 该位为1的时候就画黑块
                }
                else
                {
                    curIndexPixel = 0;//painter.setBrush(brushgray);   //为0画白块
                }
            }

            /*画点阵*/
            posx = column * this-> dotWidth + 1;
            posy = row * this-> dotHeight + 1;
            painter.setPen(penWhite);
            if (1 == curIndexPixel)
            {
                painter.setBrush(brushDark);
            }
            else
            {
                painter.setBrush(brushgray);
            }
            painter.drawRect(posx, posy, this-> dotWidth - 1, this-> dotHeight - 1);

            /*画预览64 * 64*/
            if (1 == curIndexPixel)
            {
                painter.setPen(penBlack);
            }
            else
            {
                painter.setPen(penWhite);
            }
            view1Posx = 2 * column + 1;
            view1Posy = matrixHeight + 2 + 2 * row;
            painter.drawPoint(view1Posx, view1Posy); //画四个点
            painter.drawPoint(view1Posx, view1Posy + 1);
            painter.drawPoint(view1Posx + 1, view1Posy);
            painter.drawPoint(view1Posx + 1, view1Posy + 1);


            /*画预览 32 *32 */
            view2Posx = column + FONT_VIEW1_WIDTH + 2;
            view2Posy = matrixHeight + 2 + row;
            painter.drawPoint(view2Posx, view2Posy);
        }
    }

    /*显示字符绘制宽度*/
    qint32 strWidth = 0;
    QFont font(QString::fromUtf8("simsun"), 10, QFont::Normal, false);
    //font.setFamily(("simsun"));
    painter.setFont(font);
    painter.setPen(Qt::black);
    strWidth = painter.fontMetrics().width(tr("绘制宽度"), -1); //获取”绘制宽度“这几个字的宽度，用来居中
    painter.drawText((matrixWidth - strWidth) / 2, matrixHeight + 27, tr("绘制宽度"));
    painter.end();

    /*控件坐标*/
    this->buttonUp->setGeometry(QRect(matrixWidth - 64, matrixHeight + 2, 25, 25));
    this->buttonDown->setGeometry(QRect(matrixWidth - 64, matrixHeight + 35, 25, 25));
    this->buttonLeft->setGeometry(QRect(matrixWidth - 100, matrixHeight + 35, 25, 25));
    this->buttonRight->setGeometry(QRect(matrixWidth - 28, matrixHeight + 35, 25, 25));
    this->lineEdit->setGeometry(QRect((matrixWidth - 75) / 2, matrixHeight + 35, 75, 25));
}


/***************************************************************
*Description:
    mousePressEvent() 鼠标点击事件
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::mousePressEvent(QMouseEvent * event)
{
    //qDebug() << "pos :" << event->pos().rx() << event->pos().ry();


    if (this->shapeData.isEmpty())
    {
        return;
    }

    if (event->pos().rx() >=0 &&
        event->pos().rx() <= this-> dotWidth * BMP_PIXEL_WIDTH_MAX &&
        event->pos().ry() >= 0 &&
        event->pos().ry() <= this-> dotHeight * BMP_PIXEL_HEIGHT_MAX)
    {
        if (event->button() == Qt::LeftButton)
        {
            setOnePixel(event->pos().ry() / this-> dotHeight, event->pos().rx() / this-> dotWidth);
        }
        else if (event->button() == Qt::RightButton)
        {
            resetOnePixel(event->pos().ry() / this-> dotHeight, event->pos().rx() / this-> dotWidth);
        }
    }
}

/***************************************************************
*Description:
    mousePressEvent() 鼠标点击事件
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (this->shapeData.isEmpty())
    {
        return;
    }
    //qDebug() << "pos :" << event->pos().rx() << event->pos().ry();
    if (event->pos().rx() >= 0 &&
        event->pos().rx() <= this-> dotWidth * BMP_PIXEL_WIDTH_MAX &&
        event->pos().ry() >= 0 &&
        event->pos().ry() <= this-> dotHeight * BMP_PIXEL_HEIGHT_MAX)
    {
        if (event->buttons() & Qt::LeftButton )
        {
            //qDebug() << "left move";
            setOnePixel(event->pos().ry() / this-> dotHeight, event->pos().rx() /this-> dotWidth);
        }
        else if (event->buttons() & Qt::RightButton )
        {
            //qDebug() << "right move";
            resetOnePixel(event->pos().ry() / this-> dotHeight, event->pos().rx() /this-> dotWidth);
        }
    }
}

/***************************************************************
*Description:
    setOnePixel() 点阵数据置1
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::setOnePixel(int row, int column)
{
    qint32 index = 0;
    qint32 bytePerColumn = 0;

    /*错误的坐标*/
    if (row >= this->shapeHeight || column >= this->shapeWidth)
    {
        return;
    }
    //qDebug()  << row <<" "<< column;
    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;
    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    index = bytePerColumn * row + column / 8;
    if (index >= this->shapeData.count())
    {
        qDebug() << "Degerous bug" << __FUNCTION__ << __LINE__;
        return;
    }

    this->shapeData[index] |= (1 << (7 - (column % 8)));
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->clearMoveStepDatas();
    this->calculateMoveSteps();
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();
}
/***************************************************************
*Description:
    resetOnePixel() 点阵数据置0
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::resetOnePixel(int row, int column)
{
    qint32 index = 0;
    qint32 bytePerColumn = 0;

    /*错误的坐标*/
    if (row >= this->shapeHeight || column >= this->shapeWidth)
    {
        return;
    }
    //qDebug()  << row <<" "<< column;

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;
    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    index = bytePerColumn * row + column / 8;
    if (index >= this->shapeData.count())
    {
        qDebug() << "Degerous bug" << __FUNCTION__ << __LINE__;
        return;
    }

    this->shapeData[index] &= ~(1 << (7 - (column % 8)));
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->clearMoveStepDatas();
    this->calculateMoveSteps();
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();
}

/***************************************************************
*Description:
    getPartOfShape() 字库点阵 的部分形状数据
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
QList<quint8> FontWidget::getPartOfShape(qint32 width, qint32 height)
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 curLineBytes = 0;
    qint32 outLineBytes = 0;
    QList<quint8> outArray;

    if (this->shapeData.isEmpty())
    {
        return outArray;
    }

    if (0 != this->shapeHeight)
    {
        curLineBytes = this->shapeData.count() / this->shapeHeight;;
    }

    outLineBytes = ((width + 7) & (~7)) / 8; //数据是补齐为8的整数倍的


    /*width * height 大小数据*/
    for (row = 0; row < height; ++row)
    {
         for (column = 0; column < outLineBytes; ++column)
         {

             if (row >= this->shapeHeight)
             {
                outArray.append((quint8)0);
                continue;
             }

             if (column >= curLineBytes)
             {
                outArray.append((quint8)0);
                continue;
             }

             index = curLineBytes * row + column;
             if (index > this->shapeData.count())
             {
                 qDebug()<<"Dangerous Bug!!!!!!";
                  break;
             }
             outArray.append(this->shapeData.at(index));

         }
    }

    return outArray;
}

/***************************************************************
*Description:
    modifyInputCodeChar() 改变字库输入的inputcode信息
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
qint32 FontWidget::calculateDrawWidth(QList<quint8> shapeData, qint32 width, qint32 height)
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    quint8 mask = 0;
    quint8 count = 0;
    qint32 bytesPerLine = 0;
    qint32 curDrawWidth = 0;
    qint32 drawWidthBk = 0;

    bytesPerLine = ((width + 7) & (~7))/8;

    if (bytesPerLine * height != shapeData.count())
    {
        qDebug()<<"A strange error"<<__FUNCTION__;
        return width;
    }

    for (row = 0; row < height ; ++row)
    {
        curDrawWidth = 0;
        for (column = 0; column < bytesPerLine; ++column)
        {
            index = bytesPerLine * row + column;
            if (index > shapeData.count())
            {
                qDebug() << "Dangerous bug: " << __FUNCTION__;
                return width;
            }

            mask = 0x80;
            for (count = 0; count < 8; ++count)
            {
                if (shapeData[index] & mask)
                {
                    curDrawWidth = 8 * column + count + 1;
                }
                mask = mask >> 1;
            }
        }

        if (drawWidthBk < curDrawWidth)
        {
            drawWidthBk = curDrawWidth;
        }
    }

    //qDebug()<<"draw width"<<drawWidthBk;
    return drawWidthBk;
}

/***************************************************************
*Description:
    moveToLeft() 数据整个向左移动一步
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::moveToLeft()
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 bytePerColumn = 0;
    quint32 curRowData = 0;
    quint8 curByteData = 0;
    QList<quint8> newArray;
    //qDebug() << "move moveToLeft";

    if (this->HDirectionMoveStep <= (-this->leftCanMoveStep))
    {
        return ;
    }

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;

    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    for (row = 0; row < this->shapeHeight; ++row)
    {
        curRowData = 0;
        for (column = 0; column < bytePerColumn; ++column)
        {
            index = bytePerColumn * row + column;
            if (index >= this->shapeData.count())
            {
                qDebug() << "Strange error";
                return ;
            }

            curRowData = (curRowData << 8) + this->shapeData[index];
        }

        curRowData = curRowData << 1; //左移

        for (column = bytePerColumn - 1; column >= 0 ; --column)
        {
            curByteData = (curRowData >> (column * 8)) & 0xFF;
            newArray << curByteData;
        }
    }
    this->HDirectionMoveStep--;
    this->shapeData = newArray;
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();
}

/***************************************************************
*Description:
     moveToRight() 数据整个向右移动一步
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::moveToRight()
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 bytePerColumn = 0;
    quint32 curRowData = 0;
    quint8 curByteData = 0;
    QList<quint8> newArray;

    //qDebug() << "move right";

    if (this->HDirectionMoveStep >= this->rightCanMoveStep)
    {
        return ;
    }

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;

    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    for (row = 0; row < this->shapeHeight; ++row)
    {
        curRowData = 0;
        for (column = 0; column < bytePerColumn; ++column)
        {
            index = bytePerColumn * row + column;
            if (index >= this->shapeData.count())
            {
                qDebug() << "Strange error";
                return ;
            }

            curRowData = (curRowData << 8) + this->shapeData[index];
        }

        curRowData = curRowData >> 1; //左移

        for (column = bytePerColumn - 1; column >= 0 ; --column)
        {
            curByteData = (curRowData >> (column * 8)) & 0xFF;
            newArray << curByteData;
        }
    }
    this->HDirectionMoveStep++;
    this->shapeData = newArray;
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();
}
/***************************************************************
*Description:
     moveToRight() 数据整个向上移动一步
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::moveToUp()
{
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 bytePerColumn = 0;
    QList<quint8> newArray;

   // qDebug() << "move right";

    if (this->VDirectionMoveStep <= (-this->upCanMoveStep))
    {
        return ;
    }

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;

    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    for (row = 1; row < this->shapeHeight; ++row)
    {
        for (column = 0; column < bytePerColumn; ++column)
        {
            index = bytePerColumn * row + column;
            if (index >= this->shapeData.count())
            {
                qDebug() << "Strange error";
                return ;
            }
            newArray << this->shapeData[index];
        }
    }

    for (column = 0; column < bytePerColumn; ++column)
    {
        newArray << 0;
    }
    this->VDirectionMoveStep--;
    this->shapeData = newArray;
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();

}
/***************************************************************
*Description:
     moveToRight() 数据整个向下移动一步
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::moveToDown()
{

    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 bytePerColumn = 0;
    QList<quint8> newArray;

    //qDebug() << "move right";
    if (this->VDirectionMoveStep >= this->downCanMoveStep)
    {
        return ;
    }

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;

    if (bytePerColumn > 4 )
    {
        QMessageBox::critical(NULL, "Dot matrix", "The width is more than 32!", \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    for (column = 0; column < bytePerColumn; ++column)
    {
        newArray << 0;
    }

    for (row = 0; row < this->shapeHeight - 1; ++row)
    {
        for (column = 0; column < bytePerColumn; ++column)
        {
            index = bytePerColumn * row + column;
            if (index >= this->shapeData.count())
            {
                qDebug() << "Strange error";
                return ;
            }
            newArray << this->shapeData[index];
        }
    }
    this->VDirectionMoveStep++;
    this->shapeData = newArray;
    this->drawWidth = calculateDrawWidth(this->shapeData, this->shapeWidth, this->shapeHeight);
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
    this->update();
}

void FontWidget::inputDrawWidth()
{

    bool ok = false;
    quint32 curDrawWidth;
    QString drawWidthString;

    curDrawWidth = this->lineEdit->text().toInt(&ok, 10);
    if (true == ok)
    {
        this->drawWidth = curDrawWidth;
    }

    //qDebug()<<"setValue";

}

void FontWidget::loseTheEditFocus()
{
    this->lineEdit->clearFocus();
}
/***************************************************************
*Description:
    calculateMoveSteps() 计算这个字形各个方向可移动的步数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::calculateMoveSteps()
{
    qint8 curData = 0;
    qint8 mask = 0;
    qint32 row = 0;
    qint32 column = 0;
    qint32 index = 0;
    qint32 bytePerColumn = 0;
    /*计算右移*/
    qint32 calculateRight = 0;
    qint32 calculateRightBk = 0;
    /*计算上移*/
    qint32 toCalculateUpFlag = 0;
    qint32 calculateUp = 0;
    /*计算左移*/
    qint32 toCalculateLeftFlag = 0;
    qint32 calculateLeft = -1;
    qint32 calculateLeftBk = -1;
    /*计算下移*/
    qint32 calculateDown = 0;

    bytePerColumn = ((this->shapeWidth + 7) & (~7))/8;

    for (row = 0; row < this->shapeHeight; row++)
    {
        toCalculateLeftFlag = 0;
        calculateLeft = -1;
        for (column = 0; column < bytePerColumn; ++column)
        {
            index = bytePerColumn * row + column;
            if (index >= this->shapeData.count())
            {
                qDebug()<<"Strange error:"<<__FUNCTION__;
                return ;
            }
            curData = this->shapeData[index];

            for (mask = 7; mask >= 0; --mask)
            {
                if (curData & (1 << mask))
                {
                    /*记录当前行最右边的点，计算向右可以移动的步数*/
                    calculateRight = 8 * column + 8 - mask;

                    /*记录第一次出现黑点的行数, 计算向上可以移动的步数*/
                    if (0 == toCalculateUpFlag)
                    {
                        toCalculateUpFlag = 1;
                        calculateUp = row;
                    }

                    /*记录当前行第一次出现黑点的位置，为了计算可以向左移动的步数*/
                    if (0 == toCalculateLeftFlag)
                    {
                        toCalculateLeftFlag = 1;
                        calculateLeft = 8 * column + 7 - mask;
                    }

                    calculateDown = row + 1; //最后出现黑点的行
                }
            }

        }

        /*找每行中最右边的点，记录最大的*/
        if (calculateRightBk < calculateRight)
        {
            calculateRightBk = calculateRight;
        }

        /*找每行中最左边的点，记录最小*/
        if (-1 != calculateLeft)
        {

            if (calculateLeftBk > calculateLeft || calculateLeftBk == -1)
            {
                calculateLeftBk = calculateLeft;
            }
        }
    }

    this->leftCanMoveStep = calculateLeftBk;
    this->rightCanMoveStep = this->shapeWidth - calculateRightBk; //可以向右移动的步数
    this->upCanMoveStep = calculateUp; //向上可以移动的步数
    this->downCanMoveStep = this->shapeHeight - calculateDown;

    //qDebug()<< "calculateDown" << this->downCanMoveStep;
    //qDebug() << "right can move step"<< this->rightCanMoveStep;
}

void FontWidget::clearMoveStepDatas()
{
    this->leftCanMoveStep = 0; //可以向右移动多少步
    this->rightCanMoveStep = 0; //可以向右移动多少步
    this->upCanMoveStep = 0; //可以向上移动多少步
    this->downCanMoveStep = 0; //可以向下移动多少步

    this->HDirectionMoveStep = 0; //水平方向移动操作， 正代表右移
    this->VDirectionMoveStep = 0; //竖直方向移动操作，正代表下移
}

/***************************************************************
*Description:
    setWidth() 保存当前显示数据的，横向点阵个数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void FontWidget::setWidth(qint32 width)
{
    this->shapeWidth = width;

    /*设置字符宽度输入上下限*/
    delete this->validator;
    this->validator = new QIntValidator(1, width, this);
    this->lineEdit->setValidator(this->validator);
}

void FontWidget::setHight(qint32 height)
{
    this->shapeHeight = height;
}

void FontWidget::setDrawWidth(qint32 drawWidth)
{
    this->drawWidth = drawWidth;
    this->lineEdit->setText(QString("%1").arg(this->drawWidth));
}

void FontWidget::setShapeArray(QList<quint8> data)
{
    qint32 width = this->shapeWidth;
    qint32 height = this->shapeHeight;

    if (this->shapeWidth > BMP_PIXEL_WIDTH_MAX)
    {
        width = BMP_PIXEL_WIDTH_MAX;
    }
    if (this->shapeHeight > BMP_PIXEL_HEIGHT_MAX)
    {
       height= BMP_PIXEL_HEIGHT_MAX;
    }

    this->shapeData = data;
    this->shapeData = this->getPartOfShape(width , height);
    this->shapeWidth = width;
    this->shapeHeight = height;
    this->drawWidth = calculateDrawWidth(this->shapeData, width, height);
    this->calculateMoveSteps();
}

qint32 FontWidget::getWidth()
{
    return this->shapeWidth;
}

qint32 FontWidget::getHeight()
{
    return this->shapeHeight;
}

qint32 FontWidget::getDrawWidth()
{
    return this->drawWidth;
}

QList <quint8> FontWidget::getShapeData()
{
    return this->shapeData;
}

void FontWidget::clear()
{
    this->shapeWidth = BMP_PIXEL_WIDTH_MAX;
    this->shapeHeight = BMP_PIXEL_HEIGHT_MAX;
    this->drawWidth = 0;
    this->shapeData.clear();
    this->clearMoveStepDatas();
    this->update();
}
