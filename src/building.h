#ifndef BUILDING_H
#define BUILDING_H

#include <QDialog>

namespace Ui {
class Building;
}

class Building : public QDialog
{
    Q_OBJECT

public:
    explicit Building(QWidget *parent = 0);
    ~Building();

private:
    Ui::Building *ui;
};

#endif // BUILDING_H
