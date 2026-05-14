
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CreatorLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/SecretLayer6.hpp>
#include <Geode/binding/LevelSelectLayer.hpp>
#include <Geode/binding/SearchButton.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include "CourseWorld.h"
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	struct Fields {
		// Fields
		CCNode* playbuttonWrapper = nullptr;
		CCMenu* playButtonMenu = nullptr;
		
		CCMenu* newMenu;
	};

	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}
		
		CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

// DELETE the old menu. WE HATE YOU OLD MENU BECAUSE YOU ARE UGLY AND BAD AND WE WANT TO REPLACE YOU WITH A NEW ONE THAT LOOKS LIKE MARIO MAKER 2

		auto oldMenu = this->getChildByID("main-menu");
			if (oldMenu) 
			{
				oldMenu->setVisible(false);
			}
				
		// make the main menu stuff
			
		m_fields->newMenu = CCMenu::create();
		m_fields->newMenu->setID("super-menu");
		this->addChild(m_fields->newMenu);

		m_fields->playButtonMenu = CCMenu::create();
		m_fields->playButtonMenu->setID("play-button-menu");
		this->addChild(m_fields->playButtonMenu);
		m_fields->playButtonMenu->setVisible(false);
		m_fields->playButtonMenu->setScale(0.0f);

		// buttons and stuff etc etc etc

		auto playspr = CCSprite::create("PlayButton.png"_spr);
		playspr->setScale(0.1f);

		auto playbtn = CCMenuItemSpriteExtra::create(
			playspr,
			this,
			menu_selector(MyMenuLayer::onPlayButton)
		);

		auto createspr = CCSprite::create("CreateButton.png"_spr);
		createspr->setScale(0.1f);

		auto createbtn = CCMenuItemSpriteExtra::create
		(
			createspr,
			this,
			menu_selector(MyMenuLayer::onCreateButton)
		);

		// play button stuff

		auto courseworldspr = SearchButton::create(
			"GJ_longBtn03_001.png",
			"Course World",
			0.5f,
			"GJ_downloadsIcon_001.png"
		);
			CCSprite* courseworldicon = nullptr;
		for (auto child : CCArrayExt<CCNode*>(courseworldspr->getChildren()))
			if (auto spr = typeinfo_cast<CCSprite*>(child)) {
				courseworldicon = spr;
				break;
			}
		
		
		if (courseworldicon) {
		courseworldicon->setScale(0.7f);
		}
		auto courseworldbtn = CCMenuItemSpriteExtra::create(
			courseworldspr,
			this,
			menu_selector(MyMenuLayer::onCourseWorldButton)
		);
		    auto backspr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    			backspr->setScale(0.7f);

		auto backbtn = CCMenuItemSpriteExtra::create(
			backspr,
			this,
			menu_selector(MyMenuLayer::onBackButton)
		);


		m_fields->playButtonMenu->addChild(courseworldbtn);

		// animation and stuff for play and create buttons

		auto scaleAnimation1 = CCScaleTo::create(1, 0.11f);
		auto scaleBackAnimation2 = CCScaleTo::create(1, 0.1f);
		auto scaleAnimation1B = CCScaleTo::create(1, 0.11f);
		auto scaleBackAnimation2B = CCScaleTo::create(1, 0.1f);

		auto animationSequence1 = CCSequence::create(scaleAnimation1, scaleBackAnimation2, nullptr);
		auto animationSequence2 = CCSequence::create(scaleAnimation1B, scaleBackAnimation2B, nullptr);

		auto easing1 = CCEaseSineInOut::create(animationSequence1);
		auto easing2 = CCEaseSineInOut::create(animationSequence2);

		auto animationRepeat1 = CCRepeatForever::create(easing1);
		auto animationRepeat2 = CCRepeatForever::create(easing2);

		// make the buttons exist and stuff

		this->getChildByID("super-menu")->addChild(playbtn);
		this->getChildByID("super-menu")->addChild(createbtn);
		playbtn->setPosition({-50,-0});
		createbtn->setPosition({50,0});
		playspr->runAction(animationRepeat1);
		createspr->runAction(animationRepeat2);

		// init()
		m_fields->playbuttonWrapper = CCNode::create();
		m_fields->playbuttonWrapper->setPosition(screenSize / 2);
		m_fields->playbuttonWrapper->setVisible(false);
		m_fields->playbuttonWrapper->setScale(0.01f);
		this->addChild(m_fields->playbuttonWrapper, 10);

		m_fields->playButtonMenu = CCMenu::create();
		m_fields->playButtonMenu->setPosition({0, 0});
		m_fields->playbuttonWrapper->addChild(m_fields->playButtonMenu);

		courseworldbtn->setPosition({0, 0});
		backbtn->setPosition({-70, 50});
		m_fields->playButtonMenu->addChild(courseworldbtn);
		m_fields->playButtonMenu->addChild(backbtn);

		return true;
	}

	void onCreateButton(CCObject* sender) 
	{
		auto scene = CreatorLayer::scene();
		CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
	}

	

	void onPlayButton(CCObject*) {
		m_fields->playbuttonWrapper->setVisible(true);
		m_fields->playbuttonWrapper->stopAllActions();
		m_fields->playbuttonWrapper->setScale(0.01f);
		m_fields->newMenu->setTouchEnabled(false);
		m_fields->newMenu->runAction(CCFadeOut::create(0.5f));
		
		m_fields->playbuttonWrapper->runAction(
			CCEaseBackOut::create(CCScaleTo::create(0.5f, 1.0f))
		);

	}

	void onCourseWorldButton(CCObject* sender) 
	{
		auto scene = courseworldLayer::scene();
		CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
	}
	
	void onBackButton(CCObject* sender) 
	{
		m_fields->playbuttonWrapper->setVisible(true);
		m_fields->playbuttonWrapper->stopAllActions();
		m_fields->playbuttonWrapper->setScale(0.01f);
		m_fields->newMenu->setTouchEnabled(true);
		m_fields->newMenu->runAction(CCFadeIn::create(0.5f));
		
		m_fields->playbuttonWrapper->runAction(
			CCEaseBackOut::create(CCScaleTo::create(0.5f, 0.01f))
		);
		m_fields->playbuttonWrapper->setVisible(false);
		
	}	
	
};
