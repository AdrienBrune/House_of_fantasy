#include "frag_save.h"
#include "ui_frag_save.h"

Frag_Save::Frag_Save(QWidget * parent, Save * save) :
    QWidget(parent),
    mSave(save),
    mHover(false),
    mSelected(false),
    mIsSelectable(true),
    ui(new Ui::Frag_Save)
{
    ui->setupUi(this);
    ui->saveImage->setScaledContents(true);

    if(save)
    {
        ui->saveName->setText(mSave->getName());
        ui->saveImage->setPixmap(mSave->getHero()->getImage());
    }
}

Frag_Save::~Frag_Save()
{
    delete ui;
}

void Frag_Save::onUnfocusExcept(Save * save)
{
    if(mSave != save)
    {
        mSelected = false;
        ui->saveName->setStyleSheet("QLabel{ padding-left:20px; color:#EEEEEE; }");
        ui->label->setStyleSheet("QLabel{ color:#EEEEEE; }");
    }
    update();
}

void Frag_Save::setSave(Save * save)
{
     ui->saveImage->setPixmap(save->getHero()->getImage());
     ui->saveName->setText(save->getName());
}

void Frag_Save::setSelectable(bool mode)
{
    mIsSelectable = mode;
}

void Frag_Save::paintEvent(QPaintEvent*)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing);

     if(mSelected && mIsSelectable)
     {
         mHover ? painter.setBrush(QBrush("#EEEEEE")) : painter.setBrush(QBrush("#DDDDDD"));
         painter.drawRect(0,0,width(),height());
     }
     else
     {
         painter.setOpacity(0.9);
         mHover ? painter.setBrush(QBrush("#3c3c3c")) : painter.setBrush(QBrush("#2c2c2c"));
         painter.drawRect(0,0,width(),height());
     }
}

void Frag_Save::leaveEvent(QEvent*)
{
    if(mIsSelectable)
        mHover = false;
}

void Frag_Save::enterEvent(QEvent*)
{
    if(mIsSelectable)
        mHover = true;
}

void Frag_Save::mouseReleaseEvent(QMouseEvent*)
{
    if(mIsSelectable)
    {
        if(mSelected)
        {
         mSelected = false;
         emit sig_unselectSave();
         ui->saveName->setStyleSheet("QLabel{ padding-left:20px; color:#EEEEEE; }");
         ui->label->setStyleSheet("QLabel{ color:#EEEEEE; }");
        }
        else
        {
         mSelected = true;
         emit sig_selectSave(mSave);
         ui->saveName->setStyleSheet("QLabel{ padding-left:20px; color:#2c2c2c; }");
         ui->label->setStyleSheet("QLabel{ color:#2c2c2c; }");
        }
        update();
    }
}
