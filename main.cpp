#include "widget.h"
#include <QApplication>
#include<QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
//    QSplashScreen *splash=new QSplashScreen;
//    QPixmap pix(":/images/beijing.jpg");
//    splash->showMessage(" ");
//    splash->setPixmap(pix);
//    splash->show();
//    a.processEvents();//这样可以保证程序一直可以响应鼠标键盘上事件
//    splash->finish(&w);//启动画面在窗口w创建完成以后结束

    w.show();

    return a.exec();
}
