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
     Village * getVillage();

     void setHero(Hero*);
     void setChest(HeroChest*);
     void setName(QString);
     void setVillage(Village*);
 public:
     void serialize(QDataStream& stream)const
     {
         DEBUG("SERIALIZATION[in]  : Start");

         stream << mName;
         mChest->serialize(stream);
         mHero->serialize(stream);
         mVillage->serialize(stream);

         DEBUG("SERIALIZED[in]  : Save");
     }
     void deserialize(QDataStream& stream)
     {
         DEBUG("SERIALIZATION[out] : Start");

         stream >> mName;
         if(mChest)
             delete mChest;
         mChest = new HeroChest();
         mChest->deserialize(stream);

         quint8 heroClass = 0;
         stream >> heroClass;
         mHero = Hero::getInstance(static_cast<Hero::HeroClasses>(heroClass));
         mHero->deserialize(stream);

         if(mVillage)
             delete mVillage;
         mVillage = new Village();
         mVillage->deserialize(stream);

         DEBUG("SERIALIZED[out] : Save");
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
     Village * mVillage;
 };

 #endif // SAVE_H
