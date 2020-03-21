#ifndef ABOUTWIN_H
#define ABOUTWIN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace Ui {
class AboutWin;
}

class AboutWin : public QWidget
{
    Q_OBJECT

public:
    explicit AboutWin(QWidget *parent = 0);
    ~AboutWin();
    void drawCtr();

private:
    Ui::AboutWin *ui;

    QPushButton *helloButton;
    QLabel *helloLabel;
    QLabel *cprightLabel;
    QLabel *versionLabel;
    QWidget *qWidget;
    QVBoxLayout *qVboxLayout;


private slots:
    void aboutwinLanguageChanged();

};

#endif // ABOUTWIN_H
