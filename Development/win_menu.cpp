#include "win_menu.h"
#include "ui_win_menu.h"
#include "win_loadsave.h"
#include "hero.h"
#include "win_heroselection.h"

#define NUMBER_ANIMATION_FRAME 12

Win_Menu::Win_Menu(QWidget *parent) :
     QWidget(parent),
     mAnimationIndex(0),
     mAnimationStep(1),
     mSaveSelected(nullptr),
     mSaveDisplayed(nullptr),
     ui(new Ui::Win_Menu)
{
     ui->setupUi(this);
     enableButtons(true);
     ui->buttonLoadGame->setAttribute(Qt::WA_TranslucentBackground);
     ui->buttonNewGame->setAttribute(Qt::WA_TranslucentBackground);
     ui->buttonParameters->setAttribute(Qt::WA_TranslucentBackground);

     t_animation = new QTimer(this);
     connect(t_animation, SIGNAL(timeout()), this, SLOT(onAnimation()));
     t_animation->start(200);

     mSaveDisplayed = new Frag_Save(this, nullptr);
     mSaveDisplayed->setSelectable(false);
     ui->layoutSave->addWidget(mSaveDisplayed);
     mSaveDisplayed->hide();
     ui->buttonRemoveSave->hide();
}

Win_Menu::~Win_Menu()
{
    delete ui;
}

void Win_Menu::onAnimation()
{
     mAnimationIndex += mAnimationStep;

     switch(mAnimationIndex)
     {
         case NUMBER_ANIMATION_FRAME/2:
             mAnimationStep = -1;
             break;

         case -(NUMBER_ANIMATION_FRAME/2):
             mAnimationStep = +1;
             break;
     }

     update();
}

void Win_Menu::onLoadSave(Save * save)
{
    enableButtons(true);

    mSaveSelected = save;

    /* Change display */
    if(save)
    {
        mSaveDisplayed->setSave(save);
        mSaveDisplayed->show();
        ui->buttonRemoveSave->show();
    }
    else
    {
        mSaveDisplayed->hide();
        ui->buttonRemoveSave->hide();
    }
}

void Win_Menu::paintEvent(QPaintEvent*)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing);

     painter.setBrush(QBrush("#272727"));
     painter.drawRect(0,0,width(),height());

     painter.drawPixmap(QRect(0,0,width(),height()), QPixmap(":/graphicItems/background_menu.png"));
}

void Win_Menu::on_buttonNewGame_clicked()
{
    enableButtons(false);

    if(mSaveSelected)
    {
        emit sig_startGame(mSaveSelected);
    }
    else
    {
        Win_HeroSelection * dialogHeroSelection = new Win_HeroSelection(this);
        dialogHeroSelection->show();
        dialogHeroSelection->setGeometry(0, 0, width(), height());
        connect(dialogHeroSelection, SIGNAL(sig_HeroSelected(Save*)), this, SLOT(onSelectHero(Save*)));
    }
}

void Win_Menu::onSelectHero(Save * save)
{
    enableButtons(true);

    if(!save)
        return;

    if(!mSaveSelected)
        delete mSaveSelected;
    mSaveSelected = save;
    emit sig_startGame(mSaveSelected);
}

void Win_Menu::on_buttonLoadGame_clicked()
{
    enableButtons(false);

    Win_LoadSave loadSaves(this);
    loadSaves.setGeometry(0, 0, width(), height());
    connect(&loadSaves, SIGNAL(sig_selectSave(Save*)), this, SLOT(onLoadSave(Save*)));
    loadSaves.exec();
}

void Win_Menu::on_buttonParameters_clicked()
{

}

void Win_Menu::on_buttonQuit_clicked()
{
    emit sig_quitGame();
}

void Win_Menu::on_buttonRemoveSave_clicked()
{
    delete mSaveSelected;
    mSaveSelected = nullptr;

    ui->buttonRemoveSave->hide();
    mSaveDisplayed->hide();
}

void Win_Menu::enableButtons(bool toggle)
{
    ui->buttonLoadGame->setEnabled(toggle);
    ui->buttonNewGame->setEnabled(toggle);
    ui->buttonParameters->setEnabled(toggle);
    ui->buttonQuit->setEnabled(toggle);
    ui->buttonRemoveSave->setEnabled(toggle);
}
