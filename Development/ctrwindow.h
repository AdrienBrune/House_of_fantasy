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

#include "hero.h"
#include "monster.h"
#include "mapitem.h"
#include "mapevent.h"
#include "map.h"
#include "soundmanager.h"
#include "w_dialogpannel.h"
#include "w_interface_herostats.h"
#include "w_animation_exploration.h"
#include "equipment.h"
#include "win_inventory.h"
#include "win_gear.h"
#include "win_interface_trading.h"
#include "win_fight.h"
#include "w_usetool.h"
#include "w_animation_night.h"
#include "helpdialog.h"
#include "itemgenerator.h"
#include "win_skills.h"
#include "win_menu.h"
#include "save.h"
#include "win_loadinggamescreen.h"

#define DEBUG_CLOSE_APP
#ifdef DEBUG_CLOSE_APP
    #define PRINT_DBG_CLOSE_APP(String) qDebug() << String
#else
    #define PRINT_DBG_CLOSE_APP(String)
#endif

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
    void removeCurrentPanel();
    void displayMonsterData(Monster*);
    void displayCalamitySpawned();
    void fightResult(Character*);
    void explorationCompleted();
    void on_Inventory_clicked();
    void hideInventary();
    void indicateBagFull();
    void on_Gear_clicked();
    void hideInventaryGear();
    void hideFightWindow();
    void hideSkillWindow();
    void showReplenish(QObject*);
    void useTool(Tool*);
    void useScroll(Scroll*);
    void unuseTool();
    void on_buttonHelp_clicked();
    void showBagFull();
    void showItemOnGround(Item*);
    void showItemPicked(Item*);
    void openInterface(QGraphicsItem*);
    void showPNJinfo(QGraphicsItem*);
    void on_Skills_clicked();
    void onPeriodicalEvents();
    void onQuitGame();
    void on_buttonSaveGame_clicked();

public:
    void displayPanel(W_DialogPannel*);
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
    ItemGenerator * mGameItems;
    bool heroIsSearching;
    QTimer * t_unfreezeMap;
    QTimer * t_PeriodicalEvents;
    Save * mCurrentSave;
    quint8 mLoadingAvancement;
private:
    Win_Menu * w_menu;
    Win_LoadingGameScreen * w_loadingScreen;
    W_Interface_HeroStats * w_heroStats;
    W_DialogPannel * w_panel;
    W_Animation_exploration * w_explorationLoading;
    Win_Inventory * w_inventory;
    Win_Gear * w_gear;
    Win_Fight * w_fight;
    Win_Interface_Trading * w_trading;
    Win_Skills * w_skill;
    W_UseTool * w_tool;
    QThread mLoadingThread;
public:
    Ui::CTRWindow *ui;
};

#endif // MAINWINDOW_H
