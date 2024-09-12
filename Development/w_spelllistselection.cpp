#include "w_spelllistselection.h"
#include "ui_w_spelllistselection.h"

#include "frag_speel.h"
#include "entitieshandler.h"

W_SpellListSelection::W_SpellListSelection(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::W_SpellListSelection)
{
    ui->setupUi(this);

    Hero * hero = EntitesHandler::getInstance().getHero();

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    ui->scrollAreaWidgetContents->setAutoFillBackground(false);

    for(int i=0; i < SpellSkill::NbSpells; i++)
    {
        if(hero->getSpellList()[i]->isUnlock())
        {
            Frag_Speel * spellInfo = new Frag_Speel(this, hero->getSpellList()[i]);
            connect(spellInfo, SIGNAL(sig_spellClicked(Skill*)), this, SIGNAL(sig_spellClicked(Skill*)));
            ui->layoutSpells->addWidget(spellInfo);
            mSpellList.append(spellInfo);
        }
    }
}

W_SpellListSelection::~W_SpellListSelection()
{
    delete ui;
}

void W_SpellListSelection::enable(bool toggle)
{
    for(Frag_Speel * button : mSpellList)
    {
        button->enable(toggle);
    }
}
