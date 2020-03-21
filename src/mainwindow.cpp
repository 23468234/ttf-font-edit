#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QStandardPaths>
#include <QMap>
#include <QTimer>
#include "building.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	mainWinInit();

}
void MainWindow::mainWinInit()
{
    this->menuInit();
    this->tab1Init();
    this->tab2Init();
    this->tab3Init();
    this->tab4Init();
    this->tab5Init();
}

MainWindow::~MainWindow()
{
    this->menuDeinit();
    this->tab1Deinit();
    this->tab2Deinit();
    this->tab3Deinit();
    this->tab4Deinit();
    this->tab5Deinit();
    delete ui;
}




/*Tab1 字库裁剪合并页 开始*/
void MainWindow::tab1Init()
{
    connect(ui->table1ButtonOpenFont1, SIGNAL(clicked()),
        this, SLOT(fontFileOpen1()));

    connect(ui->table1ButtonOpenFont2, SIGNAL(clicked()),
            this, SLOT(fontFileOpen2()));

    connect(ui->table1ButtonAdd, SIGNAL(released()),
            this, SLOT(newFontAdd()));

    connect(ui->table1ButtonDelete, SIGNAL(clicked()),
            this, SLOT(newFontDelete()));

    connect(ui->table1ButtonSave, SIGNAL(clicked()),
            this, SLOT(newFontSave()));

    connect(ui->table1ButtonClearAll, SIGNAL(clicked()),
            this, SLOT(listViewAllClear()));

    connect(ui->table1ButtonClearTarget, SIGNAL(clicked()),
            this, SLOT(listView3Clear()));

    list_font_select1 = NULL;
    list_font_select2 = NULL;

    memset(&font_file_info1, 0, sizeof(FONTFILE_INFO_S));
    memset(&font_file_info2, 0, sizeof(FONTFILE_INFO_S));

    mulu1Model= new QStandardItemModel(this);
    mulu1Mode2= new QStandardItemModel(this);
    mulu1Mode3= new QStandardItemModel(this);

    this->ui->listView1_font1->setModel(mulu1Model);
    this->ui->listView1_font2->setModel(mulu1Mode2);
    this->ui->listView1_new->setModel(mulu1Mode3);

    combFontDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);;
}


void MainWindow::tab1Deinit()
{
    fontfile_list_release(list_font_select1);
    fontfile_list_release(list_font_select2);

    fontfile_parse_release(&font_file_info1);
    fontfile_parse_release(&font_file_info2);

    delete mulu1Model;
    delete mulu1Mode2;
    delete mulu1Mode3;
}


/***************************************************************
*Description:
    警告信息
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
enum
{
    ERROR_FONT_OPEN_FAIL = 0,
    ERROR_FONT_NOT_OPENED,
    ERROR_FONT_CHOOSE_A_CHAR,
    ERROR_FONT_CANT_FIND,

    ERROR_BMP_OPEN_FAIL,
    ERROR_BMP_NOT_OPENED,
    ERROR_BMP_NOT_SUPPORT,
    ERROR_BMP_IS_TOO_BIG,
    ERROR_BMP_GRAY_SET,

    ERROR_TS_NOT_OPENED,
    ERROR_GUICFG_NOT_OPEND,

    ERROR_NUMBER
};

static const char *errorInfo[] =
{
    QT_TRANSLATE_NOOP("MainWindow", "Font open fail!"),
    QT_TRANSLATE_NOOP("MainWindow", "Font has not been opened!"),
    QT_TRANSLATE_NOOP("MainWindow", "Please choose a char!"),
    QT_TRANSLATE_NOOP("MainWindow", "Can not find out this char!"),

    QT_TRANSLATE_NOOP("MainWindow", "Bmp open fail!"),
    QT_TRANSLATE_NOOP("MainWindow", "Bmp has not been opened!"),
    QT_TRANSLATE_NOOP("MainWindow", "Not support yet!"),
    QT_TRANSLATE_NOOP("MainWindow", "The bmp picture is too big!"),
    QT_TRANSLATE_NOOP("MainWindow", "Must set to 1 - 255, Decima!"),

    QT_TRANSLATE_NOOP("MainWindow", "Please open the source ts file before!"),
    QT_TRANSLATE_NOOP("MainWindow", "Please open a gui dir before!")
};


/***************************************************************
*Description:
    getFrameTitle() 弹出框标题翻译
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
enum
{
    FRAME_TITLE_FONT_OPEN,
    FRAME_TITLE_FONT_SAVE,
    FRAME_TITLE_FONT_ERROR,
    FRAME_TITLE_BMP_OPEN,
    FRAME_TITLE_BMP_ERROR,
    FARME_TITLE_BMP_GRAY_SET,

    FRAME_TITLE_TS_OPEN,
    FRAME_TITLE_GUI_OPEN,
    FRAME_TITLE_TS_SAVE,
    FRAME_TITLE_GUI_SAVE,
};

QString MainWindow::getFrameTitle(int title)
{
    static const char *messageBoxTitle[] =
    {
        QT_TR_NOOP("Font open"),
        QT_TR_NOOP("Font save"),
        QT_TR_NOOP("Font error"),
        QT_TR_NOOP("Bmp open"),
        QT_TR_NOOP("Bmp error"),
        QT_TR_NOOP("Bmp gray set"),

        QT_TR_NOOP("Ts open"),
        QT_TR_NOOP("Gui open"),
        QT_TR_NOOP("Ts save"),
        QT_TR_NOOP("Gui save"),

    };

    return tr(messageBoxTitle[title]);
}
/***************************************************************
*Description:
    fillListView1() table1 字库1信息 QViewList 填充
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::fillListView1()
{
    QString nameString;
    PCHARSET_INFO_S pcur = NULL;
    char strBuf[CHARSET_INFO_MAXLEN] = {0};

	mulu1Model->clear();
    pcur = font_file_info1.pcharset_info;
    while (NULL != pcur)
    {
        fontfile_get_one_charset_info(pcur, strBuf);
        pcur = (PCHARSET_INFO_S)pcur->node.pnext;

        nameString.clear();
        nameString.append(strBuf);
		{			
            QStandardItem *item = new QStandardItem(nameString);
			QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::gray);
            linearGrad.setColorAt(1, Qt::yellow);
			QBrush brush(linearGrad);
			item->setBackground(brush);
			mulu1Model->appendRow(item);
		}
        //qDebug()<<strbuf;
    }
}

/***************************************************************
*Description:
    fillListView2() table1 字库2信息 QViewList 填充
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::fillListView2()
{
    QString nameString;
    PCHARSET_INFO_S pcur = NULL;
    char strBuf[CHARSET_INFO_MAXLEN] = {0};

	mulu1Mode2->clear();
    pcur = font_file_info2.pcharset_info;
    while (NULL != pcur)
    {
        fontfile_get_one_charset_info(pcur, strBuf);
        pcur = (PCHARSET_INFO_S)pcur->node.pnext;
		
        nameString.clear();
        nameString.append(strBuf);
        {
            QStandardItem *item = new QStandardItem(nameString);
            QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::gray);
            linearGrad.setColorAt(1, Qt::cyan);
            QBrush brush(linearGrad);
            item->setBackground(brush);
            mulu1Mode2->appendRow(item);
        }
        //qDebug()<<strbuf;
    }
}

/***************************************************************
*Description:
    fillListView3() table1 新字库信息QViewList填充
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::fillListView3()
{
    qint32 count = 0;
    QString nameString;
    char strbuf[CHARSET_INFO_MAXLEN] = {0};
    PCHARSET_INFO_S pcur = NULL;

	mulu1Mode3->clear();
    pcur = list_font_select1;
    while(NULL != pcur)
    {
        fontfile_get_one_charset_info(pcur, strbuf);
        nameString.clear();
        nameString.append("1 ");
        nameString.append(strbuf);
        {
            QStandardItem *item = new QStandardItem(nameString);
            QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::gray);
            linearGrad.setColorAt(1, Qt::yellow);
            QBrush brush(linearGrad);
            item->setBackground(brush);
			mulu1Mode3->appendRow(item);
        }
        count++;
        pcur = (PCHARSET_INFO_S)pcur->node.pnext;
        //qDebug()<<strbuf;
    }

    count = 0;
    pcur = list_font_select2;
    while(NULL != pcur)
    {
        fontfile_get_one_charset_info(pcur, strbuf);
        nameString.clear();
        nameString.append("2 ");
        nameString.append(strbuf);
        {
            QStandardItem *item = new QStandardItem(nameString);
            QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::gray);
            linearGrad.setColorAt(1, Qt::cyan);
            QBrush brush(linearGrad);
            item->setBackground(brush);
			mulu1Mode3->appendRow(item);
        }
        count++;
        pcur = (PCHARSET_INFO_S)pcur->node.pnext;
        //qDebug()<<strbuf;
    }

}

/***************************************************************
*Description:
    clearListView3() table1 清除列表 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::clearListView3()
{
    fontfile_list_release(list_font_select1);
    fontfile_list_release(list_font_select2);
    list_font_select1 = NULL;
    list_font_select2 = NULL;

	mulu1Mode3->clear();
}

void MainWindow::listView3Clear()
{
    clearListView3();
}

/***************************************************************
*Description:
    listViewAllClear() table1 字库1  字库2 信息清除按钮
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::listViewAllClear()
{
    fontfile_list_release(list_font_select1);
    fontfile_list_release(list_font_select2);
    list_font_select1 = NULL;
    list_font_select2 = NULL;

    fontfile_parse_release(&font_file_info1);
    fontfile_parse_release(&font_file_info2);

	mulu1Model->clear();
	mulu1Mode2->clear();
	mulu1Mode3->clear();
}
/***************************************************************
*Description:
    fontFileOpen1() table1 字库1 打开按钮
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::fontFileOpen1()
{
    char * nameStr = NULL;
    QString nameString;
    QByteArray nameByte;

    nameString = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_FONT_OPEN),
													 "C:\\Users\\Administrator\\Desktop",
                                                     "font (*.bin)");
    if (nameString.isEmpty())
	{
		return;
	}
    nameByte = nameString.toLocal8Bit();
    nameStr = nameByte.data();
    ui->table1lineEditFile1->setText (nameString);
	
	fontfile_parse_release(&font_file_info1);
    if (FAIL == fontfile_parse(nameStr, &font_file_info1))
	{
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_OPEN_FAIL]), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
	}

    this->fillListView1();
    if (0 != font_file_info1.chrset_num) //已经打开了第一个文件，再次打开第一个
    {
        //this->clearListView3();
    }
}
/***************************************************************
*Description:
    fontFileOpen2() table1 字库2 打开按钮
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::fontFileOpen2()
{
    char* nameStr = NULL;
    QString nameString;
    QByteArray nameByte;

    nameString = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_FONT_OPEN),
													 "C:\\Users\\Administrator\\Desktop",
                                                     "font (*.bin)");
    if (nameString.isEmpty())
	{
		return;
	}
    nameByte = nameString.toLocal8Bit();
    nameStr = nameByte.data();
    ui->table1lineEditFile2->setText (nameString);
	
	fontfile_parse_release(&font_file_info2);
    if (FAIL == fontfile_parse(nameStr, &font_file_info2))
	{
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_OPEN_FAIL]), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
	}

    this->fillListView2();
    if (0 != font_file_info2.chrset_num) //已经打开了第二个文件2，又再次打开第二个
    {
        //this->clearListView3();
    }
}

/***************************************************************
*Description:
    newFontAdd() table1 --> 按钮 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::newFontAdd()
{
	qint32 index1 = 0;
	qint32 index2 = 0;	
	PCHARSET_INFO_S pnode = NULL;

    index1 = ui->listView1_font1->currentIndex().row();
    index2 = ui->listView1_font2->currentIndex().row();

	if (index1 >= 0)
	{
        /*获取光标所指向的节点*/
        pnode = fontfile_list_get_node_by_index(font_file_info1.pcharset_info, index1);
		//qDebug()<<"index1:"<<index1<<"node.id:"<<pnode->node.id;
	}

	if (NULL != pnode)
	{
        /*添加到链表保存，并排序*/
        list_font_select1 = fontfile_list_add_node_to_list_rear(list_font_select1, pnode);
        list_font_select1 = fontfile_list_sort_by_nodeid(list_font_select1);
	}

    pnode = NULL;
	if (index2 >= 0)
	{
        pnode = fontfile_list_get_node_by_index(font_file_info2.pcharset_info, index2);
		//qDebug()<<"index2:"<<index1<<"node.id:"<<pnode->node.id;
	}
	
	if (NULL != pnode)
	{
        list_font_select2 = fontfile_list_add_node_to_list_rear(list_font_select2, pnode);
        list_font_select2 = fontfile_list_sort_by_nodeid(list_font_select2);
	}

    this->fillListView3();
}

/***************************************************************
*Description:
    newFontDelete() table1 <-- 按钮 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::newFontDelete()
{
	qint32 index = 0;
	qint32 count = 0;

    index = ui->listView1_new->currentIndex().row();
	if (index < 0)
	{
		return;
	}

    count = fontfile_list_get_node_count(list_font_select1);
    //qDebug()<<"list1 count:%d"<<count;
    if (index < count)
    {
        //qDebug()<<"delete index:"<<index;
        list_font_select1 = fontfile_list_delet_node_by_index(list_font_select1, index);
    }
    else
    {
        //qDebug()<<"delete index:"<<index - count;
        list_font_select2 = fontfile_list_delet_node_by_index(list_font_select2, index - count);
    }

    this->fillListView3();
}
/***************************************************************
*Description:
    newFontSave() table1 新建按钮 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::newFontSave()
{
    char* nameStr = NULL;
    QString nameString;
    QByteArray nameByte;
    QFileInfo fileInfo;
    PCHARSET_INFO_S viewlistList1 = NULL;
    PCHARSET_INFO_S viewlistList2 = NULL;
    PCHARSET_INFO_S plistNew = NULL;
    FONTFILE_INFO_S font_file_info_save;
	
    nameString = QFileDialog::getSaveFileName(this, getFrameTitle(FRAME_TITLE_FONT_SAVE),
                                              combFontDir,
                                              "*.bin"); //选择路径
    if (nameString.isEmpty())
	{
		return;
	}

    fileInfo = QFileInfo(nameString);
    if ("bin" != fileInfo.suffix())
    {
        nameString += ".bin";
    }
    combFontDir = fileInfo.absolutePath();

    nameByte = nameString.toLocal8Bit();
    nameStr = nameByte.data();

    if (NULL == list_font_select1 && NULL == list_font_select2)
	{
		return;
	}

    /*选中的字符集信息链表,复制一份，不要破坏*/
    viewlistList1 = fontfile_list_copy(list_font_select1);
    viewlistList2 = fontfile_list_copy(list_font_select2);
    if (NULL == viewlistList1 && NULL == viewlistList2)
	{
		return;
	}

    /*将两条链表合并成一条*/
    plistNew = fontfile_list_combo_two_lists(viewlistList1, viewlistList2);

    /*listview3 字符集信息链表，进行排序，相同区间字符集合并*/
    plistNew = fontfile_list_filter_charset_info(plistNew);

	/*排序、合并后的字符集链表，构建字符集信息结构体*/
    memset(&font_file_info_save, 0, sizeof(FONTFILE_INFO_S));
    if (FAIL == fontfile_list_create_font_info(&font_file_info_save, plistNew))
	{
		return ;
	}	
	
	/*将字符集信息结构体保存*/
    fontfile_save_by_name(nameStr, &font_file_info_save);
	
    /*释放新创建的字库信息结构体*/
	fontfile_parse_release(&font_file_info_save);

    /*释放拷贝的字符串链表*/
    fontfile_list_release(plistNew);
    plistNew = NULL;
    viewlistList2 = NULL;
    viewlistList1 = NULL;
}
/*Tab1 字库裁剪合并页 结束*/




/*Tab2 字符修改页 开始*/
/***********************************************************************************************************************************
*Description:
    tab2Init() table2 字符修改页初始化
*Input:
    无
*Output:
    无
*Return:
    无
************************************************************************************************************************************/
void MainWindow::tab2Init()
{
    memset(&font_file_info3, 0, sizeof(FONTFILE_INFO_S));

    this->bmpFile = new BmpParse;

    this->curInputCode = -1;

    this->isLeftFontWidgetShowedchar = false;

    this->fontOpenDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->fontSaveDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->bmpOpenDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    connect(ui->table2ButtonFontOpen, SIGNAL(clicked()),
        this, SLOT(table2OpenButton()));

    connect(ui->table2ButtonGoto, SIGNAL(clicked()),
        this, SLOT(table2IntputUnicode()));

    connect(ui->table2ButtonBmpOpen, SIGNAL(clicked()),
        this, SLOT(table2OpenBmp()));

    connect(ui->table2ButtonGraySet, SIGNAL(clicked()),
        this, SLOT(table2SetThreshold()));

    connect(ui->table2PushbuttonSaveChar, SIGNAL(clicked()),
        this, SLOT(table2SaveChar()));

    connect(ui->table2ButtonBmpCopy, SIGNAL(clicked()),
        this, SLOT(table2CopyBmpToFont()));

    connect(ui->table2ButtonNewFont, SIGNAL(clicked()),
        this, SLOT(table2SaveFontFile()));

    connect(ui->table2LineEditUnicode, SIGNAL(returnPressed()),
           ui->table2ButtonGoto, SIGNAL(clicked()));

    connect(ui->table2LineEditGray, SIGNAL(returnPressed()),
           ui->table2ButtonGraySet, SIGNAL(clicked()));


}

void MainWindow::tab2Deinit()
{
    delete bmpFile;
}
/***************************************************************
*Description:
    table2OpenButton() table2 字库打开 slot
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2OpenButton()
{
    QFileInfo fileInfo;
    char * nameStr = NULL;
    QString nameQstring;
    QByteArray nameQbyte;

    nameQstring = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_FONT_OPEN),
                                                     "C:\\Users\\Administrator\\Desktop\\fontManage\\qt_font_manager\\doc",//fontOpenDir,
                                                     "font (*.bin)");
    if (nameQstring.isEmpty())
    {
        return;
    }
    nameQbyte = nameQstring.toLocal8Bit();
    nameStr = nameQbyte.data();
    ui->table2LineEdit1->setText(nameQstring);

    /*保存文件打开路径*/
    fileInfo = QFileInfo(nameQstring);
    fontOpenDir = fileInfo.absolutePath();

    this->ui->table2FontWidget1->clear();
    this->isLeftFontWidgetShowedchar = false;
    fontfile_parse_release(&font_file_info3);
    if (FAIL == fontfile_parse(nameStr, &font_file_info3))
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_OPEN_FAIL]), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
}

/***************************************************************
*Description:
    table2IntputUnicode() table2 字符集跳转 slot函数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2IntputUnicode()
{
    bool ok = false;
    qint32 index = 0;
    qint32 onecharsize = 0;
    qint32 count = 0;
    qint32 offset = 0;
    qint32 inputCode = 0;
    QString inputString;
    QList<quint8> byte;
    PCHARSET_INFO_S pchrinfo = NULL;

    inputString = ui->table2LineEditUnicode->text();
    inputCode = inputString.toInt(&ok, 16);

    //qDebug()<<"input num:"<<ucode_index;
    if (false == ok)
    {
        return ;
    }

    if (0 == font_file_info3.chrset_num || 1 == font_file_info3.bad)
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_NOT_OPENED]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    this->curInputCode = inputCode;

    pchrinfo = font_file_info3.pcharset_info;
    for (index = 0; index < font_file_info3.chrset_num; index++)
    {
        /*unicode 编码在字符集索引区间内*/
        if (this->curInputCode >= pchrinfo->ucode_start && this->curInputCode < pchrinfo->ucode_end)
        {
             /*一个字符形状的数据的数目*/
            onecharsize = (pchrinfo->end_addr - pchrinfo->start_addr) / pchrinfo->char_num;

            /*字符在该字符集中的相对位置*/
            offset = this->curInputCode - pchrinfo->ucode_start;

            for (count = 0; count < onecharsize; ++count)
            {
                byte << *(pchrinfo->pshape_info + offset*onecharsize + count);
            }
            ui->table2FontWidget1->clear();
            ui->table2FontWidget1->setWidth(pchrinfo->width);
            ui->table2FontWidget1->setHight(pchrinfo->height);
            ui->table2FontWidget1->setShapeArray(byte);
            ui->table2FontWidget1->setDrawWidth(pchrinfo->pdraw_width[offset]);
            ui->table2FontWidget1->update();
            break;
        }

        pchrinfo = (PCHARSET_INFO_S)pchrinfo->node.pnext;
    }

    if (index == font_file_info3.chrset_num)
    {
         this->isLeftFontWidgetShowedchar = false;
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_CANT_FIND]), \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        this->isLeftFontWidgetShowedchar = true;
    }

}

/***************************************************************
*Description:
    table2OpenBmp() table2 字符修改页，bmp打开 SLOT函数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2OpenBmp()
{
    QFileInfo fileInfo;
    QString nameString;
    QList<quint8> curData;
    nameString = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_FONT_OPEN),
                                                     "C:\\Users\\Administrator\\Desktop\\fontManage\\qt_font_manager\\doc",//bmpOpenDir,
                                                     "bmp (*.bmp)");
    if (nameString.isEmpty())
    {
        return;
    }

    ui->table2LineEdit3->setText(nameString);

    fileInfo = QFileInfo(nameString);
    bmpOpenDir = fileInfo.absolutePath();

    if (FAIL == bmpFile->openBmp(nameString))
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_BMP_OPEN), tr(errorInfo[ERROR_BMP_OPEN_FAIL]), \
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (1 == bmpFile->getBmpBitCount()) //单色位图
    {
        ui->table2LineEditGray->setEnabled(false);
    }
    else
    {
        ui->table2LineEditGray->setEnabled(true);
    }

    bmpFile->resetThreshold();
    ui->table2LineEditGray->setText(QString("%1").arg(bmpFile->getGrayThreshold()));

    curData = bmpFile->get8BitModel();
    //qDebug()<<"bmp width"<<bmpFile->getWidth();
    //qDebug()<<"bmp height"<<bmpFile->getHeight();
    ui->table2FontWidget2->clear();
    ui->table2FontWidget2->setWidth(bmpFile->getWidth());
    ui->table2FontWidget2->setHight(bmpFile->getHeight());
    //ui->table2FontWidget2->setDrawWidth(bmpFile->getDrawWidth());
    ui->table2FontWidget2->setShapeArray(curData);
    ui->table2FontWidget2->setDrawWidth(ui->table2FontWidget2->getDrawWidth());
    ui->table2FontWidget2->update();

}

/***************************************************************
*Description:
    table2SetThreshold()  table2 灰度阈值 按钮SLOT函数
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2SetThreshold()
{
    bool ok = false;
    quint32 threshold;
    QString thresholdSTring;
    QList<quint8> curData;

    thresholdSTring = ui->table2LineEditGray->text();
    threshold = thresholdSTring.toInt(&ok, 10);

    if (false == ok|| threshold < 1 || threshold > 255)
    {
        QMessageBox::warning(this, getFrameTitle(FARME_TITLE_BMP_GRAY_SET), tr(errorInfo[ERROR_BMP_GRAY_SET]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        ui->table2LineEditGray->setText(QString("%1").arg(bmpFile->getGrayThreshold()));
        return ;
    }
    //qDebug() << "threshold "<<threshold;
    bmpFile->setGrayThreshold(threshold);
    curData = bmpFile->get8BitModel();
    if (curData.isEmpty())
    {
        QMessageBox::warning(this, getFrameTitle(FARME_TITLE_BMP_GRAY_SET), tr(errorInfo[ERROR_BMP_NOT_OPENED]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
    ui->table2FontWidget2->setWidth(bmpFile->getWidth());
    ui->table2FontWidget2->setHight(bmpFile->getHeight());
    ui->table2FontWidget2->setDrawWidth(bmpFile->getDrawWidth());
    ui->table2FontWidget2->setShapeArray(curData);
    ui->table2FontWidget2->update();
}

/***************************************************************
*Description:
    table2CopyBmpToFont() table2 font <-- bmp按钮
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2CopyBmpToFont()
{
    qint32 fontWidth = 0;
    qint32 fontHeight = 0;
    QList<quint8> curBmpData;

    fontWidth = ui->table2FontWidget1->getWidth();
    fontHeight = ui->table2FontWidget1->getHeight();


    if (ui->table2FontWidget2->getShapeData().isEmpty()) //获取的数据是空的
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_BMP_OPEN), tr(errorInfo[ERROR_BMP_NOT_OPENED]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (1 == font_file_info3.bad) //文件没有打开
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_BMP_NOT_OPENED]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (false == this->isLeftFontWidgetShowedchar) //文件打开了没有选择字符
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_ERROR), tr(errorInfo[ERROR_FONT_CHOOSE_A_CHAR]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (fontWidth < ui->table2FontWidget2->getWidth() || fontHeight < ui->table2FontWidget2->getHeight()) //照片比字体的大
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_BMP_ERROR), tr(errorInfo[ERROR_BMP_IS_TOO_BIG]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    curBmpData = ui->table2FontWidget2->getPartOfShape(fontWidth, fontHeight);  //从 font widget 2 中 截取或填充一个这么大的
    ui->table2FontWidget1->setShapeArray(curBmpData);
    ui->table2FontWidget1->setDrawWidth(ui->table2FontWidget1->getDrawWidth());
    ui->table2FontWidget1->update();

}

/***************************************************************
*Description:
    table2SaveChar() 改变字库输入的inputcode信息
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2SaveChar()
{
    qint32 index = 0;
    qint32 offset = 0;
    qint32 onecharsize = 0;
    qint32 count = 0;
    qint32 shapeWidth = 0;
    QList<quint8> shapeData;
    PCHARSET_INFO_S pchrinfo = NULL;

    if (1 == font_file_info3.bad || 0 == font_file_info3.chrset_num)
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_NOT_OPENED]), \
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }

    if (false == this->isLeftFontWidgetShowedchar) //文件打开了没有选择字符
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_ERROR), tr(errorInfo[ERROR_FONT_CHOOSE_A_CHAR]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
    //qDebug()<<"save";
    if ( QMessageBox::No == QMessageBox::question(this, getFrameTitle(FRAME_TITLE_FONT_SAVE), tr("Sure to replace?"), \
                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
    {
         return;
    }

    pchrinfo = font_file_info3.pcharset_info;
    for (index = 0; index < font_file_info3.chrset_num; index++)
    {
        /*unicode 编码在字符集索引区间内*/
        if (this->curInputCode >= pchrinfo->ucode_start && this->curInputCode < pchrinfo->ucode_end)
        {
            /*一个字符形状的数据的数目*/
            onecharsize = (pchrinfo->end_addr - pchrinfo->start_addr) / pchrinfo->char_num;

            /*从 字符点阵控件1 中截取 数据*/
            shapeData = ui->table2FontWidget1->getPartOfShape(pchrinfo->width, pchrinfo->height);
            if (shapeData.count() != onecharsize)
            {
                qDebug() << "Dangerous bug "<< __FUNCTION__;
            }

            /*字符在该字符集中的相对位置*/
            offset = this->curInputCode - pchrinfo->ucode_start;

            /*字符在该字符集中的相对位置*/
            for (count = 0; count < onecharsize; ++count)
            {
                 *(pchrinfo->pshape_info + offset*onecharsize + count) = shapeData.at(count);
            }

            /*获取绘制宽度*/
            shapeWidth = ui->table2FontWidget1->getDrawWidth();//getDrawWidth(shapeData, pchrinfo->width, pchrinfo->height);

            *(pchrinfo->pdraw_width + offset) = shapeWidth;
        }

        pchrinfo = (PCHARSET_INFO_S)pchrinfo->node.pnext;
    }
}
/***************************************************************
*Description:
    table2SaveFontFile() table2 新建 按钮
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::table2SaveFontFile()
{
    char * fileName = NULL;
    QFileInfo fileInfo;
    QString nameString;
    QByteArray nameQbyte;

    if (0 == font_file_info3.chrset_num || 1 == font_file_info3.bad)
    {
        QMessageBox::warning(this, getFrameTitle(FRAME_TITLE_FONT_OPEN), tr(errorInfo[ERROR_FONT_NOT_OPENED]), \
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    nameString = QFileDialog::getSaveFileName(this, getFrameTitle(FRAME_TITLE_FONT_SAVE),
                                              fontSaveDir,
                                              "*.bin"); //选择路径
    if (nameString.isEmpty())
    {
        return;
    }


    fileInfo = QFileInfo(nameString);
    if ("bin" != fileInfo.suffix())
    {
        nameString += ".bin";
    }
    fontSaveDir = fileInfo.absolutePath();

    nameQbyte = nameString.toLocal8Bit();
    fileName = nameQbyte.data();
    fontfile_save_by_name( fileName, &font_file_info3);
}

/*Tab2 字符修改页 结束*/




/*Tab3 ts gui互转 开始--------------*/
/***************************************************************
*Description:
    tab3Init() table3初始化
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::tab3Init()
{

    this->tsParse = new TsParse;
    this->tsSourceParse = new TsParse;
    this->guiParse = new GuiCfgParse;

    this->tsSourceDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->tsOpenDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->guiOpenDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->tsSaveDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->guiSaveDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    connect(ui->table3ButtonOpenTs, SIGNAL(clicked()),
                this, SLOT(table3OpenTs()));

    connect(ui->table3ButtonGuiToTs, SIGNAL(clicked()),
                this, SLOT(table3transformGuiToTs()));


    connect(ui->table3ButtonOpenSourceTs, SIGNAL(clicked()),
                this, SLOT(table3OpenTsSource()));

    connect(ui->table3ButtonOpenGui, SIGNAL(clicked()),
                this, SLOT(table3OpenGui()));

    connect(ui->table3ButtonTsToGui, SIGNAL(clicked()),
                this, SLOT(table3transformTsToGui()));



}

/***************************************************************
*Description:
    tab3Deinit() tab3析构
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::tab3Deinit()
{

    delete this->tsParse;
    delete this->guiParse;
    delete this->tsSourceParse;
}

/***************************************************************
*Description:
    table3OpenTs() tab3 ts文件打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table3OpenTs()
{
    QFileInfo fileInfo;
    QString nameString;

    nameString = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_TS_OPEN),
                                                    this->tsOpenDir,
                                                    "*.ts"); //选择路径

    if (nameString.isEmpty())
    {
        return;
    }

    ui->table3LineEditTs->setText(nameString);

    fileInfo = QFileInfo(nameString);
    tsOpenDir = fileInfo.absolutePath();
    //qDebug()<< "Open dir" << tsOpenDir;
    if (SUCCESS == this->tsParse->checkTsFile(nameString)) //检查是不是ts文件
    {
        this->tsParse->readTsFile(nameString);
        //this->tsParse->fillListWidget(ui->table3ListWidgetTsInfo1);
    }

}

/***************************************************************
*Description:
    table3transformTsToGui() tab3 ts转GUI
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table3transformTsToGui()
{
    QDir dir;
    GuiCfgParse guiCfg;

    if (SUCCESS == this->tsParse->translateToGui(guiCfg))
    {
        QString dirString;
        dirString = QFileDialog::getExistingDirectory(this, getFrameTitle(FRAME_TITLE_GUI_SAVE),
                                                            this->guiSaveDir,
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontConfirmOverwrite); //选择路径

        if (dirString.isEmpty())
        {
            return;
        }

        guiSaveDir = dirString;
        dir.setPath(guiSaveDir);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        QStringList filters;
        filters << "*.txt";
        dir.setNameFilters(filters);
        if (!dir.entryInfoList().isEmpty())
        {
            if ( QMessageBox::No == QMessageBox::question(this, getFrameTitle(FRAME_TITLE_GUI_SAVE),
                           tr("The dir is not empty,sure to write?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes))
            {
                 return;
            }

        }
        //qDebug()<< "guiOpenDir" << guiSaveDir;
        guiCfg.guiCfgSave(guiSaveDir);

        /*转换成功弹出框*/
        QMessageBox::information(this, getFrameTitle(FRAME_TITLE_GUI_OPEN),tr("Translate success!"),
                               QMessageBox::Yes |  QMessageBox::No, QMessageBox::Yes);
    }
    else
    {
        /*转换失败弹出框*/
        QMessageBox::information(this, getFrameTitle(FRAME_TITLE_GUI_OPEN),tr("Translate fail!"),
                               QMessageBox::Yes |  QMessageBox::No, QMessageBox::Yes);
    }

}
/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table3OpenTsSource()
{
    QFileInfo fileInfo;
    QString nameString;

    nameString = QFileDialog::getOpenFileName(this, getFrameTitle(FRAME_TITLE_TS_OPEN),
                                                    this->tsSourceDir,
                                                    "*.ts"); //选择路径

    if (nameString.isEmpty())
    {
        return;
    }

    ui->table3LineEditTsSource->setText(nameString);

    fileInfo = QFileInfo(nameString);
    this->tsSourceDir = fileInfo.absolutePath();
    //qDebug()<< "Open dir" << tsOpenDir;
    if (SUCCESS == this->tsParse->checkTsFile(nameString)) //检查是不是ts文件
    {
        this->tsSourceParse->readTsFile(nameString);
        this->tsSourceParse->fillTreeWidget(ui->table3TreeWidgetTsInfo2);
    }
}
/***************************************************************
*Description:
    table3OpenGui() tab3 gui文件夹打开按钮
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table3OpenGui()
{
    QFileInfo fileInfo;
    QStringList fileList;
    fileList = QFileDialog::getOpenFileNames(this, getFrameTitle(FRAME_TITLE_TS_OPEN),
                                                        this->guiOpenDir,
                                                         "*.txt"); //选择路径

    if (fileList.isEmpty())
    {
        return;
    }

    fileInfo = QFileInfo(fileList.at(0));
    guiOpenDir = fileInfo.absolutePath();
    ui->table3LineEditGui->setText(guiOpenDir);
    //qDebug()<< "guiOpenDir" << guiOpenDir;

    this->guiParse->guiCfgOpen(fileList);
    this->guiParse->fillTreeWidget(ui->table3TreeWidgetMiniguiInfo);

}

/***************************************************************
*Description:
    table3transformGuiToTs() tab3 Gui转ts按钮
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table3transformGuiToTs()
{
    if (!this->tsSourceParse->isOpened())
    {
        QMessageBox::critical(this, getFrameTitle(FRAME_TITLE_TS_OPEN), tr(errorInfo[ERROR_TS_NOT_OPENED]), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    if (!this->guiParse->isOpened())
    {
        QMessageBox::critical(this, getFrameTitle(FRAME_TITLE_GUI_OPEN), tr(errorInfo[ERROR_GUICFG_NOT_OPEND]), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    //拷贝一份打开的ts信息
    TsParse tsCfg = *this->tsSourceParse;

    if (SUCCESS == this->guiParse->translateToTs(tsCfg))
    {
        QFileInfo fileInfo;
        QString nameString;

        nameString = QFileDialog::getSaveFileName(this, getFrameTitle(FRAME_TITLE_TS_SAVE),
                                                            this->tsSaveDir,
                                                            "*.ts"); //选择路径
        if (nameString.isEmpty())
        {
            return;
        }

        fileInfo = QFileInfo(nameString);
        if ("ts" != fileInfo.suffix()) //后缀名不是.ts的，添加.ts
        {
            nameString += ".ts";
        }
        tsSaveDir = fileInfo.absolutePath();
        //qDebug()<< "Open dir" << tsSaveDir;

        /*写ts文件*/
        if (SUCCESS == tsCfg.writeTs(nameString))
        {
            /*校验ts文件*/
            if (SUCCESS == tsCfg.checkTsFile(nameString))
            {
                /*转换成功弹出框*/
                QMessageBox::information(this, getFrameTitle(FRAME_TITLE_GUI_OPEN),tr("Translate success!"),
                                       QMessageBox::Yes |  QMessageBox::No, QMessageBox::Yes);
                return;
            }
        }
        /*转换失败弹出框*/
        QMessageBox::information(this, getFrameTitle(FRAME_TITLE_GUI_OPEN),tr("Translate fail!"),
                                   QMessageBox::Yes |  QMessageBox::No, QMessageBox::Yes);



    }
}

/*Tab3 ts gui互转 结束*/


/*Tab4 GUI字符统计 开始--------------*/
void MainWindow::tab4Init()
{
    this->table4CharStatistics = new CharacterStatistics();

    this->dicFileDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    connect(ui->table4buttonOpenDic, SIGNAL(clicked()),
            this, SLOT(table4OpenGuiDicFile()));
}


void MainWindow::tab4Deinit()
{
    delete this->table4CharStatistics;
}

/***************************************************************
*Description:
    table4OpenGuiDicFile() tab4 打开字典总表按钮
*Input:
    无
*Return:
    无
****************************************************************/
void MainWindow::table4OpenGuiDicFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open dictionary file"),
                                                        this->dicFileDir,
                                                         "*.txt"); //选择路径

    if (fileName.isEmpty())
    {
        return;
    }

    /*保存打开文件的路径*/
    QFileInfo fileInfo = QFileInfo(fileName);
    dicFileDir = fileInfo.absolutePath();
    ui->table4LineEditDicFile->setText(fileName);

    /*显示比较的结果*/
    if (this->table4CharStatistics->openDicFile(fileName))
    {
        ui->tabel4textEditGb2312->setText(this->table4CharStatistics->resultCompareWithGb2312());
        ui->tabel4textEditGbk->setText(this->table4CharStatistics->resultCompareWithGbk());
    }

}

/*Tab4 GUI字符统计 结束*/

/*Tab5  矢量字体裁剪开始--------------*/
void MainWindow::tab5Init()
{
    isTtfOpenOk = false;
    this->ttfModify = new TtfModify;
    this->table5CharStatistics = new CharacterStatistics;

    this->table5Mode1 = new QStandardItemModel(this);
    ui->table5ListViewTTFCharset->setModel(table5Mode1);

    this->buildingWin = new Building(this);
    this->buildingWin->setWindowTitle("请等待");
    this->buildingWin->hide();

    this->ttfOpenDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->ttfSaveDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    connect(ui->table5buttonTtfOpen, SIGNAL(clicked()),
            this, SLOT(table5TtfFileOpen()));

    connect(ui->table5ButtonTtfSave, SIGNAL(clicked()),
            this, SLOT(table5TtfFileSave()));

    connect(ui->table5ButtonSelectShift, SIGNAL(clicked()),
            this, SLOT(table5ButtonShift()));

    connect(ui->table5CheckBoxAll, SIGNAL(clicked()),
            this, SLOT(table5ButtonSelectAll()));

    connect(ui->table5radioButtonCharset, SIGNAL(clicked()),
            this, SLOT(table5RadioButtonChangeCutWay()));

    connect(ui->table5radioButtonLanguageSet, SIGNAL(clicked()),
            this, SLOT(table5RadioButtonChangeCutWay()));

    connect(ui->table5RadioButtonCharAndCharset, SIGNAL(clicked()),
            this, SLOT(table5RadioButtonChangeCutWay()));



    connect(this, SIGNAL(ttfFontBuild(QString)), this, SLOT(buildTtfFile(QString)), Qt::QueuedConnection);
}

void MainWindow::tab5Deinit()
{
    delete this->table5Mode1;
    delete this->ttfModify;
    delete this->table5CharStatistics;
}

/***************************************************************
*Description: table5打开ttf文件
*Input:    无
*Output:    无
*Return:    无
****************************************************************/
void MainWindow::table5TtfFileOpen()
{
    QString fileName = QFileDialog:: getOpenFileName(this,  tr("Save ttf file"),
                                                            "C:\\Users\\Administrator\\Desktop\\fontManage\\qt_font_manager\\doc",//this->ttfOpenDir,
                                                            "*.ttf"); //选择路径
    if (fileName.isEmpty())
    {
        return;
    }

    /*保存打开文件的路径*/
    QFileInfo fileInfo = QFileInfo(fileName);
    ttfOpenDir = fileInfo.absolutePath();
    ui->table5lineEditTtfOpen->setText(fileName);

    if (!this->ttfModify->ttfFileOpen(fileName))
    {
        this->table5Mode1->clear();
        isTtfOpenOk = false;
        return;
    }

    isTtfOpenOk = true;

    ui->table5CheckBoxAll->setChecked(true);
    this->ttfModify->fillListView(this->table5Mode1);
}

QByteArray MainWindow::getListViewMask()
{
    QByteArray outArray;
    for (int i = 0; i < this->table5Mode1->rowCount(); ++i)
    {
        QStandardItem * item = (QStandardItem *)this->table5Mode1->item(i);
        if (Qt::Checked == item->checkState())
        {
            outArray.append((char)1);
        }
        else
        {
            outArray.append((char)0);
        }
    }

    return outArray;
}


void MainWindow::buildTtfFile(QString fileName)
{
    /*listView的选择情况*/
    QByteArray listViewMask = getListViewMask();

    /*精确裁剪所有的unicode*/
    QMap<quint16, bool> newTtfUncodeMap = this->precisionCutInputUnicode();


    if (newTtfUncodeMap.isEmpty())
    {
       // QMessageBox::warning(this, tr("ttf save"), tr("没有任何字符"), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
       // return;
    }
    /*创建新ttf文件*/
    this->ttfModify->ttfFileCreate(fileName, listViewMask, newTtfUncodeMap);

    this->buildingWin->hide();
}

/***************************************************************
*Description: table5新建ttf文件
*Input:    无
*Output:    无
*Return:    无
****************************************************************/
void MainWindow::table5TtfFileSave()
{
    if (!isTtfOpenOk)
    {
        QMessageBox::warning(this, tr("ttf save"), tr("请先打开一个ttf"), \
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,   tr("Open ttf file"),
                                                           "C:\\Users\\Administrator\\Desktop\\fontManage\\qt_font_manager\\doc",//
                                                            "*.ttf"); //选择路径
    if (fileName.isEmpty())
    {
        return;
    }

    /*保存打开文件的路径*/
    QFileInfo fileInfo = QFileInfo(fileName);
    if ("ttf" != fileInfo.suffix()) //后缀名不是.ts的，添加.ts
    {
        fileName += ".ttf";
    }
    ttfSaveDir = fileInfo.absolutePath();

    this->buildingWin->show();
    emit ttfFontBuild(fileName);
}

/***************************************************************
*Description: table5反选
*Input:    无
*Output:    无
*Return:    无
****************************************************************/
void MainWindow::table5ButtonShift()
{
    for (int i = 0; i < this->table5Mode1->rowCount(); ++i)
    {
        QStandardItem * item = (QStandardItem *)this->table5Mode1->item(i);
        //qDebug () <<"check state"<<item->checkState();
        if (Qt::Checked == item->checkState())
        {
            item->setCheckState(Qt::Unchecked);
        }
        else
        {
            item->setCheckState(Qt::Checked);
        }
    }
}
/***************************************************************
*Description: table5全选、清除
*Input:    无
*Output:    无
*Return:    无
****************************************************************/
void MainWindow::table5ButtonSelectAll()
{
    Qt::CheckState checkState = Qt::Checked;
    if (ui->table5CheckBoxAll->isChecked())
    {
        checkState = Qt::Checked;
    }
    else
    {
        checkState = Qt::Unchecked;
    }

    for (int i = 0; i < this->table5Mode1->rowCount(); ++i)
    {
        QStandardItem * item = (QStandardItem *)this->table5Mode1->item(i);
        item->setCheckState(checkState);
    }
}

/***************************************************************
*Description: table5精确裁剪方式切换
*Input:    无
*Output:    无
*Return:    无
****************************************************************/
void MainWindow::table5RadioButtonChangeCutWay()
{
    if (ui->table5radioButtonCharset->isChecked())
    {
        ui->table5TextInput->setEnabled(true);
        ui->table5ComboboxLanguage->setEnabled(false);
    }
    else if(ui->table5radioButtonLanguageSet->isChecked())
    {
        ui->table5TextInput->setEnabled(false);
        ui->table5ComboboxLanguage->setEnabled(true);
    }
    else if(ui->table5RadioButtonCharAndCharset->isChecked())
    {
        ui->table5TextInput->setEnabled(true);
        ui->table5ComboboxLanguage->setEnabled(true);
    }


}
/***************************************************************
*Description: 根据界面选择，计算新字库中包含的所包含的unicode
*Input:    无
*Output:    无
*Return:    新字库中所包含的unicode
****************************************************************/
/*为了排序和字符不重复，采用map*/
QMap<quint16, bool> MainWindow::precisionCutInputUnicode()
{
    QMap<quint16, bool> outMap;
    if(ui->table5radioButtonCharset->isChecked())
    {
        QString inputText = ui->table5TextInput->toPlainText();

        /*将输入的txt内容的unicode，在选中的QListView中找，是否存在.存在就添加到list中,*/
        for (int stringIndex = 0; stringIndex < inputText.count(); ++stringIndex)
        {
            quint16 curUnicode = inputText.at(stringIndex).unicode();
            outMap.insert(curUnicode, true);
        }
        return outMap;
    }
    else  if (ui->table5radioButtonLanguageSet->isChecked())//语言集合选取
    {
        if (0 == ui->table5ComboboxLanguage->currentIndex())
        {
            QMap<qint32, quint16>::const_iterator gb2312MapIterator;

            /*遍历gb2312映射unicode的map*/
            for (gb2312MapIterator = this->table5CharStatistics->unicodeToGb2312Map.begin();
                gb2312MapIterator != this->table5CharStatistics->unicodeToGb2312Map.end(); ++gb2312MapIterator)
                {
                    qint32 curGb2312Unicode = gb2312MapIterator.key();
                    outMap.insert(curGb2312Unicode, true);
                }


            /*基本的ascii*/
            for (int i = 0x20; i < 0x7F; ++i)
            {
                outMap.insert(i, true);
            }

        }
        else if(1 == ui->table5ComboboxLanguage->currentIndex())
        {
            QMap<qint32, quint16>::const_iterator gbkMapIterator;

            /*遍历gb2312映射unicode的map*/
            for (gbkMapIterator = this->table5CharStatistics->unicodeToGbkMap.begin();
                gbkMapIterator != this->table5CharStatistics->unicodeToGbkMap.end(); ++gbkMapIterator)
            {
                qint32 curGbkUnicode = gbkMapIterator.key();
                outMap.insert(curGbkUnicode, true);
            }


            /*基本的ascii*/
            for (int i = 0x20; i < 0x7F; ++i)
            {
                outMap.insert(i, true);
            }
        }
    }
    else if (ui->table5RadioButtonCharAndCharset->isChecked())
    {
        QString inputText = ui->table5TextInput->toPlainText();

        /*将输入的txt内容的unicode，在选中的QListView中找，是否存在.存在就添加到list中,*/
        for (int stringIndex = 0; stringIndex < inputText.count(); ++stringIndex)
        {
            quint16 curUnicode = inputText.at(stringIndex).unicode();
            outMap.insert(curUnicode, true);
        }

        if (0 == ui->table5ComboboxLanguage->currentIndex())
        {
            QMap<qint32, quint16>::const_iterator gb2312MapIterator;

            /*遍历gb2312映射unicode的map*/
            for (gb2312MapIterator = this->table5CharStatistics->unicodeToGb2312Map.begin();
                gb2312MapIterator != this->table5CharStatistics->unicodeToGb2312Map.end(); ++gb2312MapIterator)
                {
                    qint32 curGb2312Unicode = gb2312MapIterator.key();
                    outMap.insert(curGb2312Unicode, true);
                }


            /*基本的ascii*/
            for (int i = 0x20; i < 0x7F; ++i)
            {
                outMap.insert(i, true);
            }

        }
        else if(1 == ui->table5ComboboxLanguage->currentIndex())
        {
            QMap<qint32, quint16>::const_iterator gbkMapIterator;

            /*遍历gb2312映射unicode的map*/
            for (gbkMapIterator = this->table5CharStatistics->unicodeToGbkMap.begin();
                gbkMapIterator != this->table5CharStatistics->unicodeToGbkMap.end(); ++gbkMapIterator)
            {
                qint32 curGbkUnicode = gbkMapIterator.key();
                outMap.insert(curGbkUnicode, true);
            }


            /*基本的ascii*/
            for (int i = 0x20; i < 0x7F; ++i)
            {
                outMap.insert(i, true);
            }
        }
    }

    return outMap;
}


/*Tab5  矢量字体裁剪结束*/

/*菜单栏*/
void MainWindow::menuInit()
{
    aboutWindow = new AboutWin;
    aboutWindow->hide();

    translator = new QTranslator;

    connect(ui->actionLanguage_Ch, SIGNAL(triggered()),
        this, SLOT(changeLanguageToChinese()));

    connect(ui->actionLanguage_En, SIGNAL(triggered()),
        this, SLOT(changeLanguageToEnglish()));

    connect(ui->actionLanguage_Kr, SIGNAL(triggered()),
        this, SLOT(changeLanguageToKorean()));

    connect(ui->actionAbout_A, SIGNAL(triggered()),
        this, SLOT(showAboutWin()));

    connect(this, SIGNAL(languageChanged()),
            this, SLOT(mainwinLanguageChanged()));

    connect(this, SIGNAL(languageChanged()),
            this->aboutWindow, SLOT(aboutwinLanguageChanged()));
}

void MainWindow::menuDeinit()
{
    delete aboutWindow;
    delete translator;
}

/***************************************************************
*Description:
    mainwinLanguageChanged() 主窗口语言切换SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::mainwinLanguageChanged()
{
   //ui->retranslateUi(this);
    this->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
    ui->actionLanguage_En->setText(QApplication::translate("MainWindow", "\350\213\261\350\257\255(&E)", 0));
    ui->actionLanguage_En->setShortcut(QApplication::translate("MainWindow", "Ctrl+E", 0));
    ui->actionLanguage_Ch->setText(QApplication::translate("MainWindow", "\347\256\200\344\275\223\344\270\255\346\226\207(&Z)", 0));
    ui->actionLanguage_Ch->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0));
    ui->actionAbout_A->setText(QApplication::translate("MainWindow", "About(&A)", 0));
    ui->actionAbout_A->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0));
    ui->actionLanguage_Kr->setText(QApplication::translate("MainWindow", "\351\237\251\350\257\255(&K)", 0));
    ui->actionLanguage_Kr->setShortcut(QApplication::translate("MainWindow", "Ctrl+K", 0));
    ui->table1LabelFont1->setText(QApplication::translate("MainWindow", "\345\255\227\345\272\2231    ", 0));
    ui->table1ButtonOpenFont1->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table1LabelFont2->setText(QApplication::translate("MainWindow", "\345\255\227\345\272\2232    ", 0));
    ui->table1ButtonOpenFont2->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table1LabelFont3->setText(QApplication::translate("MainWindow", "\345\255\227\345\272\2231", 0));
    ui->table1ButtonClearAll->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\211\200\346\234\211", 0));
    ui->table1LabelFont4->setText(QApplication::translate("MainWindow", "\345\255\227\345\272\2232", 0));
    ui->table1ButtonAdd->setText(QString());
    ui->table1ButtonDelete->setText(QString());
    ui->table1ButtonSave->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272", 0));
    ui->table1LabelFontTaget->setText(QApplication::translate("MainWindow", "\350\246\201\347\224\237\346\210\220\347\232\204\345\255\227\345\272\223", 0));
    ui->table1ButtonClearTarget->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\345\210\227\350\241\250", 0));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab), QApplication::translate("MainWindow", "\350\243\201\345\211\252\345\220\210\345\271\266", 0));

    ui->talbe2Label1->setText(QApplication::translate("MainWindow", "\345\255\227\345\272\223", 0));
    ui->table2ButtonFontOpen->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table2Label2->setText(QApplication::translate("MainWindow", "\345\255\227\345\275\242", 0));
    ui->table2ButtonGoto->setText(QApplication::translate("MainWindow", "\350\267\263\350\275\254\345\210\260", 0));
    ui->table2ButtonBmpCopy->setText(QString());
    ui->table2PushbuttonSaveChar->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\255\227\347\254\246", 0));
    ui->table2ButtonNewFont->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272", 0));
    ui->table2Label3->setText(QApplication::translate("MainWindow", "bmp \345\233\276\347\211\207", 0));
    ui->table2ButtonBmpOpen->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table2Label4->setText(QApplication::translate("MainWindow", "\347\201\260\345\272\246\351\230\210\345\200\274", 0));
    ui->table2ButtonGraySet->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256", 0));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab_2), QApplication::translate("MainWindow", "\345\255\227\347\254\246\344\277\256\346\224\271", 0));
    ui->menu_L->setTitle(QApplication::translate("MainWindow", "\350\257\255\350\250\200(&L)", 0));
    ui->menuAbout_A->setTitle(QApplication::translate("MainWindow", "\345\270\256\345\212\251(&H)", 0));

    ui->groupBox->setTitle(QApplication::translate("MainWindow", "ts\350\275\254minigui\351\205\215\347\275\256\346\226\207\344\273\266", 0));
    ui->table3LabelTsOpen->setText(QApplication::translate("MainWindow", "ts \346\226\207\344\273\266", 0));
    ui->table3ButtonOpenTs->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));  
    ui->table3ButtonTsToGui->setText(QApplication::translate("MainWindow", "\350\275\254\346\215\242", 0));
    ui->groupBox_2->setTitle(QApplication::translate("MainWindow", "minigui\351\205\215\347\275\256\346\226\207\344\273\266\350\275\254ts", 0));
    ui->table3TsSourceLabel->setText(QApplication::translate("MainWindow", "ts\345\216\237\346\226\207\344\273\266", 0));
    ui->table3ButtonOpenSourceTs->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table3LabelGuiOpen->setText(QApplication::translate("MainWindow", "Gui\351\205\215\347\275\256 ", 0));
    ui->table3ButtonOpenGui->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
    ui->table3TsInfoLabel2->setText(QApplication::translate("MainWindow", "ts context\344\277\241\346\201\257", 0));
    ui->table3ButtonGuiToTs->setText(QApplication::translate("MainWindow", "\350\275\254\346\215\242", 0));
    ui->table3MiniguiInfoLabel->setText(QApplication::translate("MainWindow", "minigui \351\205\215\347\275\256\344\277\241\346\201\257", 0));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab_3), QApplication::translate("MainWindow", "\351\205\215\347\275\256\346\226\207\344\273\266", 0));

}

/***************************************************************
*Description:
    changeLanguageToEnglish() 切换为 英文 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::changeLanguageToEnglish()
{
    qDebug()<< "English";
    if (LANGUAGE_ENGLISH == Config::curLanguage)
    {
        return ;
    }
    Config::curLanguage = LANGUAGE_ENGLISH;
    Config::saveConfig();

    if (false == translator->load(LANGUAGE_ENGLISH_FILE))
    {
        qDebug()<< "load language error!!";
        return;
    }
    qApp->installTranslator(translator);

    emit languageChanged();
}

/***************************************************************
*Description:
    changeLanguageToChinese() 切换为 中文 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::changeLanguageToChinese()
{
    qDebug()<< "Chinese";
    if (LANGUAGE_CHINESE == Config::curLanguage)
    {
        return ;
    }
    Config::curLanguage = LANGUAGE_CHINESE;
    Config::saveConfig();

    if (false == translator->load(LANGUAGE_CHINESE_FILE))
    {
        qDebug()<< "load language error!!";
        return;
    }
    qApp->installTranslator(translator);

    emit languageChanged();
}


/***************************************************************
*Description:
    changeLanguageToKorean() 切换为 韩语 SLOT
*Input:
    无
*Output:
    无
*Return:
    无
****************************************************************/
void MainWindow::changeLanguageToKorean()
{
    qDebug()<< "Korean";
    if (LANGUAGE_KOREAN == Config::curLanguage)
    {
        return ;
    }
    Config::curLanguage = LANGUAGE_KOREAN;
    Config::saveConfig();

    if (false == translator->load(LANGUAGE_KOREAN_FILE))
    {
        qDebug()<< "load language error!!";
        return;
    }
    qApp->installTranslator(translator);

    emit languageChanged();
}



void MainWindow::showAboutWin()
{
    aboutWindow->show();
}

