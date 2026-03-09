#ifndef SAVE_H
#define SAVE_H

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>

#include "hero.h"
#include "map.h"

struct GameContentStruct
{
    Hero* hero;
    Map* map;
};

class Save
{
public:
    Save():
        mId("")
    {}
    ~Save(){}

public:
    inline const QString& GetId() const { return mId; }
    inline void SetId(QString id) { mId = id; }

    void LoadFromFile(QString path);
    void SaveToFile(QString path);

    void UpdateGameContent(GameContentStruct game);

    Hero* HeroFactory();
    Map* MapFactory(QWidget * parent, QGraphicsView * view);

private:
    QString mId;
    QJsonObject mJsonContent;
};

#endif // SAVE_H
