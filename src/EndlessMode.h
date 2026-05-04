#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct EndlessLevel {
    int id;
    std::string name;
    int difficulty;

    int page;
    int levelIndex;
};

class endlessmodeLayer : public cocos2d::CCLayer {
public:
    static endlessmodeLayer* create();
    static cocos2d::CCScene* scene();

    bool init() override;

    void onBack(cocos2d::CCObject*);
    void onRequestLevels(cocos2d::CCObject*);
    void onLevel(cocos2d::CCObject*);

    cocos2d::CCNode* createLevelNode(EndlessLevel const& level, int index);
    void removeOffScreenStuff(float cameraY);
    const char* getSpotTexture(int index, int currentIndex);

    std::vector<EndlessLevel> m_levels;
    std::unordered_map<int, cocos2d::CCNode*> m_loadedNodes;

    float m_spacing = 140.f;
    int m_buffer = 8;
    int m_currentIndex = 0;
    CCNode* m_contentLayer = nullptr;
    CCRect m_scrollArea;

    float m_scrollY = 0.f;
    float m_lastTouchY = 0.f;

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;

    void requestSpecificLevel(int buttonIndex);
    void onResetSeed(CCObject*);
    void onTestComplete(CCObject*);
    void onRerollCurrent(CCObject*);
    void keyBackClicked() override;
};
