#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include "constants.h"
#include "monster.h"
#include "equipment.h"

class SoundManager : public QWidget
{
    Q_OBJECT
public:
    explicit SoundManager(QWidget *parent = nullptr);
    ~SoundManager();
public slots:
    void playSound(int);
    void heroEnterVillage();
    void heroLeftVillage();
    void startMusicEvent(int);
    void endMusicEvent(int);
private slots:
    void delayReached();
public:
    void startMusicFight(Monster*);
    void endMusicFight();
    void setHeroAttack(Weapon*);
private:
    void playMusic(QMediaPlayer*);
private:
    QMediaPlayer * music_world;
    QMediaPlaylist * playlist_world;

    QMediaPlayer * music_village;
    QMediaPlaylist * playlist_village;

    QMediaPlayer * music_fight;
    QMediaPlaylist * playlist_fight;

    QMediaPlayer * music_event;
    QMediaPlaylist * playlist_event;

    QMediaPlayer * pickObject;
    QMediaPlayer * drink;
    QMediaPlayer * closeChest;
    QMediaPlayer * openChest;
    QMediaPlayer * heroAttack;
    QMediaPlayer * rockMoved;
    QMediaPlayer * plankMoved;
    QMediaPlayer * equipmentSet;
    QMediaPlayer * stuffSold;
    QMediaPlayer * forge;
    QMediaPlayer * bushShaked_1;
    QMediaPlayer * bushShaked_2;
    QMediaPlayer * bushShaked_3;
    QMediaPlayer * bushShakedCoins;
    QMediaPlayer * bushShakedEquipment;
    QMediaPlayer * beginFight;
    QMediaPlayer * showPannel;
    QMediaPlayer * treeFall;
    QMediaPlayer * rockCrush;
    QMediaPlayer * wolfSound;
    QMediaPlayer * wolfAggroSound;
    QMediaPlayer * wolfHeavyAttack;
    QMediaPlayer * wolfLightAttack;
    QMediaPlayer * wolfRoar;
    QMediaPlayer * wolfDie;
    QMediaPlayer * bearSound;
    QMediaPlayer * bearAggroSound;
    QMediaPlayer * bearHeavyAttack;
    QMediaPlayer * bearLightAttack;
    QMediaPlayer * bearRoar;
    QMediaPlayer * bearDie;
    QMediaPlayer * goblinSound;
    QMediaPlayer * goblinAggroSound;
    QMediaPlayer * goblinHeavyAttack;
    QMediaPlayer * goblinLightAttack;
    QMediaPlayer * goblinRoar;
    QMediaPlayer * goblinDie;
    QMediaPlayer * trollSound;
    QMediaPlayer * trollAggroSound;
    QMediaPlayer * trollHeavyAttack;
    QMediaPlayer * trollLightAttack;
    QMediaPlayer * trollRoar;
    QMediaPlayer * trollDie;
    QMediaPlayer * oggreSound;
    QMediaPlayer * oggreAggroSound;
    QMediaPlayer * oggreHeavyAttack;
    QMediaPlayer * oggreLightAttack;
    QMediaPlayer * oggreRoar;
    QMediaPlayer * oggreDie;
    QMediaPlayer * laoshanlungSound;
    QMediaPlayer * laoshanlungAggroSound;
    QMediaPlayer * laoshanlungHeavyAttack;
    QMediaPlayer * laoshanlungLightAttack;
    QMediaPlayer * laoshanlungRoar;
    QMediaPlayer * laoshanlungDie;
    QMediaPlayer * spell;
    QMediaPlayer * teleport;

    QTimer * t_delayVillage;
    QTimer * t_delayEvent;
    bool mHeroInVillage;
    QMediaPlayer * mCurrentMusicPlaying;

};

#endif // SOUNDMANAGER_H
