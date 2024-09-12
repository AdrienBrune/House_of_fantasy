#include "w_usetool.h"

#include "entitieshandler.h"

W_UseTool::W_UseTool(QWidget *parent, Tool * tool) :
    QWidget(parent),
    mTool(tool)
{
    hide();
    mExitButton = new QPushButton(this);
    mExitButton->setText(">>");
    mExitButton->setFont(QFont("Papyrus"));
    mExitButton->setStyleSheet("QPushButton{color:#3c3c3c;background-color:#c6c6c6;border:2px solid #3c3c3c;padding-right:10px;padding-left:10px;border-radius:4px;}");
    setGeometry(0,0,300,300);
    mExitButton->adjustSize();
    mExitButton->setGeometry(width()-mExitButton->width(), 0, mExitButton->width(), mExitButton->height());

   if(tool->getIdentifier() == TOOL_COMPASS)
   {
           QTimer * timer = new QTimer(this);
           connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
           timer->start(500);
    }
}

W_UseTool::~W_UseTool()
{

}

void W_UseTool::refresh()
{
    update();
}

QPushButton *W_UseTool::getExitButton()
{
    return mExitButton;
}

void W_UseTool::displayTool()
{
    show();
}

Tool * W_UseTool::getTool()
{
    return mTool;
}

void W_UseTool::hideTool()
{
    close();
}

void W_UseTool::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    Hero *hero = EntitesHandler::getInstance().getHero();
    Map *map = EntitesHandler::getInstance().getMap();

    int w = static_cast<int>(width()-mExitButton->height());
    int h = static_cast<int>((height()-mExitButton->height()));
    QRect area = QRect((width()-w)/2,(height()-h)/2,w,h);
    QRect compassArea = QRect(area.x() + area.width()/10, area.y() + area.height()/10, area.width()*8/10, area.height()*8/10);

    if(mTool->getIdentifier() == TOOL_COMPASS)
    {
        painter.drawPixmap(area, QPixmap(":/graphicItems/compassDisplayer.png"));
        if(!hero->isInVillage())
        {
            QPoint center(area.x() + area.width()/2, area.y() + area.height()/2);
            int angle = ToolFunctions::getAngleBetween(map->getVillage(), hero);
            int length = compassArea.width()/2;
            int dx = static_cast<int>(center.x() + length * std::cos(qDegreesToRadians(static_cast<double>(angle))));
            int dy = static_cast<int>(center.y() + length * std::sin(qDegreesToRadians(static_cast<double>(angle))));
            painter.setPen(QPen(QColor("#b97474"), 2));
            painter.drawLine(center, QPoint(dx, dy));
        }
        else
        {
            painter.setPen(QPen(QColor("#81e8ae"), 1));
            painter.drawArc(compassArea, 0, 360*16);
        }
    }
    else
        painter.drawPixmap(area, mTool->getImage());
}
