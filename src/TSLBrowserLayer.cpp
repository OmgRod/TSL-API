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

    m_scroll = ScrollLayer::create({ winSize.width * 0.8f, winSize.height * 0.7f });
    m_scroll->setPositionX((winSize.width - m_scroll->getContentSize().width) * 0.5f);
    m_sContents = m_scroll->m_contentLayer;
    AxisLayout* layout = AxisLayout::create(Axis::Row);
    layout->setGap(5.f);
    layout->setDefaultScaleLimits(1.f, 1.f);
    layout->setAxisAlignment(AxisAlignment::Center);
    m_sContents->setLayout(layout);

    m_topMenu = CCMenu::create();
    m_topMenu->setPosition({winSize.width - 50.f, winSize.height - 50.f});
    addChild(m_topMenu);
    auto refreshBtnSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    auto refreshBtn = CCMenuItemSpriteExtra::create(refreshBtnSprite, this, menu_selector(TSLBrowserLayer::onRefresh));
    m_topMenu->addChild(refreshBtn);

    addChild(m_scroll);
    reloadTiles();
    return true;
}

void TSLBrowserLayer::reloadTiles() {
    m_sContents->removeAllChildren();
    std::vector<tsl::List*> lists = tsl::ListRegistry::getRegisteredLists();
    log::info("Number of registered lists: {}", lists.size());
    int idx = 0;
    for (tsl::List* list : lists) {
        if (!list) {
            log::warn("List pointer at index {} is null!", idx);
            ++idx;
            continue;
        }
        log::info("List {} pointer: {} name: {}", idx, fmt::ptr(list), list->getName());
        auto container = TSLTile::create(list, m_scroll->getContentSize());
        if (!container) {
            log::warn("TSLTile::create returned null for list {}!", idx);
        } else {
            log::info("Created tile for list {} ({}), adding to content layer.", idx, list->getName());
            m_sContents->addChild(container);
        }
        ++idx;
    }
    m_sContents->updateLayout();
}

void TSLBrowserLayer::onRefresh(CCObject*) {
    reloadTiles();
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
