#include "win_heroselection.h"
#include "ui_win_heroselection.h"

#define INDEX_SWORDMAN  0
#define INDEX_ARCHER    1
#define INDEX_WIZARD    2

Win_HeroSelection::Win_HeroSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Win_HeroSelection)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    ui->image->setScaledContents(true);
    ui->image->setPixmap(SwordMan("").getImage());
}

Win_HeroSelection::~Win_HeroSelection()
{
    delete ui;
}

void Win_HeroSelection::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#EEEEEE"));
    painter.drawRect(0,0,width(),height());

    painter.setBrush(QBrush("#272727"));
    painter.drawRect(2,2,width()-4,height()-4);
}

void Win_HeroSelection::on_heroClass_currentIndexChanged(int index)
{
    switch(index)
    {
        case INDEX_SWORDMAN:
            ui->image->setPixmap(SwordMan("").getImage());
            break;

        case INDEX_ARCHER:
            ui->image->setPixmap(Archer("").getImage());
            break;

        case INDEX_WIZARD:
            ui->image->setPixmap(Wizard("").getImage());
            break;

        default:
            break;
    }
}

void Win_HeroSelection::on_buttonReturn_clicked()
{
    emit sig_HeroSelected(nullptr);
    close();
}

void Win_HeroSelection::on_buttonContinue_clicked()
{
    if(!ui->heroName->text().length())
    {
        ui->heroName->setStyleSheet("QLineEdit{ border:1px solid #CC3333; selection-background-color: #11FFFFFF; background-color:#11FFFFFF; color:#EEEEEE; }");
        QTimer::singleShot(1000, this, [&]{ui->heroName->setStyleSheet("QLineEdit{ border: 1px solid #888888; selection-background-color: #11FFFFFF; background-color:#11FFFFFF; color:#EEEEEE; }");});
        return;
    }

    if(!ui->saveName->text().length())
    {
        ui->saveName->setStyleSheet("QLineEdit{ border: 1px solid #CC3333; };");
        QTimer::singleShot(1000, this, [&]{ ui->saveName->setStyleSheet("QLineEdit{ border: 1px solid #888888; };"); });
        return;
    }

    Hero * hero = Hero::getInstance(static_cast<Hero::HeroClasses>(ui->heroClass->currentIndex()));
    hero->setName(ui->heroName->text());
    Save * save = new Save();
    save->setHero(hero);
    save->setChest(new HeroChest);
    save->setName(ui->saveName->text());
    emit sig_HeroSelected(save);
    close();
}
