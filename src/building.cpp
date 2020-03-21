#include "building.h"
#include "ui_building.h"
#include <QLabel>

Building::Building(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Building)
{
    ui->setupUi(this);

    QLabel label("正在创建ttf字库文件.......", this);
    label.move(0, 75);
    //label.setGeometry(QRect((matrixWidth - 75) / 2, matrixHeight + 35, 75, 25));
}

Building::~Building()
{
    delete ui;
}
