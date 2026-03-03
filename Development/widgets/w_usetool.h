#ifndef W_USETOOLS_H
#define W_USETOOLS_H

#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QTimer>
#include <QCheckBox>
#include "hero.h"
#include "village.h"
#include "toolfunctions.h"
#include "item.h"

class W_UseTool : public QWidget
{
    Q_OBJECT
public:
    explicit W_UseTool(QWidget *parent = nullptr, Tool * tool = nullptr);
    ~W_UseTool(){}
public slots:
    void refresh() { update(); }
public:
    QPushButton * getExitButton() { return mExitButton; }
    Tool * getTool() { return mTool; }
    virtual void displayTool() { show(); }
    virtual void hideTool() { close(); }
protected:
    void paintEvent(QPaintEvent*) override;
protected:
    Tool * mTool;
    QPushButton * mExitButton;
};

class W_UseToolCompass : public W_UseTool
{
    Q_OBJECT
public:
    explicit W_UseToolCompass(QWidget *parent = nullptr, Tool * tool = nullptr);
    ~W_UseToolCompass(){}
protected:
    void paintEvent(QPaintEvent*) override;
private:
    QTimer mTimer;
};

class W_UseToolMap : public W_UseTool
{
    Q_OBJECT
public:
    explicit W_UseToolMap(QWidget *parent = nullptr, Tool * tool = nullptr);
    ~W_UseToolMap() {}
private slots:
    void onFilterChanged()
    {
        MapScroll * mapScroll = dynamic_cast<MapScroll*>(mTool);
        if(mapScroll)
        {
            QCheckBox *checkbox = qobject_cast<QCheckBox*>(sender());
            if(!checkbox)
                return;

            for(auto it = mFilterList.begin(); it != mFilterList.end(); ++it)
            {
                if(it.value() == checkbox)
                {
                    mapScroll->setFilter(it.key(), checkbox->isChecked());
                    break;
                }
            }
        }
        W_UseTool::refresh();
    }
public:
    void displayTool() override;
    void hideTool() override;
protected:
    void paintEvent(QPaintEvent*) override;
    void enterEvent(QEvent *event) override
    {
        for(auto checkbox : qAsConst(mFilterList))
            checkbox->setVisible(true);
        QWidget::enterEvent(event);
        update();
    }
    void leaveEvent(QEvent *event) override
    {
        for (auto checkbox : qAsConst(mFilterList))
            checkbox->setVisible(false);
        QWidget::leaveEvent(event);
        update();
    }
private:
    QMap<QString, QCheckBox*> mFilterList;
};

#endif // W_USETOOLS_H
