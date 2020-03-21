#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTranslator>

#include "common.h"
#include "binfont_parse.h"
#include "bmpparse.h"
#include "config.h"
#include "aboutwin.h"
#include "tsparse.h"
#include "guicfgparse.h"
#include "characterstatistics.h"
#include "ttfmodify.h"
#include "building.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*Tab1 合并裁剪*/
    void tab1Init();
    void tab1Deinit();
    void mainWinInit();
    void fillListView1();
    void fillListView2();
    void fillListView3();
    void clearListView3();

   /*Tab2 字符修改*/
    void tab2Init();
    void tab2Deinit();

    /*Tab3 文件转换*/
    void tab3Init();
    void tab3Deinit();

    /*Tab4 字符统计*/
    void tab4Init();
    void tab4Deinit();

    /*Tab5 矢量字体裁剪*/
    void tab5Init();
    void tab5Deinit();
    QByteArray getListViewMask();
    QMap<quint16, bool> precisionCutInputUnicode();

    /*标题栏*/
    void menuInit();
    void menuDeinit();
    QString getFrameTitle(int title);

private:
    Ui::MainWindow *ui;
    AboutWin * aboutWindow;

    /*Tab1 合并裁剪*/
    QStandardItemModel * mulu1Model ;   //listView1模型
    QStandardItemModel * mulu1Mode2 ;   //listView2模型
    QStandardItemModel * mulu1Mode3 ;   //listView3模型

    PCHARSET_INFO_S list_font_select1;  //第一个字库所选取的字符集链表
    PCHARSET_INFO_S list_font_select2;  //第二个字库所选取的字符集链表

    FONTFILE_INFO_S font_file_info1;    //第一个字库打开的文件信息；
    FONTFILE_INFO_S font_file_info2;    //第二个字库打开的文件信息；
    QString combFontDir; //table1 新建字库路径

    /*Tab2 字符修改*/
    BmpParse *bmpFile;
    FONTFILE_INFO_S font_file_info3;
    bool isLeftFontWidgetShowedchar;    //左边是否已经选择了字符
    quint16 curInputCode;
    QString fontOpenDir;
    QString fontSaveDir;
    QString bmpOpenDir;

    /*Tab3 文件互转*/
    GuiCfgParse* guiParse;
    TsParse* tsParse;
    TsParse* tsSourceParse;
    QString tsSourceDir;
    QString tsOpenDir;
    QString guiOpenDir;
    QString tsSaveDir;
    QString guiSaveDir;

    /*Tab4 字符统计*/
    CharacterStatistics *table4CharStatistics;
    QString dicFileDir;

    /*Tab5 矢量字体裁剪*/
    TtfModify * ttfModify;
    bool isTtfOpenOk;
    CharacterStatistics * table5CharStatistics;
    QStandardItemModel * table5Mode1 ;   //listView3模型
    QString ttfOpenDir;
    QString ttfSaveDir;
    Building * buildingWin;

    /*标题栏*/
    QTranslator *translator;

signals:
    void languageChanged(); //语言切换
    void ttfFontBuild(QString);

private slots:
    /*tab1 合并裁剪*/
    void fontFileOpen1();
    void fontFileOpen2();
    void newFontAdd();
    void newFontDelete();
    void newFontSave();
    void listView3Clear();
    void listViewAllClear();

    /*tab2 字符修改*/
    void table2OpenButton();
    void table2IntputUnicode();
    void table2OpenBmp();
    void table2SetThreshold();
    void table2CopyBmpToFont();
    void table2SaveFontFile();
    void table2SaveChar();

    /*tab3 ts gui互转*/
    void table3OpenTs();
    void table3transformGuiToTs();
    void table3OpenTsSource();
    void table3OpenGui();
    void table3transformTsToGui();

    /*tab4 gui字符统计*/
    void table4OpenGuiDicFile();

    /*tab5 ttf字体裁剪*/
    void table5TtfFileOpen();
    void table5TtfFileSave();
    void table5ButtonShift();
    void table5ButtonSelectAll();
    void table5RadioButtonChangeCutWay();
    void buildTtfFile(QString fileName);


    /*menu 标题栏*/
    void showAboutWin();
    void changeLanguageToEnglish();
    void changeLanguageToChinese();
    void changeLanguageToKorean();


    /*language 语言*/
    void mainwinLanguageChanged();

};

#endif // MAINWINDOW_H
