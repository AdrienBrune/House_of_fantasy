#include "win_loadinggamescreen.h"
#include "ui_win_loadinggamescreen.h"

Win_LoadingGameScreen::Win_LoadingGameScreen(QWidget * parent) :
    QWidget(parent),
    mImageIndex(0),
    ui(new Ui::Win_LoadingGameScreen)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
}

Win_LoadingGameScreen::~Win_LoadingGameScreen()
{
    delete ui;
}

void Win_LoadingGameScreen::setImage(int index)
{
    mImageIndex = index;
}

void Win_LoadingGameScreen::updateLoadingProgress(quint8 value)
{
    ui->loading->setValue(value);
}

void Win_LoadingGameScreen::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#272727"));
    painter.drawRect(QRect(0,0,width(),height()));

    QRect cadre(ui->image->x(), ui->image->y(), ui->image->width(), ui->image->height());
    QRect image;

    if(cadre.width() > cadre.height()*1.6)
    {
        image.setRect(cadre.x()+(cadre.width()-cadre.height()*1.6)/2, cadre.y(), cadre.height()*1.6, cadre.height());
    }
    else
    {
        image.setRect(cadre.x(), cadre.y()+(cadre.height()-cadre.width()/1.6), cadre.width(), cadre.width()/1.6);
    }

    painter.drawPixmap(image, QPixmap(":/graphicItems/loadingImage.png"), QRect((mImageIndex)*1725, 0, 1725, 1080));
}
