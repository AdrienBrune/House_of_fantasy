#ifndef WIN_HEROSELECTION_H
#define WIN_HEROSELECTION_H

#include <QWidget>
#include <QPainter>

#include "save.h"

namespace Ui {
class Win_HeroSelection;
}

class Win_HeroSelection : public QWidget
{
    Q_OBJECT

public:
    explicit Win_HeroSelection(QWidget *parent = nullptr);
    ~Win_HeroSelection();

signals:
    void sig_HeroSelected(Save*);

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void on_heroClass_currentIndexChanged(int index);
    void on_buttonReturn_clicked();
    void on_buttonContinue_clicked();

private:
    Ui::Win_HeroSelection *ui;
};

#endif // WIN_HEROSELECTION_H
