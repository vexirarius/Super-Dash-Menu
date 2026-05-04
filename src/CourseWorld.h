#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class courseworldLayer : public CCLayer {
public:
    static courseworldLayer* create();

    static CCScene* scene();
    bool init() override;

    void onBack(CCObject*);
    void onCourses(CCObject*);
    void onLeaderboards(CCObject*);
    void onEndless(CCObject*);
    void onMainLevels(CCObject*);
    void onIconKit(CCObject*);
    void keyBackClicked() override;
};