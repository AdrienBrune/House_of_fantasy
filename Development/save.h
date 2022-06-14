 #ifndef SAVE_H
 #define SAVE_H

 #include "hero.h"
 #include "village.h"

 class Save
 {
 public:
     Save();
     ~Save();

 public:
     Hero * getHero();
     HeroChest * getHeroChest();
     const QString & getName()const;

     void setHero(Hero*);
     void setChest(HeroChest*);
     void setName(QString);
 public:
     void serialize(QDataStream& stream)const
     {
         qDebug() << "SERIALIZATION[in]  : Start";

         stream << mName;
         mChest->serialize(stream);
         mHero->serialize(stream);

         qDebug() << "SERIALIZED[in]  : Save";
     }
     void deserialize(QDataStream& stream)
     {
         qDebug() << "SERIALIZATION[out] : Start";

         stream >> mName;
         mChest = new HeroChest();
         mChest->deserialize(stream);

         quint8 heroClass = 0;
         stream >> heroClass;
         mHero = Hero::getInstance(static_cast<Hero::HeroClasses>(heroClass));
         mHero->deserialize(stream);

         qDebug() << "SERIALIZED[out] : Save";
     }
     friend QDataStream& operator<<(QDataStream& stream, const Save& object)
     {
         object.serialize(stream);
         return stream;
     }
     friend QDataStream& operator>>(QDataStream& stream, Save& object)
     {
         object.deserialize(stream);
         return stream;
     }
 private:
     Hero * mHero;
     HeroChest * mChest;
     QString mName;
 };

 #endif // SAVE_H
