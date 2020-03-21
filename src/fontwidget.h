
#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>

class FontWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FontWidget(QWidget *parent);
    ~FontWidget();

    void setWidth(qint32 width);
    void setHight(qint32 height);
    void setDrawWidth(qint32 drawWidth);
    void setShapeArray(QList<quint8> data);
    void clear();
    qint32 getWidth();
    qint32 getHeight();
    qint32 getDrawWidth();
    QList<quint8> getShapeData();
    QList<quint8> getPartOfShape(qint32 width, qint32 height);
    qint32 calculateDrawWidth(QList<quint8> shapeData, qint32 width, qint32 height);

    /*移动相关*/
    void calculateMoveSteps();
    void clearMoveStepDatas();

private:
    /*基本信息*/
    qint32 shapeWidth;   //字形尺寸 、16 、 24、 32
    qint32 shapeHeight;   //字形尺寸 、16 、 24、 32
    qint32 drawWidth;   //绘制宽度
    QList <quint8> shapeData;

    /*移动相关数据*/
    QPushButton *buttonUp;
    QPushButton *buttonDown;
    QPushButton *buttonLeft;
    QPushButton *buttonRight;
    QLineEdit *lineEdit;
    QValidator *validator;
    qint32 leftCanMoveStep; //可以向右移动多少步
    qint32 rightCanMoveStep; //可以向右移动多少步
    qint32 upCanMoveStep; //可以向上移动多少步
    qint32 downCanMoveStep; //可以向下移动多少步

    qint32 HDirectionMoveStep; //水平方向移动操作， 正代表右移
    qint32 VDirectionMoveStep; //竖直方向移动操作，正代表下移

    qint32 dotWidth;
    qint32 dotHeight;

private:
    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void setOnePixel(int row, int column);
    void resetOnePixel(int row, int column);

protected slots:
    void moveToLeft();
    void moveToRight();
    void moveToUp();
    void moveToDown();
    void inputDrawWidth();
    void loseTheEditFocus();

};

#endif
