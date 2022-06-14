#ifndef FRAG_INTERFACE_POTIONPREFERENCIES_H
#define FRAG_INTERFACE_POTIONPREFERENCIES_H

#include <QWidget>
#include <QPainter>
#include <QComboBox>
#include <QDebug>
#include "consumable.h"



class Custom_ComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit Custom_ComboBox(QWidget * parent = nullptr, int currentIndex = 0);
    ~Custom_ComboBox();
public:
    int getLastIndex();
    void setLastIndex(int);
    void setIndex(int);
private:
    int mLastIndex;
};





namespace Ui {
class Frag_Interface_PotionPreferencies;
}

class Frag_Interface_PotionPreferencies : public QWidget
{
    Q_OBJECT
public:
    explicit Frag_Interface_PotionPreferencies(QWidget *parent = nullptr, QList<Consumable*> list = {});
    ~Frag_Interface_PotionPreferencies();
signals:
    void sig_potionPreferenciesChanged();
private slots:
    void currentIndexChangedSelection_1(int);
    void currentIndexChangedSelection_2(int);
    void currentIndexChangedSelection_3(int);
public:
    QList<int> getPotionPreferencies();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QList<Custom_ComboBox*> mComboBoxList;
    QList<int> mList;
private:
    Ui::Frag_Interface_PotionPreferencies *ui;
};

#endif // FRAG_INTERFACE_POTIONPREFERENCIES_H
