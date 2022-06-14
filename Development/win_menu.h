 #ifndef WIN_MENU_H
 #define WIN_MENU_H

 #include <QWidget>
 #include <QPainter>
 #include <QTimer>

 #include "hero.h"
 #include "village.h"
 #include "save.h"
 #include "frag_save.h"

 namespace Ui {
 class Win_Menu;
 }

 class Win_Menu : public QWidget
 {
     Q_OBJECT

 public:
     explicit Win_Menu(QWidget *parent = nullptr);
     ~Win_Menu();

 signals:
     void sig_startGame(Save*);
     void sig_quitGame();

 private slots:
     void onAnimation();
     void onLoadSave(Save*);
     void onSelectHero(Save*);

 protected:
     void paintEvent(QPaintEvent*);

 private slots:
     void on_buttonNewGame_clicked();
     void on_buttonLoadGame_clicked();
     void on_buttonParameters_clicked();
     void on_buttonQuit_clicked();
     void on_buttonRemoveSave_clicked();

public:
     void enableButtons(bool);

 private:
     qint16 mAnimationIndex;
     qint8 mAnimationStep;
     QTimer * t_animation;
     Save * mSaveSelected;
     Frag_Save * mSaveDisplayed;

 private:
     Ui::Win_Menu *ui;
 };

 #endif // WIN_MENU_H
