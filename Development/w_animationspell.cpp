#include "w_animationspell.h"
#include "ui_w_spellanimation.h"

#include <QtDebug>

W_AnimationSpell::W_AnimationSpell(QWidget * parent, quint8 spell) :
    QWidget(parent),
    mSpellIndex(spell),
    mAnimation(0),
    ui(new Ui::W_SpellAnimation)
{
    ui->setupUi(this);
    tAnimation = new QTimer(this);
    connect(tAnimation, &QTimer::timeout, this, &W_AnimationSpell::animate);
    tAnimation->start(150);
    show();
}

W_AnimationSpell::~W_AnimationSpell()
{
    delete ui;
}

void W_AnimationSpell::animate()
{
    if(mAnimation > 9)
    {
        tAnimation->stop();
        emit sig_hideAnimation();
    }
    else
    {
        mAnimation++;
        update();
    }
}

void W_AnimationSpell::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(QRect(0,0,width(),height()), QPixmap(QString(":/animation/animation_spell_%1.png").arg(mSpellIndex)), QRect(mAnimation*700,0,700,700));
}
