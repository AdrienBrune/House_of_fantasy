#include "save.h"

void Save::LoadFromFile(QString path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUG_ERR(QString("Impossible d'ouvrir le fichier en lecture :%1").arg(path));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
    {
        DEBUG_ERR(QString("JSON format error :%1").arg(error.errorString()));
        return;
    }

    if (doc.isObject())
    {
        QJsonObject root = doc.object();
        if (root.contains("id") && root["id"].isString())
        {
            mId = root["id"].toString();
        }
        if (root.contains("content") && root["content"].isObject())
        {
            mJsonContent = root["content"].toObject();
        }
    }
}

void Save::SaveToFile(QString path)
{
    QJsonObject root;
    root.insert("id", mId);
    root.insert("content", mJsonContent);

    QJsonDocument doc(root);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(doc.toJson());
        file.close();
    }
    else
    {
        DEBUG_ERR(QString("Erreur : Impossible d'ouvrir le fichier pour sauvegarde :%1").arg(path));
    }
}

void Save::UpdateGameContent(GameContentStruct game)
{
    Hero* hero = game.hero;
    Map* map = game.map;
    if (hero)
    {
        QJsonObject jsonHero;
        hero->toJson(jsonHero);
        mJsonContent["hero"] = jsonHero;
    }
    if (map)
    {
        QJsonObject jsonMap;
        map->toJson(jsonMap);
        mJsonContent["map"] = jsonMap;
    }
}

Hero* Save::HeroFactory()
{
    if (mJsonContent.contains("hero") && mJsonContent["hero"].isObject())
    {
        QJsonObject jsonHero = mJsonContent["hero"].toObject();
        if (jsonHero.contains("class") && jsonHero["class"].isDouble())
        {
            int heroclass = jsonHero["class"].toInt();
            Hero* hero = Hero::factory(static_cast<Hero::HeroClasses>(heroclass));
            hero->fromJson(jsonHero);
            return hero;
        }
    }
    return nullptr;
}

Map* Save::MapFactory(QWidget* parent, QGraphicsView* view)
{
    if (mJsonContent.contains("map") && mJsonContent["map"].isObject())
    {
        QJsonObject jsonMap = mJsonContent["map"].toObject();
        Map* map = new Map(parent, view);
        map->fromJson(jsonMap);
        return map;
    }
    return nullptr;
}