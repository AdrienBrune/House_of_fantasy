#ifndef W_SPELLLISTSELECTION_H
#define W_SPELLLISTSELECTION_H

#include <QWidget>

#include "hero.h"
#include "skill.h"
#include "frag_speel.h"

namespace Ui {
class W_SpellListSelection;
}

class W_SpellListSelection : public QWidget
{
    Q_OBJECT

public:
    explicit W_SpellListSelection(QWidget * parent = nullptr, Hero * hero = nullptr);
    ~W_SpellListSelection();

signals:
    void sig_spellClicked(Skill*);

public:
    void enable(bool);

private:
    Hero * mHero;
    QList<Frag_Speel*> mSpellList;

private:
    Ui::W_SpellListSelection *ui;
};

#endif // W_SPELLLISTSELECTION_H
