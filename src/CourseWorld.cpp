// main stuff w/ the ui

#include <Geode/binding/LeaderboardsLayer.hpp>
#include <Geode/binding/LevelSelectLayer.hpp>
#include <Geode/binding/LevelSearchLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "CourseWorld.h"
#include "EndlessMode.h"
// icon button stuff
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/SimplePlayer.hpp>
#include <Geode/binding/GJGarageLayer.hpp>
#include <Geode/binding/LevelSearchLayer.hpp>

void courseworldLayer::onBack(CCObject*) {
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, MenuLayer::scene(false)));
}

void courseworldLayer::onCourses(CCObject*) {
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelSearchLayer::scene(0)));
}

void courseworldLayer::onEndless(CCObject*) {
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, endlessmodeLayer::scene()));
}
void courseworldLayer::onLeaderboards(CCObject*) {
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LeaderboardsLayer::scene(LeaderboardType::Top100, LeaderboardStat::Stars)));
}
void courseworldLayer::onMainLevels(CCObject*)
{
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, LevelSelectLayer::scene(0)));
}

courseworldLayer* courseworldLayer::create() {
    auto ret = new courseworldLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}
CCScene* courseworldLayer::scene() {
    auto scene = CCScene::create();
    auto layer = courseworldLayer::create();

    if (layer) {
        scene->addChild(layer);
    }

    return scene;
}
void courseworldLayer::keyBackClicked() {
    CCDirector::sharedDirector()->pushScene(
        CCTransitionFade::create(0.5f, MenuLayer::scene(false))
    );
}
bool courseworldLayer::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    this->setKeypadEnabled(true);
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    auto backMenu = CCMenu::create();
    backMenu->setPosition({30,290});
    backMenu->setID("back-menu");
    this->addChild(backMenu);

    auto mainButtonsMenu = CCMenu::create();
    mainButtonsMenu->setPosition({0, 0});
    mainButtonsMenu->setID("main-buttons-menu");
    this->addChild(mainButtonsMenu);

    auto backspr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    backspr->setScale(0.7f);

    auto backbtn = CCMenuItemSpriteExtra::create(
        backspr,
        this,
        menu_selector(courseworldLayer::onBack)
    );
    backMenu->addChild(backbtn);

    auto coursesSpr = CCSprite::create("CoursesButton.png"_spr);
    coursesSpr->setScale(0.15f);
    

    auto coursesBtn = CCMenuItemSpriteExtra::create(
        coursesSpr,
        this,
        menu_selector(courseworldLayer::onCourses)
    );

    auto leaderboardsSpr = CCSprite::create("LeaderboardsButton.png"_spr);
    leaderboardsSpr->setScale(0.15f);
    
    auto leaderboardsBtn = CCMenuItemSpriteExtra::create(
        leaderboardsSpr,
        this,
        menu_selector(courseworldLayer::onLeaderboards)
    );

    auto endlessSpr = CCSprite::create("EndlessButton.png"_spr);
    endlessSpr->setScale(0.15f);

    auto endlessBtn = CCMenuItemSpriteExtra::create(
        endlessSpr,
        this,
        menu_selector(courseworldLayer::onEndless)
    );

    auto mainLevelsSpr = CCSprite::create("MainLevelsButton.png"_spr);
    mainLevelsSpr->setScale(0.15f);
    auto mainLevelsBtn = CCMenuItemSpriteExtra::create
    (
        mainLevelsSpr, 
        this, 
        menu_selector(courseworldLayer::onMainLevels)
    );

    auto gm = GameManager::sharedState();

    auto holder = CCNode::create();
    holder->setContentSize({90.f, 90.f});

    // text
    auto iconLabel = CCLabelBMFont::create("Icon Kit", "goldFont.fnt");
    iconLabel->setScale(0.45f);
    iconLabel->setPosition({45.f, 72.f});
    holder->addChild(iconLabel);

    // player icon
    auto player = SimplePlayer::create(gm->getPlayerFrame());
    player->updatePlayerFrame(gm->getPlayerFrame(), IconType::Cube);
    player->setColor(gm->colorForIdx(gm->getPlayerColor()));
    player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
    player->updateColors();
    player->setScale(1.1f);
    player->setPosition({45.f, 30.f});
    holder->addChild(player);

    // clickable
    auto iconBtn = CCMenuItemSpriteExtra::create(
        holder,
        this,
        menu_selector(courseworldLayer::onIconKit)
    );

    iconBtn->setPosition({screenSize.width / 2, 45.f});
    mainButtonsMenu->addChild(iconBtn);

    auto label = CCLabelBMFont::create("Course World", "goldFont.fnt");
    label->setAnchorPoint({0.5f, 0.5f});
    label->setPosition({screenSize.width / 2, screenSize.height / 2});
    this->addChild(label);


    mainButtonsMenu->addChild(coursesBtn);
    mainButtonsMenu->addChild(leaderboardsBtn);
    mainButtonsMenu->addChild(endlessBtn);
    mainButtonsMenu->addChild(mainLevelsBtn);
    coursesBtn->setPosition({screenSize.width / 4, screenSize.height * 3 / 4});
    leaderboardsBtn->setPosition({screenSize.width * 3 / 4, screenSize.height * 3 / 4});
    endlessBtn->setPosition({screenSize.width * 3 / 4, screenSize.height / 4});
    mainLevelsBtn->setPosition({screenSize.width / 4, screenSize.height / 4});

    auto gradientBG = CCLayerGradient::create({50, 46, 140,255}, {3, 84, 111,255});
    this->addChild(gradientBG);
    gradientBG->setZOrder(-1);
    return true;

}

void courseworldLayer::onIconKit(CCObject*) {
    CCDirector::sharedDirector()->pushScene(
        CCTransitionFade::create(0.5f, GJGarageLayer::scene())
    );
}