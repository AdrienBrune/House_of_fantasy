#include "ctrwindow.h"
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CTRWindow w;

    return a.exec();
}
