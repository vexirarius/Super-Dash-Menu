#include <Geode/Geode.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <fmt/format.h>
#include <random>
#include "EndlessMode.h"
#include "CourseWorld.h"
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

static bool g_waitingForEndlessLevel = false;
static int g_requestedLevelIndex = 0;

int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

int stableRandomInt(int seed, int min, int max) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

class $modify(MyEndLevelLayer, EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        if (!Mod::get()->getSavedValue<bool>("playing-endless-level", false)) {
            return;
        }

        Mod::get()->setSavedValue<bool>("playing-endless-level", false);

        int current = Mod::get()->getSavedValue<int>("endless-current-index", 0);
        int completed = Mod::get()->getSavedValue<int>("endless-playing-index", -1);

        if (completed == current) {
            Mod::get()->setSavedValue<int>("endless-current-index", current + 1);
            log::info("Endless progress advanced to {}", current + 1);
        }
    }
};
void endlessmodeLayer::onResetSeed(CCObject*) {
    Mod::get()->setSavedValue<int>("endless-seed", randomInt(1, 999999999));

    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.3f, endlessmodeLayer::scene())
    );
}
void endlessmodeLayer::requestSpecificLevel(int buttonIndex) {

    Mod::get()->setSavedValue<bool>("playing-endless-level", true);
    Mod::get()->setSavedValue<int>("endless-playing-index", buttonIndex);
    auto const& endless = m_levels[buttonIndex];

    g_waitingForEndlessLevel = true;
    g_requestedLevelIndex = endless.levelIndex;

    Mod::get()->setSavedValue<bool>("playing-endless-level", true);
    Mod::get()->setSavedValue<int>("endless-playing-index", buttonIndex);

    auto search = GJSearchObject::create(
        SearchType::Search,
        "",
        "2,3,4,5",
        "-",
        endless.page,
        true,
        false,
        false,
        0,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        3,
        0,
        0
    );

    GameLevelManager::sharedState()->getOnlineLevels(search);
}

class $modify(MyGLM, GameLevelManager) {
    void onGetOnlineLevelsCompleted(gd::string response, gd::string tag) {
        GameLevelManager::onGetOnlineLevelsCompleted(response, tag);

        if (!g_waitingForEndlessLevel)
            return;

        g_waitingForEndlessLevel = false;

        auto levels = this->getStoredOnlineLevels(tag.c_str());
        if (!levels || levels->count() == 0) {
            log::warn("No levels parsed");
            return;
        }

        int index = g_requestedLevelIndex;
        if (index >= levels->count())
            index = 0;

        auto level = static_cast<GJGameLevel*>(levels->objectAtIndex(index));
        if (!level)
            return;

        CCDirector::sharedDirector()->replaceScene(
            CCTransitionFade::create(
                0.5f,
                LevelInfoLayer::scene(level, false)
            )
        );
    }
};

endlessmodeLayer* endlessmodeLayer::create() {
    auto ret = new endlessmodeLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    }
    else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

CCScene* endlessmodeLayer::scene() {
    auto scene = CCScene::create();
    auto layer = endlessmodeLayer::create();

    if (layer)
        scene->addChild(layer);

    return scene;
}

const char* endlessmodeLayer::getSpotTexture(int index, int currentIndex) {
    if (index < currentIndex) return "PassedSpot.png"_spr;
    if (index == currentIndex) return "ActivateSpot.png"_spr;
    return "NonActivateSpot.png"_spr;
}

CCNode* endlessmodeLayer::createLevelNode(EndlessLevel const& level, int index) {
    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });

    auto spot = CCSprite::create(getSpotTexture(index, m_currentIndex));
    if (!spot)
        spot = CCSprite::createWithSpriteFrameName("GJ_button_01.png");

    auto btn = CCMenuItemSpriteExtra::create(
        spot,
        this,
        menu_selector(endlessmodeLayer::onLevel)
    );

    btn->setTag(index);
    menu->addChild(btn);

    auto label = CCLabelBMFont::create(level.name.c_str(), "bigFont.fnt");
    label->setScale(0.4f);
    label->setPosition({ 0.f, -45.f });
    menu->addChild(label);

    return menu;
}

void endlessmodeLayer::removeOffScreenStuff(float cameraY) {
    int center = std::max(0, static_cast<int>(cameraY / m_spacing));
    int start = std::max(0, center - m_buffer);
    int end = std::min(static_cast<int>(m_levels.size()) - 1, center + m_buffer);

    for (auto it = m_loadedNodes.begin(); it != m_loadedNodes.end();) {
        int index = it->first;

        if (index < start || index > end) {
            it->second->removeFromParent();
            it = m_loadedNodes.erase(it);
        }
        else {
            ++it;
        }
    }

    for (int i = start; i <= end; i++) {
        if (!m_loadedNodes.contains(i)) {
            auto node = createLevelNode(m_levels[i], i);
            node->setPosition({ 0.f, -i * m_spacing });
            m_contentLayer->addChild(node);
            m_loadedNodes[i] = node;
        }
    }
}

void endlessmodeLayer::onLevel(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    int index = btn->getTag();

    if (index > m_currentIndex) {
        FLAlertLayer::create(
            "Locked",
            "Complete the previous level first!",
            "OK"
        )->show();
        return;
    }

    requestSpecificLevel(index);
}
void endlessmodeLayer::onBack(CCObject*) {
    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.5f, MenuLayer::scene(false))
    );
}

void endlessmodeLayer::keyBackClicked() {
    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.5f, courseworldLayer::scene())
    );
}





// init function. this code is so messy i need to make a gap here so i can easily spot it while scrolling









// so yeah
bool endlessmodeLayer::init() {

    
    if (!CCLayer::init())
        return false;
    this->setKeypadEnabled(true);
    auto screenSize = CCDirector::sharedDirector()->getWinSize();
    m_currentIndex = Mod::get()->getSavedValue<int>("endless-current-index", 0);
    auto gradientBG = CCLayerGradient::create(
        {50, 46, 140, 255},
        {3, 84, 111, 255}
    );
    gradientBG->setZOrder(-1);
    this->addChild(gradientBG);

    m_scrollArea = CCRect(
        screenSize.width / 2 - 140.f,
        55.f,
        280.f,
        screenSize.height - 120.f
    );

    m_contentLayer = CCNode::create();
    m_contentLayer->setPosition({
        m_scrollArea.getMidX(),
        m_scrollArea.getMaxY() - 40.f
    });
    this->addChild(m_contentLayer);

    this->setTouchEnabled(true);
    this->setTouchMode(cocos2d::kCCTouchesOneByOne);
    this->setTouchPriority(0);
    int seed = Mod::get()->getSavedValue<int>("endless-seed", -1);

    if (seed == -1) {
        seed = randomInt(1, 999999999);
        Mod::get()->setSavedValue<int>("endless-seed", seed);
    }
    for (int i = 0; i < 50; i++) {
        int page = Mod::get()->getSavedValue<int>(
            fmt::format("endless-page-{}", i),
            stableRandomInt(seed + i * 2, 0, 50)
        );

        int levelIndex = Mod::get()->getSavedValue<int>(
            fmt::format("endless-level-index-{}", i),
            stableRandomInt(seed + i * 2 + 1, 0, 9)
        );

        m_levels.push_back({
            i,
            fmt::format("Level {}", i + 1),
            1,
            page,
            levelIndex
        });
    }

    m_currentIndex = Mod::get()->getSavedValue<int>("endless-current-index", 0);

    float maxScroll = std::max(
        0.f,
        static_cast<float>(m_levels.size() - 1) * m_spacing
    );

    m_scrollY = std::clamp(
        static_cast<float>(m_currentIndex) * m_spacing,
        0.f,
        maxScroll
    );

    m_contentLayer->setPositionY(
        m_scrollArea.getMaxY() - 40.f + m_scrollY
    );

    removeOffScreenStuff(m_scrollY);


    auto backMenu = CCMenu::create();
    backMenu->setPosition({30, 290});
    this->addChild(backMenu);

    auto backbtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(endlessmodeLayer::onBack)
    );
    backbtn->setPosition({0, 0});
    backMenu->addChild(backbtn);
/*
    auto resetSprite = ButtonSprite::create("Reset");
    auto resetBtn = CCMenuItemSpriteExtra::create(
        resetSprite,
        this,
        menu_selector(endlessmodeLayer::onResetSeed)
    );
    resetBtn->setPosition({85, 0});

    backMenu->addChild(resetBtn);

    */
    auto rerollSprite = ButtonSprite::create("Reroll");
    auto rerollBtn = CCMenuItemSpriteExtra::create(
        rerollSprite,
        this,
        menu_selector(endlessmodeLayer::onRerollCurrent)
    );

    rerollBtn->setPosition({85, -125});
    backMenu->addChild(rerollBtn);
    auto label = CCLabelBMFont::create("Endless Mode", "goldFont.fnt");
    label->setPosition(screenSize.width / 2, screenSize.height * 5 / 6);
    this->addChild(label);

    return true;
}

bool endlessmodeLayer::ccTouchBegan(CCTouch* touch, CCEvent*) {
    auto pos = touch->getLocation();

    if (!m_scrollArea.containsPoint(pos))
        return false;

    m_lastTouchY = pos.y;
    return true;
}

void endlessmodeLayer::ccTouchMoved(CCTouch* touch, CCEvent*) {
    auto pos = touch->getLocation();

    float deltaY = pos.y - m_lastTouchY;
    m_lastTouchY = pos.y;

    m_scrollY += deltaY;

    float maxScroll = std::max(
        0.f,
        (float)(m_levels.size() - 1) * m_spacing
    );

    m_scrollY = std::clamp(m_scrollY, 0.f, maxScroll);

    m_contentLayer->setPositionY(
        m_scrollArea.getMaxY() - 40.f + m_scrollY
    );

    removeOffScreenStuff(m_scrollY);
}
void endlessmodeLayer::onRerollCurrent(CCObject*) {
    int current = Mod::get()->getSavedValue<int>("endless-current-index", 0);

    Mod::get()->setSavedValue<int>(
        fmt::format("endless-page-{}", current),
        randomInt(0, 50)
    );

    Mod::get()->setSavedValue<int>(
        fmt::format("endless-level-index-{}", current),
        randomInt(0, 9)
    );

    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.3f, endlessmodeLayer::scene())
    );
}

void endlessmodeLayer::onTestComplete(CCObject*) {
    m_currentIndex++;
    Mod::get()->setSavedValue<int>("endless-current-index", m_currentIndex);

    m_loadedNodes.clear();
    m_contentLayer->removeAllChildren();
    removeOffScreenStuff(m_scrollY);
}

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    void onBack(CCObject* sender) {
        if (Mod::get()->getSavedValue<bool>("playing-endless-level", false)) {
            Mod::get()->setSavedValue<bool>("playing-endless-level", false);

            CCDirector::sharedDirector()->replaceScene(
                CCTransitionFade::create(0.5f, endlessmodeLayer::scene())
            );
            return;
        }

        LevelInfoLayer::onBack(sender);
    }
};