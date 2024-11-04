#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWidget>
#include <QThread>
#include <QDialog>

#include "hero.h"
#include "monster.h"
#include "mapitem.h"
#include "mapevent.h"
#include "map.h"
#include "soundmanager.h"
#include "w_animation_exploration.h"
#include "equipment.h"
#include "win_inventory.h"
#include "win_gear.h"
#include "win_interface_trading.h"
#include "win_fight.h"
#include "w_usetool.h"
#include "w_animation_night.h"
#include "itemgenerator.h"
#include "win_skills.h"
#include "win_menu.h"
#include "save.h"
#include "win_loadinggamescreen.h"
#include "w_quicktooldrawer.h"
#include "w_messagelogger.h"
#include "w_herostats.h"

namespace Ui {
class CTRWindow;
}

class CTRWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CTRWindow(QWidget *parent = nullptr);
    ~CTRWindow();
signals:
    void sig_loadingGameUpdate(quint8);
private slots:
    void onStartGame(Save*);
    void generateNewGame();
    void GoToMonsterFight(Monster*);
    void scrollSceneView();
    void displayMonsterData(Monster*);
    void displayCalamitySpawned();
    void fightResult(Character*);
    void explorationCompleted();
    void on_Inventory_clicked();
    void hideInventary();
    void on_Gear_clicked();
    void hideInventaryGear();
    void hideFightWindow();
    void hideSkillWindow();
    void showReplenish(QObject*);
    void useTool(Tool*);
    void useScroll(Scroll*);
    void unuseToolIfThrown(Item * item);
    void unuseTool();
    void unuseMapTool();
    void showBagFull();
    void showItemOnGround(Item*);
    void showItemPicked(Item*);
    void openInterface(QGraphicsItem*);
    void showPNJinfo(QGraphicsItem*);
    void on_Skills_clicked();
    void onPeriodicalEvents();
    void onQuitGame();
    void on_buttonQuit_clicked();
    void showQuickToolDrawer(Tool*);
    void hideQuickToolDrawer();
    void onCloseMessageLogger();
    void on_Map_clicked();

public:
    void ShowPopUpInfo(LogMessage*);
    void displayInterfaceTrading(Win_Interface_Trading*);
    void setButtonsEnable(bool);
protected :
    bool eventFilter(QObject *, QEvent *);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    void closeGame();
private:
    Map * mMap;
    Hero * mHero;
    SoundManager * mSoundManager;
    bool heroIsSearching;
    QTimer * t_unfreezeMap;
    QTimer * t_PeriodicalEvents;
    Save * mCurrentSave;
    quint8 mLoadingAvancement;
    bool mHeroMovementAllowed;
private:
    Win_Menu * w_menu;
    Win_LoadingGameScreen * w_loadingScreen;
    W_HeroStats * w_heroStats;
    W_Animation_exploration * w_explorationLoading;
    Win_Inventory * w_inventory;
    Win_Gear * w_gear;
    Win_Fight * w_fight;
    Win_Interface_Trading * w_trading;
    Win_Skills * w_skill;
    W_UseTool * w_tool;
    W_UseToolMap * w_map;
    W_QuickToolDrawer * w_quickItemDrawer;
    W_MessageLogger * w_messageLogger;
    W_Animation_Night * w_night;
public:
    Ui::CTRWindow *ui;
};




class ConfirmationDialog : public QDialog {
    Q_OBJECT

public:
    ConfirmationDialog(QWidget *parent = nullptr) : QDialog(parent) {
        // Configuration pour occuper tout l'écran
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(parent->size());

        // Fond semi-transparent couvrant tout l'écran
        QWidget *overlay = new QWidget(this);
        overlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.75);");
        overlay->setFixedSize(size());

        // Zone centrale de la popup
        QWidget *centerPopup = new QWidget(this);
        centerPopup->setStyleSheet(
            "background-color: #2e2e2e; "
            "border-radius: 10px;"
        );
        centerPopup->setFixedSize(400, 150);

        QLabel *label = new QLabel("Voulez-vous vraiment quitter la partie ?", centerPopup);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: white; font-size: 16px;");

        QPushButton *confirmButton = new QPushButton("Confirmer", centerPopup);
        QPushButton *cancelButton = new QPushButton("Annuler", centerPopup);

        // Connexions des boutons
        connect(confirmButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

        // Personnalisation des boutons
        QString buttonStyle = R"(
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border-radius: 5px;
            padding: 8px 16px;
        )";
        confirmButton->setStyleSheet(buttonStyle);
        cancelButton->setStyleSheet(buttonStyle);

        // Mise en page du contenu central
        QVBoxLayout *centerLayout = new QVBoxLayout(centerPopup);
        centerLayout->addWidget(label);

        QHBoxLayout *buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(confirmButton);
        buttonLayout->addWidget(cancelButton);
        centerLayout->addLayout(buttonLayout);

        // Centrer la popup dans la fenêtre
        centerPopup->move((width() - centerPopup->width()) / 2, (height() - centerPopup->height()) / 2);
    }
};

#endif // MAINWINDOW_H
