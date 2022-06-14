#include "frag_speel.h"
#include "ui_frag_speel.h"

Frag_Speel::Frag_Speel(QWidget * parent, Skill * skill) :
    QPushButton(parent),
    mSpell(skill),
    mHover(false),
    ui(new Ui::Frag_Speel)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    ui->image->setScaledContents(true);
    ui->image->setPixmap(skill->getImage(1));
    ui->title->setText(skill->getName());

    SpellSkill * spell = dynamic_cast<SpellSkill*>(skill);
    if(spell)
    {
        ui->manaCost->setText(QString("Coût en mana : %1").arg(spell->getManaCost()));
        ui->capacity->setText(QString("%1 : %2").arg(spell->getCapacityLabel()).arg(spell->getCapacity()));
    }
}

Frag_Speel::~Frag_Speel()
{
    delete ui;
}

void Frag_Speel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setOpacity(0.9);

    mHover ? painter.setBrush(QBrush("#3c3c3c")) : painter.setBrush(QBrush("#2c2c2c"));
    painter.drawRect(0,0,width(),height());

    painter.setPen(QPen(QBrush("#FFFFFF"), 1));
    painter.drawLine(ui->image->x() + ui->image->width()/2, ui->image->y() + ui->image->height()/2 + 5,
                     ui->capacity->x() + ui->capacity->width() + 5, ui->image->y() + ui->image->height()/2 + 5);
}

void Frag_Speel::leaveEvent(QEvent*)
{
    mHover = false;
}

void Frag_Speel::enterEvent(QEvent*)
{
    mHover = true;
}

void Frag_Speel::mouseReleaseEvent(QMouseEvent*)
{
    emit sig_spellClicked(mSpell);
}

void Frag_Speel::enable(bool toggle)
{
    setEnabled(toggle);
}
