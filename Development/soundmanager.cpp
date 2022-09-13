#include "soundmanager.h"

#include <QElapsedTimer>
SoundManager::SoundManager(QWidget * parent):
    QWidget(parent),
    mHeroInVillage(false)
{
    mCurrentMusicPlaying = nullptr;

    t_delayVillage = new QTimer(this);
    t_delayVillage->setSingleShot(true);
    connect(t_delayVillage, SIGNAL(timeout()), this, SLOT(delayReached()));

    t_delayEvent = new QTimer(this);
    t_delayEvent->setSingleShot(true);

    music_world = new QMediaPlayer(this);
    music_world->setVolume(2);
    playlist_world = new QMediaPlaylist(this);
    playlist_world->addMedia(QUrl("qrc:/audio/musics/music_world.wav"));
    playlist_world->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    music_world->setPlaylist(playlist_world);

    music_village = new QMediaPlayer(this);
    music_village->setVolume(3);
    playlist_village = new QMediaPlaylist(this);
    playlist_village->addMedia(QUrl("qrc:/audio/musics/music_village_1.wav"));
    playlist_village->addMedia(QUrl("qrc:/audio/musics/music_village_2.wav"));
    playlist_village->addMedia(QUrl("qrc:/audio/musics/music_village_3.wav"));
    playlist_village->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    music_village->setPlaylist(playlist_village);

    music_fight = new QMediaPlayer(this);
    music_fight->setVolume(2);
    playlist_fight = new QMediaPlaylist(this);
    playlist_fight->addMedia(QUrl("qrc:/audio/musics/music_fight_1.wav"));
    playlist_fight->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);
    music_fight->setPlaylist(playlist_fight);

    music_event = new QMediaPlayer(this);
    music_event->setVolume(4);
    playlist_event = new QMediaPlaylist(this);
    playlist_event->addMedia(QUrl("qrc:/audio/musics/music_event_0.wav"));
    playlist_event->addMedia(QUrl("qrc:/audio/musics/music_event_1.wav"));
    playlist_event->addMedia(QUrl("qrc:/audio/musics/music_event_2.wav"));
    playlist_event->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
    music_event->setPlaylist(playlist_event);

    pickObject = new QMediaPlayer(this);
    pickObject->setMedia(QUrl("qrc:/audio/sounds/sound_pick.wav"));
    pickObject->setVolume(5);

    drink = new QMediaPlayer(this);
    drink->setMedia(QUrl("qrc:/audio/sounds/sound_drink.wav"));
    drink->setVolume(5);

    openChest = new QMediaPlayer(this);
    openChest->setMedia(QUrl("qrc:/audio/sounds/sound_openchest.wav"));
    openChest->setVolume(10);

    closeChest = new QMediaPlayer(this);
    closeChest->setMedia(QUrl("qrc:/audio/sounds/sound_closechest.wav"));
    closeChest->setVolume(10);

    heroAttack = new QMediaPlayer(this);
    heroAttack->setMedia(QUrl("qrc:/audio/sounds/sound_hero_attack_generic.wav"));
    heroAttack->setVolume(20);

    rockMoved = new QMediaPlayer(this);
    rockMoved->setMedia(QUrl("qrc:/audio/sounds/sound_move_rock.wav"));
    rockMoved->setVolume(40);

    plankMoved = new QMediaPlayer(this);
    plankMoved->setMedia(QUrl("qrc:/audio/sounds/sound_move_plank.wav"));
    plankMoved->setVolume(40);

    bushShaked_1 = new QMediaPlayer(this);
    bushShaked_1->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_1.wav"));
    bushShaked_1->setVolume(5);

    bushShaked_1 = new QMediaPlayer(this);
    bushShaked_1->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_1.wav"));
    bushShaked_1->setVolume(5);

    bushShaked_2 = new QMediaPlayer(this);
    bushShaked_2->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_2.wav"));
    bushShaked_2->setVolume(5);

    bushShaked_3 = new QMediaPlayer(this);
    bushShaked_3->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_3.wav"));
    bushShaked_3->setVolume(5);

    bushShakedCoins = new QMediaPlayer(this);
    bushShakedCoins->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_coins.wav"));
    bushShakedCoins->setVolume(5);

    bushShakedEquipment = new QMediaPlayer(this);
    bushShakedEquipment->setMedia(QUrl("qrc:/audio/sounds/sound_bush_shaked_equipment.wav"));
    bushShakedEquipment->setVolume(5);

    beginFight = new QMediaPlayer(this);
    beginFight->setMedia(QUrl("qrc:/audio/sounds/sound_start_fight.wav"));
    beginFight->setVolume(50);

    showPannel = new QMediaPlayer(this);
    showPannel->setMedia(QUrl("qrc:/audio/sounds/sound_show_pannel.wav"));
    showPannel->setVolume(10);

    treeFall = new QMediaPlayer(this);
    treeFall->setMedia(QUrl("qrc:/audio/sounds/sound_tree_fall.wav"));
    treeFall->setVolume(10);

    rockCrush = new QMediaPlayer(this);
    rockCrush->setMedia(QUrl("qrc:/audio/sounds/sound_rock_crush.wav"));
    rockCrush->setVolume(10);

    wolfSound = new QMediaPlayer(this);
    wolfSound->setMedia(QUrl("qrc:/audio/sounds/sound_wolf.wav"));
    wolfSound->setVolume(5);

    wolfAggroSound = new QMediaPlayer(this);
    wolfAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_wolf_aggro.wav"));
    wolfAggroSound->setVolume(5);

    wolfHeavyAttack = new QMediaPlayer(this);
    wolfHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_wolf_heavy_attack.wav"));
    wolfHeavyAttack->setVolume(5);

    wolfLightAttack = new QMediaPlayer(this);
    wolfLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_wolf_light_attack.wav"));
    wolfLightAttack->setVolume(15);

    wolfRoar = new QMediaPlayer(this);
    wolfRoar->setMedia(QUrl("qrc:/audio/sounds/sound_wolf_aggro.wav"));
    wolfRoar->setVolume(5);

    wolfDie = new QMediaPlayer(this);
    wolfDie->setMedia(QUrl("qrc:/audio/sounds/sound_wolf_die.wav"));
    wolfDie->setVolume(5);

    bearSound = new QMediaPlayer(this);
    bearSound->setMedia(QUrl("qrc:/audio/sounds/sound_bear.wav"));
    bearSound->setVolume(5);

    bearAggroSound = new QMediaPlayer(this);
    bearAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_bear_aggro.wav"));
    bearAggroSound->setVolume(5);

    bearHeavyAttack = new QMediaPlayer(this);
    bearHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_bear_heavy_attack.wav"));
    bearHeavyAttack->setVolume(10);

    bearLightAttack = new QMediaPlayer(this);
    bearLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_bear_light_attack.wav"));
    bearLightAttack->setVolume(15);

    bearRoar = new QMediaPlayer(this);
    bearRoar->setMedia(QUrl("qrc:/audio/sounds/sound_bear.wav"));
    bearRoar->setVolume(10);

    bearDie = new QMediaPlayer(this);
    bearDie->setMedia(QUrl("qrc:/audio/sounds/sound_bear_die.wav"));
    bearDie->setVolume(5);

    goblinSound = new QMediaPlayer(this);
    goblinSound->setMedia(QUrl("qrc:/audio/sounds/sound_goblin.wav"));
    goblinSound->setVolume(5);

    goblinAggroSound = new QMediaPlayer(this);
    goblinAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_goblin_aggro.wav"));
    goblinAggroSound->setVolume(5);

    goblinHeavyAttack = new QMediaPlayer(this);
    goblinHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_goblin_heavy_attack.wav"));
    goblinHeavyAttack->setVolume(10);

    goblinLightAttack = new QMediaPlayer(this);
    goblinLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_goblin_light_attack.wav"));
    goblinLightAttack->setVolume(50);

    goblinRoar = new QMediaPlayer(this);
    goblinRoar->setMedia(QUrl("qrc:/audio/sounds/sound_goblin_aggro.wav"));
    goblinRoar->setVolume(5);

    goblinDie = new QMediaPlayer(this);
    goblinDie->setMedia(QUrl("qrc:/audio/sounds/sound_goblin_die.wav"));
    goblinDie->setVolume(5);

    trollSound = new QMediaPlayer(this);
    trollSound->setMedia(QUrl("qrc:/audio/sounds/sound_troll.wav"));
    trollSound->setVolume(5);

    trollAggroSound = new QMediaPlayer(this);
    trollAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_troll_aggro.wav"));
    trollAggroSound->setVolume(5);

    trollHeavyAttack = new QMediaPlayer(this);
    trollHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_troll_heavy_attack.wav"));
    trollHeavyAttack->setVolume(10);

    trollLightAttack = new QMediaPlayer(this);
    trollLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_troll_light_attack.wav"));
    trollLightAttack->setVolume(10);

    trollRoar = new QMediaPlayer(this);
    trollRoar->setMedia(QUrl("qrc:/audio/sounds/sound_troll_aggro.wav"));
    trollRoar->setVolume(5);

    trollDie = new QMediaPlayer(this);
    trollDie->setMedia(QUrl("qrc:/audio/sounds/sound_trolln_die.wav"));
    trollDie->setVolume(5);

    oggreSound = new QMediaPlayer(this);
    oggreSound->setMedia(QUrl("qrc:/audio/sounds/sound_oggre.wav"));
    oggreSound->setVolume(5);

    oggreAggroSound = new QMediaPlayer(this);
    oggreAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_oggre_aggro.wav"));
    oggreAggroSound->setVolume(5);

    oggreHeavyAttack = new QMediaPlayer(this);
    oggreHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_oggre_heavy_attack.wav"));
    oggreHeavyAttack->setVolume(15);

    oggreLightAttack = new QMediaPlayer(this);
    oggreLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_oggre_light_attack.wav"));
    oggreLightAttack->setVolume(5);

    oggreRoar = new QMediaPlayer(this);
    oggreRoar->setMedia(QUrl("qrc:/audio/sounds/sound_oggre_aggro.wav"));
    oggreRoar->setVolume(5);

    oggreDie = new QMediaPlayer(this);
    oggreDie->setMedia(QUrl("qrc:/audio/sounds/sound_oggre_die.wav"));
    oggreDie->setVolume(5);

    equipmentSet = new QMediaPlayer(this);
    equipmentSet->setMedia(QUrl("qrc:/audio/sounds/sound_equip.wav"));
    equipmentSet->setVolume(5);

    stuffSold = new QMediaPlayer(this);
    stuffSold->setMedia(QUrl("qrc:/audio/sounds/sound_buy.wav"));
    stuffSold->setVolume(10);

    forge = new QMediaPlayer(this);
    forge->setMedia(QUrl("qrc:/audio/sounds/sound_forge.wav"));
    forge->setVolume(10);

    laoshanlungSound = new QMediaPlayer(this);
    laoshanlungSound->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung.wav"));
    laoshanlungSound->setVolume(5);

    laoshanlungAggroSound = new QMediaPlayer(this);
    laoshanlungAggroSound->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung_aggro.wav"));
    laoshanlungAggroSound->setVolume(5);

    laoshanlungHeavyAttack = new QMediaPlayer(this);
    laoshanlungHeavyAttack->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung_heavy_attack.wav"));
    laoshanlungHeavyAttack->setVolume(10);

    laoshanlungLightAttack = new QMediaPlayer(this);
    laoshanlungLightAttack->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung_light_attack.wav"));
    laoshanlungLightAttack->setVolume(10);

    laoshanlungRoar = new QMediaPlayer(this);
    laoshanlungRoar->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung.wav"));
    laoshanlungRoar->setVolume(5);

    laoshanlungDie = new QMediaPlayer(this);
    laoshanlungDie->setMedia(QUrl("qrc:/audio/sounds/sound_laoshanlung_die.wav"));
    laoshanlungDie->setVolume(5);

    spell = new QMediaPlayer(this);
    spell->setVolume(15);

    teleport = new QMediaPlayer(this);
    teleport->setMedia(QUrl("qrc:/audio/sounds/sound_teleport.wav"));
    teleport->setVolume(25);

    playMusic(music_world);
}

void SoundManager::playSound(int sound)
{
    switch(sound)
    {
    case SOUND_HERO_ATTACK :
        heroAttack->play();
        break;
    case SOUND_MOVE_ROCK :
        if(rockMoved->state() == QMediaPlayer::State::PlayingState){
            rockMoved->setPosition(0);
        }else{
            rockMoved->play();
        }
        break;
    case SOUND_MOVE_PLANK :
        if(plankMoved->state() == QMediaPlayer::State::PlayingState){
            rockMoved->setPosition(0);
        }else{
            plankMoved->play();
        }
        break;
    case SOUND_BUSH_SHAKED :
            switch(QRandomGenerator::global()->bounded(3))
            {
            case 0 :
                if(bushShaked_1->state() == QMediaPlayer::PlayingState){
                    bushShaked_1->setPosition(0);
                }else{
                    bushShaked_1->play();
                }
                break;
            case 1 :
                if(bushShaked_2->state() == QMediaPlayer::PlayingState){
                    bushShaked_2->setPosition(0);
                }else{
                    bushShaked_2->play();
                }
                break;
            case 2 :
                if(bushShaked_3->state() == QMediaPlayer::PlayingState){
                    bushShaked_3->setPosition(0);
                }else{
                    bushShaked_3->play();
                }
                break;
            }
        break;
    case SOUND_BUSH_SHAKED_EVENT_COIN :
        if(bushShakedCoins->state() == QMediaPlayer::PlayingState){
            bushShakedCoins->setPosition(0);
        }else{
            bushShakedCoins->play();
        }
        break;
    case SOUND_BUSH_SHAKED_EVENT_EQUIPMENT :
        if(bushShakedEquipment->state() == QMediaPlayer::PlayingState){
            bushShakedEquipment->setPosition(0);
        }else{
            bushShakedEquipment->play();
        }
        break;
    case SOUND_START_FIGHT :
        beginFight->play();
        break;
    case SOUND_WOLF :
        wolfSound->play();
        break;
    case SOUND_WOLF_AGGRO :
        wolfAggroSound->play();
        break;
    case SOUND_WOLF_HEAVYATTACK :
        wolfHeavyAttack->play();
        break;
    case SOUND_WOLF_LIGHTATTACK :
        wolfLightAttack->play();
        break;
    case SOUND_WOLF_ROAR :
        wolfRoar->play();
        break;
    case SOUND_WOLF_DIE :
        wolfDie->play();
        break;
    case SOUND_BEAR :
        bearSound->play();
        break;
    case SOUND_BEAR_AGGRO :
        bearAggroSound->play();
        break;
    case SOUND_BEAR_HEAVYATTACK :
        bearHeavyAttack->play();
        break;
    case SOUND_BEAR_LIGHTATTACK :
        bearLightAttack->play();
        break;
    case SOUND_BEAR_ROAR :
        bearRoar->play();
        break;
    case SOUND_BEAR_DIE :
        bearDie->play();
        break;
    case SOUND_GOBLIN :
        goblinSound->play();
        break;
    case SOUND_GOBLIN_AGGRO :
        goblinAggroSound->play();
        break;
    case SOUND_GOBLIN_HEAVYATTACK :
        goblinHeavyAttack->play();
        break;
    case SOUND_GOBLIN_LIGHTATTACK :
        goblinLightAttack->play();
        break;
    case SOUND_GOBLIN_ROAR :
        goblinRoar->play();
        break;
    case SOUND_GOBLIN_DIE :
        goblinDie->play();
        break;
    case SOUND_TROLL :
        trollSound->play();
        break;
    case SOUND_TROLL_AGGRO :
        trollAggroSound->play();
        break;
    case SOUND_TROLL_HEAVYATTACK :
        trollHeavyAttack->play();
        break;
    case SOUND_TROLL_LIGHTATTACK :
        trollLightAttack->play();
        break;
    case SOUND_TROLL_ROAR :
        trollRoar->play();
        break;
    case SOUND_TROLL_DIE :
        trollDie->play();
        break;
    case SOUND_OGGRE :
        oggreSound->play();
        break;
    case SOUND_OGGRE_AGGRO :
        oggreAggroSound->play();
        break;
    case SOUND_OGGRE_HEAVYATTACK :
        oggreHeavyAttack->play();
        break;
    case SOUND_OGGRE_LIGHTATTACK :
        oggreLightAttack->play();
        break;
    case SOUND_OGGRE_ROAR :
        oggreRoar->play();
        break;
    case SOUND_OGGRE_DIE :
        oggreDie->play();
        break;  
    case SOUND_LAOSHANLUNG :
        laoshanlungSound->play();
        break;
    case SOUND_LAOSHANLUNG_AGGRO :
        laoshanlungAggroSound->play();
        break;
    case SOUND_LAOSHANLUNG_HEAVYATTACK :
        laoshanlungHeavyAttack->play();
        break;
    case SOUND_LAOSHANLUNG_LIGHTATTACK :
        laoshanlungLightAttack->play();
        break;
    case SOUND_LAOSHANLUNG_ROAR :
        laoshanlungRoar->play();
        break;
    case SOUND_LAOSHANLUNG_DIE :
        laoshanlungDie->play();
        break;
    case SOUND_EQUIP :
        if(equipmentSet->state() == QMediaPlayer::State::PlayingState){
            equipmentSet->setPosition(0);
        }else{
            equipmentSet->play();
        }
        break;
    case SOUND_BUY :
        if(stuffSold->state() == QMediaPlayer::State::PlayingState){
            stuffSold->setPosition(0);
        }else{
            stuffSold->play();
        }
        break;
    case SOUND_FORGE :
        forge->play();
        break;
    case SOUND_OPENCHEST :
        openChest->play();
        break;
    case SOUND_CLOSECHEST :
        closeChest->play();
        break;
    case SOUND_DRINK :
        if(drink->state() == QMediaPlayer::State::PlayingState){
            drink->setPosition(0);
        }else{
            drink->play();
        }
        break;
    case SOUND_PICK_OBJECT :
        if(pickObject->state() == QMediaPlayer::State::PlayingState){
            pickObject->setPosition(0);
        }else{
            pickObject->play();
        }
        break;
    case SOUND_TREE_FALL :
        if(treeFall->state() == QMediaPlayer::State::PlayingState){
            treeFall->setPosition(0);
        }else{
            treeFall->play();
        }
        break;
    case SOUND_ROCK_CRUSH:
        if(rockCrush->state() == QMediaPlayer::State::PlayingState){
            rockCrush->setPosition(0);
        }else{
            rockCrush->play();
        }
        break;
    case SOUND_SPELL_0:
    case SOUND_SPELL_1:
    case SOUND_SPELL_2:
    case SOUND_SPELL_3:
    case SOUND_SPELL_4:
    case SOUND_SPELL_5:
    case SOUND_SPELL_6:
    case SOUND_SPELL_7:
        spell->setMedia(QUrl(QString("qrc:/audio/sounds/sound_spell_%1.wav").arg(sound-SOUND_SPELL_0)));
        spell->play();
        break;

    case SOUND_TELEPORT:
        teleport->play();
        break;
    }
}

void SoundManager::heroEnterVillage()
{
    mHeroInVillage = true;

    if(mCurrentMusicPlaying == music_event)
        return;
    if(!t_delayVillage->isActive()){
        playlist_village->next();
        playMusic(music_village);
    }
}

void SoundManager::heroLeftVillage()
{
    mHeroInVillage = false;

    if(mCurrentMusicPlaying == music_event)
        return;
    t_delayVillage->start(5000);
    playMusic(music_world);
}

void SoundManager::delayReached()
{
    if(mCurrentMusicPlaying == music_event)
        return;
    if(mHeroInVillage){
        playlist_village->next();
        playMusic(music_village);
    }
}

void SoundManager::endMusicEvent(int)
{
    if(mHeroInVillage){
        playMusic(music_village);
    }
    playMusic(music_world);
}

void SoundManager::startMusicEvent(int event)
{
    if(mCurrentMusicPlaying == music_event && event != MUSICEVENT_START_DAY)
        return;

    if(!t_delayEvent->isActive() && (event == MUSICEVENT_CLOSE_FIGHT)){
        disconnect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
        playlist_event->setCurrentIndex(event);
        playMusic(music_event);
        t_delayEvent->start(300000);
        connect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
    }else if(event == MUSICEVENT_START_DAY){
        disconnect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
        playlist_event->setCurrentIndex(event);
        playMusic(music_event);
        connect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
    }else if(!t_delayEvent->isActive() && event == MUSICEVENT_ENTER_GOBLIN_VILLAGE){
        disconnect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
        playlist_event->setCurrentIndex(event);
        playMusic(music_event);
        t_delayEvent->start(8000);
        connect(playlist_event, SIGNAL(currentIndexChanged(int)), this, SLOT(endMusicEvent(int)));
    }
}

void SoundManager::startMusicFight(Monster *)
{
    playMusic(music_fight);
}

void SoundManager::endMusicFight()
{
    if(mHeroInVillage){
        playMusic(music_village);
    }
    music_fight->setPosition(0);
    playMusic(music_world);
}

void SoundManager::setHeroAttack(Weapon * weapon)
{
    Bow * bow = dynamic_cast<Bow*>(weapon);
    if(bow)
    {
        heroAttack->setMedia(QUrl("qrc:/audio/sounds/sound_hero_attack_bow.wav"));
    }
    else
    {
        heroAttack->setMedia(QUrl("qrc:/audio/sounds/sound_hero_attack_generic.wav"));
    }
}

void SoundManager::playMusic(QMediaPlayer * media)
{
    if(mCurrentMusicPlaying != nullptr){
        mCurrentMusicPlaying->pause();
    }
    mCurrentMusicPlaying = media;
    media->play();
}

SoundManager::~SoundManager()
{

}
