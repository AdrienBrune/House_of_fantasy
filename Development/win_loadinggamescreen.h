#ifndef WIN_LOADINGGAMESCREEN_H
#define WIN_LOADINGGAMESCREEN_H

#include <QMainWindow>
#include <QPainter>
#include "hero.h"

namespace Ui {
class Win_LoadingGameScreen;
}

class Win_LoadingGameScreen : public QWidget
{
    Q_OBJECT

public:
    explicit Win_LoadingGameScreen(QWidget *parent = nullptr);
    ~Win_LoadingGameScreen();

public:
    void setImage(int);

public slots:
    void updateLoadingProgress(quint8);

protected:
    void paintEvent(QPaintEvent*);

private:
    int mImageIndex;

private:
    Ui::Win_LoadingGameScreen *ui;
};

#endif // WIN_LOADINGGAMESCREEN_H
