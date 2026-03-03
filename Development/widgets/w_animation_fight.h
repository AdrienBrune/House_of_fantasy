#ifndef W_FIGHTANIMATION_H
#define W_FIGHTANIMATION_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class W_Animation_Fight : public QWidget
{
    Q_OBJECT
public:
    explicit W_Animation_Fight(QWidget *parent = nullptr, QPixmap attackImage = QPixmap());
    ~W_Animation_Fight();
signals:
    void sig_hideAnimation();
protected:
    void paintEvent(QPaintEvent *event);
private:
    QPixmap pAttackPixmap;
};

#endif // W_FIGHTANIMATION_H
