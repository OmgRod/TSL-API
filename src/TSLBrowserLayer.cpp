#include <TSL.hpp>
#include "TSLBrowserLayer.hpp"
#include "TSLTile.hpp"
#include "Geode/ui/Layout.hpp"

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
    AxisLayout* layout = AxisLayout::create(Axis::Row);
    layout->setGap(5.f);
    layout->setDefaultScaleLimits(1.f, 1.f);
    layout->setAxisAlignment(AxisAlignment::Center);
    sContents->setLayout(layout);
    
    tsl::ListRegistry reg;
    std::vector<tsl::List*> lists = reg.getRegisteredLists();

    log::info("[TSLBrowserLayer] Number of registered lists: {}", lists.size());
    int idx = 0;
    for (tsl::List* list : lists) {
        if (!list) {
            log::warn("[TSLBrowserLayer] List pointer at index {} is null!", idx);
            ++idx;
            continue;
        }
        log::info("[TSLBrowserLayer] List {} pointer: {} name: {}", idx, fmt::ptr(list), list->getName());
        auto container = TSLTile::create(list, scroll->getContentSize());
        if (!container) {
            log::warn("[TSLBrowserLayer] TSLTile::create returned null for list {}!", idx);
        } else {
            log::info("[TSLBrowserLayer] Created tile for list {} ({}), adding to content layer.", idx, list->getName());
            sContents->addChild(container);
        }
        ++idx;
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
