#include "aboutwin.h"
#include "ui_aboutwin.h"

AboutWin::AboutWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutWin)
{
    ui->setupUi(this);


    connect(ui->pushButtonYes, SIGNAL(clicked()),
            this, SLOT(hide()));

    connect(ui->pushButtonNo, SIGNAL(clicked()),
            this, SLOT(hide()));
    drawCtr();

}

enum
{
    TEXT_COPYRIGHT,
    TEXT_VERSION,
    TEXT_FONTTOOL
};

const static char *labelText[] =
{
    QT_TRANSLATE_NOOP("AboutWin", "Copyright (c) 2016 by hb"),
    QT_TRANSLATE_NOOP("AboutWin", "Version 1.0"),
    QT_TRANSLATE_NOOP("AboutWin", "This is a tool to modify font file!")
};

void AboutWin::drawCtr()
{

    qWidget = new QWidget(this);
    qWidget->setGeometry(50, 0, 200, 130);
    qVboxLayout = new QVBoxLayout(qWidget);

    helloButton = new QPushButton(tr("Test"), this);
    helloButton->setEnabled(false);

    helloLabel = new QLabel(tr(labelText[TEXT_FONTTOOL]), this);
    helloLabel->setWordWrap(true);
    helloLabel->setAlignment(Qt::AlignCenter);

    cprightLabel = new QLabel(tr(labelText[TEXT_COPYRIGHT]), this);
    cprightLabel->setWordWrap(true);
    cprightLabel->setAlignment(Qt::AlignCenter);

    versionLabel = new QLabel(tr(labelText[TEXT_VERSION]), this);
    versionLabel->setWordWrap(true);
    versionLabel->setAlignment(Qt::AlignCenter);

    qVboxLayout->addWidget(helloButton);
    qVboxLayout->addWidget(cprightLabel);
    qVboxLayout->addWidget(versionLabel);
    qVboxLayout->addWidget(helloLabel);



}

AboutWin::~AboutWin()
{
    delete helloButton;
    delete helloLabel;
    delete versionLabel;
    delete cprightLabel;

    delete qVboxLayout;
    delete qWidget;

    delete ui;
}
/*关于窗语言切换槽函数*/
void AboutWin::aboutwinLanguageChanged()
{
    ui->retranslateUi(this);

    helloLabel->setText(tr(labelText[TEXT_FONTTOOL]));

    cprightLabel->setText(tr(labelText[TEXT_COPYRIGHT]));

    versionLabel->setText(tr(labelText[TEXT_VERSION]));

    helloButton->setText(tr("Test"));
}
