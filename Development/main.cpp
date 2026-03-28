#include "ctrwindow.h"
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("AdrienBrune");
    a.setApplicationName("HouseOfFantasy");
    CTRWindow w;

    return a.exec();
}
