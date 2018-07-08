#include "SyaringanWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SyaringanWidget w;
    w.show();

    return a.exec();
}
