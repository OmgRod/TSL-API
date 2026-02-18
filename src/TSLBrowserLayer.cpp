#include "TSLBrowserLayer.hpp"

bool TSLBrowserLayer::init() {
    if (!CCLayer::init()) return false;

    setKeyboardEnabled(true);
    setKeypadEnabled(true);

    CCDirector* director = CCDirector::sharedDirector();
    CCSize winSize = director->getWinSize();

    CCSprite* background = createLayerBG();
    background->setColor({ 50, 50, 50 });
    addChild(background);

    CCMenu* backMenu = CCMenu::create();
    backMenu->setPosition({ 0.f, 0.f });
    backMenu->setID("back-menu");
    addChild(backMenu);

    CCMenuItemSpriteExtra* backBtn = addBackButton(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        BackButtonStyle::Green
    );
    backBtn->setPosition({ 24.f, winSize.height - 24.f });
    backMenu->addChild(backBtn);

    CCSprite* logo = CCSprite::create("listsLogo.png"_spr);
    logo->setPosition({ winSize.width / 2, winSize.height * 0.9f });
    addChild(logo);

    ScrollLayer* scroll = ScrollLayer::create({ winSize.width * 0.8f, winSize.height * 0.7f });
    scroll->setPositionX((winSize.width - scroll->getContentSize().width) * 0.5f);
    CCContentLayer* sContents = scroll->m_contentLayer;
    SimpleAxisLayout* layout = SimpleAxisLayout::create(Axis::Row);
    layout->setGap(5.f);
    layout->setMainAxisAlignment(MainAxisAlignment::Center);
    sContents->setLayout(layout);
    
    std::vector<tsl::List*> lists = tsl::ListRegistry::getRegisteredLists();
    
    for (tsl::List* list : lists) {
        CCMenu* container = CCMenu::create();
        container->setContentSize({ sContents->getContentSize().width * 0.3f, sContents->getContentSize().height * 0.75f });
        
        CCScale9Sprite* bg = CCScale9Sprite::create("GJ_square02.png");
        bg->setContentSize(container->getContentSize());
        container->addChild(bg);
        
        sContents->addChild(container);
    }

    sContents->updateLayout();
    addChild(scroll);
    
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
    CCScene* scene = CCScene::create();
    scene->addChild(TSLBrowserLayer::create());
    return scene;
}

void TSLBrowserLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void TSLBrowserLayer::onBack(CCObject*) {
    keyBackClicked();
}
