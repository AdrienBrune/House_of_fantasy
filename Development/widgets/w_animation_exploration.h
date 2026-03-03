#ifndef W_SEARCHLOADING_H
#define W_SEARCHLOADING_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class W_Animation_exploration : public QWidget
{
    Q_OBJECT

public:
    explicit W_Animation_exploration(QWidget *parent = nullptr);
    ~W_Animation_exploration();
signals:
    void sig_explorationCompleted();
private slots:
    void animate();
protected:
    void paintEvent(QPaintEvent *event);
private:
    double mNextStep;
};

#endif // W_SEARCHLOADING_H
