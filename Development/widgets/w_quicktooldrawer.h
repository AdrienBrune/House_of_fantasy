#ifndef W_QUICKTOOLDRAWER_H
#define W_QUICKTOOLDRAWER_H

#include <QWidget>
#include <QPushButton>
#include "item.h"

class W_QuickToolDrawer : public QWidget
{
    Q_OBJECT

public:
    explicit W_QuickToolDrawer(QWidget *parent = nullptr, Tool * tool = nullptr);
    ~W_QuickToolDrawer();
signals:
    void sig_useTool(Tool*);
private slots:
    void useTool();
public:
    QPushButton * getButton();
protected:
    void paintEvent(QPaintEvent*);
private:
    Tool * mTool;
    QPushButton * mButtonShowTool;
};

#endif // W_QUICKTOOLDRAWER_H
