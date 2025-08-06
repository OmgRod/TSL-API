#include "TSLBrowserLayer.hpp"

bool TSLBrowserLayer::init() {
    if (!CCLayer::init()) return false;

    setKeyboardEnabled(true);
    setKeypadEnabled(true);

    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    auto background = createLayerBG();
    background->setColor({ 50, 50, 50 });
    addChild(background);

    auto backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(TSLBrowserLayer::onBack));
    backBtn->setSizeMult(1.6f);

    auto backMenu = CCMenu::create(backBtn, 0);
    addChild(backMenu, 1);
    backMenu->setPosition({ director->getScreenLeft() + 25.f, director->getScreenTop() - 22.f });

    if(PlatformToolbox::isControllerConnected()) GameToolbox::addBackButton(this, backBtn);

    auto logo = CCSprite::create("listsLogo.png"_spr);
    logo->setPosition({ winSize.width / 2, winSize.height * 0.9f });
    addChild(logo);

    auto lists = tsl::List::getRegisteredLists();

    return true;
}

TSLBrowserLayer* TSLBrowserLayer::create() {
    TSLBrowserLayer* instance = new TSLBrowserLayer();
    if (instance && instance->init()) {
        instance->autorelease();
        return instance;
    }

    delete instance;
    return nullptr;
}

CCScene* TSLBrowserLayer::scene() {
    auto scene = CCScene::create();
    scene->addChild(TSLBrowserLayer::create());
    return scene;
}

void TSLBrowserLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void TSLBrowserLayer::onBack(CCObject*) {
    keyBackClicked();
}
