#include "ctrwindow.h"
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Qt animates combo box / menu / tooltip popups by grabbing what is on screen. The map is
    // rendered with OpenGL and comes out black in such grabs on Windows -> black flash.
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateMenu, false);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, false);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);
    a.setOrganizationName("AdrienBrune");
    a.setApplicationName("HouseOfFantasy");
    CTRWindow w;

    return a.exec();
}
