#ifndef W_ANIMATIONSPELL_H
#define W_ANIMATIONSPELL_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

#include "skill.h"

namespace Ui {
class W_SpellAnimation;
}

class W_AnimationSpell : public QWidget
{
    Q_OBJECT

public:
    explicit W_AnimationSpell(QWidget * parent = nullptr, quint8 = 0);
    ~W_AnimationSpell();

signals:
    void sig_hideAnimation();

private slots:
    void animate();

protected:
    void paintEvent(QPaintEvent*);

private:
    quint8 mSpellIndex;
    uint8_t mAnimation;
    QTimer * tAnimation;

private:
    Ui::W_SpellAnimation *ui;
};

#endif // W_ANIMATIONSPELL_H
