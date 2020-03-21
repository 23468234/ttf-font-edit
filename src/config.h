#ifndef CONFIG_H
#define CONFIG_H

#define LANGUAGE_CHINESE_FILE   "../qt_font_manager/fontManager_zh_CN.qm"
#define LANGUAGE_ENGLISH_FILE   "../qt_font_manager/fontManager_en_US.qm"
#define LANGUAGE_KOREAN_FILE    "../qt_font_manager/fontManager_ko_Kp.qm"

#define CONFIG_FILE "../qt_font_manager/qtFontConfig.bat"

typedef enum
{
    LANGUAGE_ENGLISH,
    LANGUAGE_CHINESE,
    LANGUAGE_KOREAN,
    LANGUAGE_INVALID = 0xFF,
}LANGUAGE_TYPE;


class Config
{
public:
    Config();
    ~Config();

    static LANGUAGE_TYPE curLanguage;

    static bool saveConfig();
    static bool readConfig();
    static void globalInit();

};

#endif
