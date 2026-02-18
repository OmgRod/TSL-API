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

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    addChild(menu);

    auto backBtn = addBackButton(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        BackButtonStyle::Green
    );
    backBtn->setPosition({ 40.f, winSize.height - 40.f });
    menu->addChild(backBtn);

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
