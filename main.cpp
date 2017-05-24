#include "logindialog.h"
#include <QApplication>
#include "myhelper.h"

//软件界面皮肤颜色风格
enum SKIN_COLOR
{
    SKIN_BLACK,//黑色风格
    SKIN_BLUE,//蓝色风格
    SKIN_GRAY,//灰色风格
    SKIN_NAVY//天蓝色风格
};

//软件界面皮肤颜色风格选择
#define SOFTWARE_SKIN SKIN_BLUE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myHelper::SetUTF8Code();
    switch (SOFTWARE_SKIN)
    {
    case SKIN_BLACK://黑色风格
        myHelper::SetStyle("black");
        break;
    case SKIN_BLUE://蓝色风格
        myHelper::SetStyle("blue");
        break;
    case SKIN_GRAY://灰色风格
        myHelper::SetStyle("gray");
        break;
    case SKIN_NAVY://天蓝色风格
        myHelper::SetStyle("navy");
        break;
    }
    myHelper::SetChinese();

    LoginDialog w;
    w.show();

    return a.exec();
}
