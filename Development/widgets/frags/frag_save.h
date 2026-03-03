 #ifndef FRAG_SAVE_H
 #define FRAG_SAVE_H

 #include <QWidget>
 #include <QPainter>

 #include "save.h"

 namespace Ui {
 class Frag_Save;
 }

 class Frag_Save : public QWidget
 {
     Q_OBJECT

 public:
     explicit Frag_Save(QWidget *parent = nullptr, Save * save = nullptr);
     ~Frag_Save();

 signals:
     void sig_selectSave(Save*);
     void sig_unselectSave();

 public slots:
    void onUnfocusExcept(Save*);

 public:
     void setSave(Save*);
     void setSelectable(bool);

 protected:
     void paintEvent(QPaintEvent*);
     void leaveEvent(QEvent*);
     void enterEvent(QEvent*);
     virtual void mouseReleaseEvent(QMouseEvent * event);

 private:
     Save * mSave;
     bool mHover;
     bool mSelected;
     bool mIsSelectable;

 private:
     Ui::Frag_Save *ui;
 };

 #endif // FRAG_SAVE_H
