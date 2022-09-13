#include "w_quicktooldrawer.h"

W_QuickToolDrawer::W_QuickToolDrawer(QWidget *parent, Tool * tool) :
    QWidget(parent),
    mTool(tool),
    mButtonShowTool(nullptr)
{
#define WIDGET_HEIGHT   60
#define WIDGET_WIDTH    100

    mButtonShowTool = new QPushButton(this);
    mButtonShowTool->setText("<<");
    mButtonShowTool->setFont(QFont("Papyrus"));
    mButtonShowTool->setStyleSheet("QPushButton{color:#3c3c3c;background-color:#c6c6c6;border:2px solid #3c3c3c;padding-right:10px;padding-left:10px;border-radius:4px;}");
    setGeometry(0, 0, WIDGET_WIDTH, WIDGET_HEIGHT);
    mButtonShowTool->adjustSize();
    mButtonShowTool->setGeometry(0, (WIDGET_HEIGHT-mButtonShowTool->height())/2, mButtonShowTool->width(), mButtonShowTool->height());
    connect(mButtonShowTool, SIGNAL(clicked()), this, SLOT(useTool()));
}

W_QuickToolDrawer::~W_QuickToolDrawer()
{

}

void W_QuickToolDrawer::useTool()
{
    if(mTool)
        emit sig_useTool(mTool);
}

QPushButton *W_QuickToolDrawer::getButton()
{
    return mButtonShowTool;
}

void W_QuickToolDrawer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(!mTool)
        return;

    QRect area = QRect(mButtonShowTool->width() + 10, (height()-mButtonShowTool->height())/2, 40, 40);
    painter.drawPixmap(area, mTool->getImage());
}
