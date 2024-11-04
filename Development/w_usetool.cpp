#include "w_usetool.h"
#include <QtGlobal>
#include <QVBoxLayout>

#include "entitieshandler.h"

#define TOOL_AREA() QRect((width()-w)/2,(height()-h)/2,w,h)

QString checkboxStyle = R"(
    QCheckBox {
        background-color: transparent; /* Pas de fond autour du widget général */
        padding: 5px; /* Un peu de padding autour du texte */
        font-family: Mongolian Baiti; /* Choisissez la police souhaitée */
        font-size: 12px; /* Taille de la police */
        color: #5A5A5A; /* Couleur du texte en gris foncé */
    }

    QCheckBox::indicator {
        width: 15px;
        height: 15px;
        border-radius: 7px; /* Coins arrondis pour la case */
    }

    QCheckBox::indicator:unchecked {
        border: 2px solid #5A5A5A; /* Bordure de la case non cochée */
        background-color: rgba(255, 255, 255, 0.2); /* Fond transparent à 20% */
    }

    QCheckBox::indicator:checked {
        border: 2px solid #5A5A5A; /* Bordure de la case cochée */
        background-color: #3A3A3A; /* Fond gris foncé pour la case cochée */
    }

    QCheckBox::indicator:unchecked:hover {
        border: 2px solid #888888; /* Bordure plus claire au survol */
    }

    QCheckBox::indicator:checked:hover {
        background-color: #4A4A4A; /* Fond légèrement plus clair au survol lorsqu'elle est cochée */
    }
)";

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
}

void W_UseTool::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = static_cast<int>(width()-mExitButton->height());
    int h = static_cast<int>((height()-mExitButton->height()));
    QRect area = TOOL_AREA();

    painter.drawPixmap(area, mTool->getImage());
}


W_UseToolCompass::W_UseToolCompass(QWidget *parent, Tool * tool):
    W_UseTool(parent, tool)
{
    connect(&mTimer, &QTimer::timeout, this, &W_UseTool::refresh);
    mTimer.start(500);
}

void W_UseToolCompass::paintEvent(QPaintEvent * event)
{
    W_UseTool::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    Hero *hero = EntitiesHandler::getInstance().getHero();
    Map *map = EntitiesHandler::getInstance().getMap();

    int w = static_cast<int>(width()-mExitButton->height());
    int h = static_cast<int>((height()-mExitButton->height()));
    QRect area = TOOL_AREA();

    QRect compassArea = QRect(area.x() + area.width()/10, area.y() + area.height()/10, area.width()*8/10, area.height()*8/10);
    painter.drawPixmap(area, QPixmap(":/graphicItems/compassDisplayer.png"));
    if(!hero->isInVillage())
    {
        QPoint center(area.x() + area.width()/2, area.y() + area.height()/2);
        int angle = ToolFunctions::getAngleBetween(map->getVillage(), hero);
        int length = compassArea.width()/2;
        int dx = static_cast<int>(center.x() + length * std::cos(qDegreesToRadians(static_cast<double>(angle))));
        int dy = static_cast<int>(center.y() + length * std::sin(qDegreesToRadians(static_cast<double>(angle))));
        painter.setPen(QPen(QColor(185,116,116), 2));
        painter.drawLine(center, QPoint(dx, dy));
    }
    else
    {
        painter.setPen(QPen(QColor(129,232,174), 1));
        painter.drawArc(compassArea, 0, 360*16);
    }
}

W_UseToolMap::W_UseToolMap(QWidget *parent, Tool * tool):
    W_UseTool(parent, tool)
{
    mExitButton->setText("X");

    MapScroll * mapScroll = dynamic_cast<MapScroll*>(mTool);
    assert(mapScroll);
    if(mapScroll)
        connect(mapScroll, &MapScroll::sig_update, this, &W_UseTool::refresh);

    mFilterList["monster"] = new QCheckBox("monstre", this);
    mFilterList["monster"]->setCheckState(mapScroll->getFilter("monster") ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    mFilterList["ore"] = new QCheckBox("minerai", this);
    mFilterList["ore"]->setCheckState(mapScroll->getFilter("ore") ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    mFilterList["chest"] = new QCheckBox("coffre", this);
    mFilterList["chest"]->setCheckState(mapScroll->getFilter("chest") ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    mFilterList["lake"] = new QCheckBox("poisson", this);
    mFilterList["lake"]->setCheckState(mapScroll->getFilter("lake") ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    for(auto checkbox : qAsConst(mFilterList))
    {
        checkbox->setStyleSheet(checkboxStyle);
        connect(checkbox, &QCheckBox::stateChanged, this, &W_UseToolMap::onFilterChanged);
        layout->addWidget(checkbox);
        checkbox->setVisible(false);
    }
    setLayout(layout);
}

void W_UseToolMap::displayTool()
{
    W_UseTool::show();

    MapScroll * scroll = dynamic_cast<MapScroll*>(mTool);
    if(!scroll)
        return;
    scroll->discoveryMode(true);
}

void W_UseToolMap::hideTool()
{
    MapScroll * scroll = dynamic_cast<MapScroll*>(mTool);
    if(scroll)
        scroll->discoveryMode(false);

    W_UseTool::hideTool();
}

void W_UseToolMap::paintEvent(QPaintEvent * event)
{
    W_UseTool::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    Hero *hero = EntitiesHandler::getInstance().getHero();
    Map *map = EntitiesHandler::getInstance().getMap();

    int w = static_cast<int>(width()-mExitButton->height());
    int h = static_cast<int>((height()-mExitButton->height()));
    QRect area = TOOL_AREA();

    MapScroll * scroll = dynamic_cast<MapScroll*>(mTool);
    if(!scroll)
        return;

    painter.drawPixmap(area, QPixmap(":/graphicItems/mapBackground.png"));

    QSizeF worldSize = map->getScene()->sceneRect().size();
    int xpos, ypos;

    // Draw village
    xpos = (map->getVillage()->pos().x() + map->getVillage()->boundingRect().width()/2) * area.width() / worldSize.width();
    ypos = (map->getVillage()->pos().y() + map->getVillage()->boundingRect().height()/2) * area.height() / worldSize.height();
    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(":/icons/icon_village.png"));

    // Draw Goblin Village
    xpos = (map->getGoblinVillage()->pos().x() + map->getGoblinVillage()->boundingRect().width()/2) * area.width() / worldSize.width();
    ypos = (map->getGoblinVillage()->pos().y() + map->getGoblinVillage()->boundingRect().height()/2) * area.height() / worldSize.height();
    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(":/icons/icon_goblin_village.png"));

    if(mFilterList["monster"])
    {
        if(mFilterList["monster"]->checkState() == Qt::CheckState::Checked)
        {
            // Draw monsters
            for(Monster * monster : scroll->getDiscoveryData().monsters)
            {   
                if(monster)
                {
                    xpos = monster->pos().x() * area.width() / worldSize.width();
                    ypos = monster->pos().y() * area.height() / worldSize.height();
                    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(QString(":/icons/icon_%1.png").arg(monster->getName())));
                }
            }
        }
    }

    if(mFilterList["ore"])
    {
        if(mFilterList["ore"]->checkState() == Qt::CheckState::Checked)
        {
            // Draw ore spots
            for(MapEvent * mapEvent : scroll->getDiscoveryData().mapEvent)
            {
                OreSpot * oreSpot = dynamic_cast<OreSpot*>(mapEvent);
                if(oreSpot)
                {
                    xpos = oreSpot->pos().x() * area.width() / worldSize.width();
                    ypos = oreSpot->pos().y() * area.height() / worldSize.height();
                    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(QString(":/icons/icon_%1.png").arg(oreSpot->invocName())));
                }
            }
        }
    }

    if(mFilterList["lake"])
    {
        if(mFilterList["lake"]->checkState() == Qt::CheckState::Checked)
        {
            // Draw lakes
            for(MapEvent * mapEvent : scroll->getDiscoveryData().mapEvent)
            {
                Lake * lake = dynamic_cast<Lake*>(mapEvent);
                if(lake)
                {
                    xpos = lake->pos().x() * area.width() / worldSize.width();
                    ypos = lake->pos().y() * area.height() / worldSize.height();
                    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(QString(":/icons/icon_fish.png")));
                }
            }
        }
    }

    if(mFilterList["chest"])
    {
        if(mFilterList["chest"]->checkState() == Qt::CheckState::Checked)
        {
            // Draw chests
            for(MapEvent * mapEvent : scroll->getDiscoveryData().mapEvent)
            {
                ChestEvent * chest = dynamic_cast<ChestEvent*>(mapEvent);
                if(chest)
                {
                    xpos = chest->pos().x() * area.width() / worldSize.width();
                    ypos = chest->pos().y() * area.height() / worldSize.height();
                    painter.drawPixmap(QRect(xpos, ypos, 25, 25), QPixmap(QString(":/icons/icon_chest.png")));
                }
            }
        }
    }

    // Draw fog
    QPixmap fogTexture(":/graphicItems/fog_brush.png");
    QBrush fogBrush(fogTexture);
    fogBrush.setStyle(Qt::TexturePattern);
    painter.setOpacity(0.9);

    const QVector<QVector<bool>> & fogMatrix = scroll->getDiscoveryData().fogMatrix;
    int xSplit = fogMatrix[0].size();
    int ySplit = fogMatrix.size();
    qreal cellWidth = static_cast<qreal>(area.width()) / static_cast<qreal>(xSplit);
    qreal cellHeight = static_cast<qreal>(area.height()) / static_cast<qreal>(ySplit);

    for (int row = 0; row < ySplit; ++row)
    {
        for (int col = 0; col < xSplit; ++col)
        {
            qreal x = area.x() + col * cellWidth;
            qreal y = area.y() + row * cellHeight;

            int roundedX = static_cast<int>(x);
            if(x - roundedX >= 0.5)
                roundedX += 1;

            int roundedY = static_cast<int>(y);
            if(y - roundedY >= 0.5)
                roundedY += 1;

            qreal nextX = x + cellWidth;
            qreal nextY = y + cellHeight;

            int roundedWidth = static_cast<int>(nextX) - roundedX;
            if(nextX - static_cast<int>(nextX) >= 0.5)
                roundedWidth += 1;

            int roundedHeight = static_cast<int>(nextY) - roundedY;
            if(nextY - static_cast<int>(nextY) >= 0.5)
                roundedHeight += 1;

            QRect cellRect(roundedX, roundedY, roundedWidth, roundedHeight);
            if(!fogMatrix[row][col])
            {
                painter.setBrush(fogBrush);
                painter.setPen(Qt::NoPen);
                painter.drawRect(cellRect);
            }
        }
    }

    // Draw hero
    int crossSize = 10;
    painter.setOpacity(1);
    xpos = (hero->pos().x() + hero->boundingRect().width()/2) * area.width() / worldSize.width() ;
    ypos = (hero->pos().y() + hero->boundingRect().height()/2) * area.height() / worldSize.height();
    painter.setPen(QPen(QColor(35,35,35), 2));
    painter.drawLine(xpos + crossSize/2, ypos + crossSize/2, xpos + crossSize + crossSize/2, ypos + crossSize + crossSize/2);
    painter.drawLine(xpos + crossSize + crossSize/2, ypos + crossSize/2, xpos + crossSize/2, ypos + crossSize + crossSize/2);

    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(QColor(0,0,0,0)));
    painter.drawRect(area);

    // Draw "Display Filters"
    if(!underMouse())
    {
        QFont font("Mongolian Baiti", 9);
        painter.setFont(font);
        painter.setPen(QPen(QColor(90,90,90), 1));

        int textX = area.left() + 30;
        int textY = area.bottom() - painter.fontMetrics().height();

        painter.drawText(textX, textY, "afficher les filtres");

        int arrowSize = 10;
        int arrowX = textX - arrowSize - 10;
        int arrowY = textY;

        QPolygon arrow;
        arrow << QPoint(arrowX, arrowY)
              << QPoint(arrowX + arrowSize, arrowY)
              << QPoint(arrowX + arrowSize / 2, arrowY - arrowSize / 2);
        painter.setBrush(QBrush(Qt::darkGray));
        painter.drawPolygon(arrow);
    }
}
