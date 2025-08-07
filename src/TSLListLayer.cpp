#include <TSL.hpp>
#include <TSLListLayer.hpp>
#include <WeeklyPopup.hpp>
#include "InfoPopup.hpp"
#include "DoubleArrow.hpp"

bool TSLListLayer::init(tsl::List* list) {
    if (!CCLayer::init()) return false;

    m_listData = list;
    list->setLayer(this);

    setKeypadEnabled(true);
    cocos::handleTouchPriority(this, true);

    CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    backgroundSprite->setID("background");
    
    cocos2d::CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCSize size = backgroundSprite->getContentSize();
    backgroundSprite->setScaleX(winSize.width / size.width);
    backgroundSprite->setScaleY(winSize.height / size.height);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({100, 100, 100});
    backgroundSprite->setZOrder(-1);
    addChild(backgroundSprite); 

    CCMenu* menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setAnchorPoint({ 0.f, 0.f });
    addChild(menu);

    CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(TSLListLayer::onBack)
    );
    button->setPosition({25, winSize.height - 25});

    menu->addChild(button);

    CCSprite* bottomLeft = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomLeft->setPosition({-1,-1});
    bottomLeft->setAnchorPoint({0,0});
    addChild(bottomLeft, -1);

    CCSprite* bottomRight = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    bottomRight->setPosition({winSize.width + 1, -1});
    bottomRight->setAnchorPoint({1,0});
    bottomRight->setFlipX(true);
    addChild(bottomRight, -1);
    
    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setParentLayer(this);
    m_loadingCircle->retain();
    m_loadingCircle->show();

    m_errorLabel = CCLabelBMFont::create("Unable to load page", "bigFont.fnt");
    m_errorLabel->setPosition(winSize / 2);
    m_errorLabel->setVisible(false);
    m_errorLabel->setOpacity(125);
    m_errorLabel->setScale(0.73f);
    addChild(m_errorLabel, 2);

    spr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    m_refreshButton = CCMenuItemSpriteExtra::create(spr,
        this,
        menu_selector(TSLListLayer::onRefresh)
    );
    m_refreshButton->setCascadeOpacityEnabled(true);
    m_refreshButton->setPosition({winSize.width - 27, 27});

    menu->addChild(m_refreshButton);

    spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    spr->setFlipX(true);

    m_nextButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(TSLListLayer::onNext)
    );
    m_nextButton->setCascadeOpacityEnabled(true);
    m_nextButton->setPosition({winSize.width - 24.f, winSize.height / 2.f});

    menu->addChild(m_nextButton);

    spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");

    m_prevButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(TSLListLayer::onPrev)
    );
    m_prevButton->setCascadeOpacityEnabled(true);
    m_prevButton->setPosition({24.f, winSize.height / 2.f});

    menu->addChild(m_prevButton);
    
    if (m_listData->m_settings->staffInfo && m_listData->m_settings->staff) {
        std::string editors = m_listData->generateStaffList();

        spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");

        m_infoButton = CCMenuItemSpriteExtra::create(spr, this, menu_selector(TSLListLayer::onInfo));
        m_infoButton->setPosition({ 30.0f, 30.0f });
        m_infoButton->setCascadeOpacityEnabled(true);
        menu->addChild(m_infoButton);

        if (editors.empty()) m_infoButton->setVisible(false);
    }

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setPosition({winSize.width - 2.f, winSize.height - 2.f});
    m_pageLabel->setAnchorPoint({1.f, 1.f});
    m_pageLabel->setScale(0.56f);
    addChild(m_pageLabel);

    m_list = GJListLayer::create(nullptr, m_listData->m_settings->name.c_str(), {191, 114, 62, 255}, 356.f, 220.f, 0);
    m_list->setPosition(winSize / 2 - m_list->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(m_list);

    spr = CCSprite::create("GJ_button_02.png");
    spr->setScale(0.7f);

    m_pageButtonLabel = CCLabelBMFont::create("1", "bigFont.fnt");
    m_pageButtonLabel->setPosition(spr->getContentSize() / 2);
    spr->addChild(m_pageButtonLabel);

    CCMenuItemSpriteExtra* goToPageButton = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(TSLListLayer::onGoToPage)
    );
    goToPageButton->setPosition({winSize.width - 21, winSize.height - 41});

    menu->addChild(goToPageButton);

    CCMenuItemSpriteExtra* lastPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(true, "GJ_arrow_03_001.png"),
        this,
        menu_selector(TSLListLayer::onLastPage)
    );
    lastPageButton->setPosition({winSize.width - 21, winSize.height - 74});

    menu->addChild(lastPageButton);

    CCMenuItemSpriteExtra* firstPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(false, "GJ_arrow_03_001.png"),
        this,
        menu_selector(TSLListLayer::onFirstPage)
    );
    firstPageButton->setPosition({21.f, winSize.height - 74});

    menu->addChild(firstPageButton);

    auto buttonIcon = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("gj_dailyCrown_001.png"),
        CircleBaseColor::Green,
        CircleBaseSize::Medium
    );
    buttonIcon->setScale(1.175f);

    if (m_listData->m_settings->weekly) {
        CCMenuItemSpriteExtra* weeklyButton = CCMenuItemSpriteExtra::create(buttonIcon, this, menu_selector(TSLListLayer::onWeekly));
        weeklyButton->setPosition({winSize.width - (weeklyButton->getContentWidth() / 2) - 10, winSize.height - 225});
        
        menu->addChild(weeklyButton);
    }

    showLoading();

    goToPage(m_currentPage);
    
    updatePageLabels();

    return true;
}

TSLListLayer* TSLListLayer::create(tsl::List* list) {
    TSLListLayer* instance = new TSLListLayer();
    if (instance && instance->init(list)) {
        instance->autorelease();
        return instance;
    }

    delete instance;
    return nullptr;
}

CCScene* TSLListLayer::scene(tsl::List* list) {
    auto scene = CCScene::create();
    scene->addChild(TSLListLayer::create(list));
    return scene;
}

void TSLListLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void TSLListLayer::onBack(CCObject*) {
    keyBackClicked();
}

int TSLListLayer::getLastPage() {
    return (m_listData->getLevelCount() + m_listData->m_settings->levelsPerPage) / m_listData->m_settings->levelsPerPage;
}

void TSLListLayer::onInfo(CCObject*) {
    InfoPopup::create(m_listData)->show();
}

void TSLListLayer::onWeekly(CCObject*) {
    if (m_listData->m_settings->weekly) WeeklyPopup::create(m_listData)->show();
}

void TSLListLayer::onRefresh(CCObject*) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastRefresh).count();

    if (elapsed < (3 + (std::rand() % 3)) && !m_isError && m_didRefresh) return;

    m_lastRefresh = currentTime;
    m_didRefresh = true;

    m_listData->clearAllCache();

    if (m_listData->m_settings->weekly) tsl::Request::loadWeekly(true, m_listData);

    goToPage(m_currentPage);

    hideError();
    showLoading();
    updateButtons();
}

void TSLListLayer::goToPage(int page) {
    if (CCArray* cachedPage = m_listData->getCachedPage(page)) {
        showPage(cachedPage);
    } else {
        tsl::Request req;
        req.loadPage(page, m_listData);
    }
}

void TSLListLayer::showPage(cocos2d::CCArray* levels) {
    auto levelsArray = typeinfo_cast<CCArray*>(levels);
    if (levelsArray && levelsArray->count() > 0) {
        m_customListView = CustomListView::create(levels, BoomListType::Level, 220.0, 356.0);
        m_list->addChild(m_customListView);

        for (LevelCell* cell : CCArrayExt<LevelCell*>(m_customListView->m_tableView->m_cellArray)) {
            if (!typeinfo_cast<LevelCell*>(cell)) continue;

            int top = 0;
            top = abs(m_listData->getTopForLevelId(cell->m_level->m_levelID.value()));

            std::string topStr = std::to_string(top);

            if (top == 0) topStr = "NA";
	        if (top > 150) topStr = "Legacy";

            int coins = cell->m_level->m_coins;

            CCLabelBMFont* topLabel = CCLabelBMFont::create(topStr.c_str(), top < 6 ? "goldFont.fnt" : "bigFont.fnt");
            topLabel->setOpacity(150);
            if (auto diffContainer = cell->m_mainLayer->getChildByID("difficulty-container")) {
                topLabel->setPosition({ diffContainer->getPositionX(), coins > 0 ? 1.f : 1.f });
            } else {
                log::debug("difficulty-container not found. :(");
            }
            if (Loader::get()->isModLoaded("cvolton.compact_lists") && cell->getContentSize().height < 90) {
                float scale = (topStr == "Legacy") ? 0.5f : (top < 6 ? 0.45f : 0.32f);
                topLabel->limitLabelWidth(25.f, scale, 0.001f);
                topLabel->setAnchorPoint({ 0.5f, 0.f });
                if (topStr == "Legacy") {
                    topLabel->setScale(0.25f);
                } else {
                    topLabel->setScale(scale);
                }
            } else {
                float scale = (topStr == "Legacy") ? 0.6f : (coins > 0 ? (top < 6 ? 0.55f : 0.4f) : (top < 6 ? 0.65f : 0.5f));
                topLabel->limitLabelWidth(25.f, scale, 0.001f);
                topLabel->setPositionY(coins > 0 ? 9.f : 14.f);
                if (topStr == "Legacy") {
                    topLabel->setScale(0.35f);
                } else {
                    topLabel->setScale(scale);
                }
            }

            if (top > 75) topLabel->setColor(ccc3(233, 233, 233));
            else if (top > 150) topLabel->setColor(ccc3(188, 188, 188));

            if (CCNode* mainLayer = cell->getChildByID("main-layer")) {
                mainLayer->addChild(topLabel);

                if (coins < 1) continue;

                if (CCNode* container = mainLayer->getChildByID("difficulty-container")) {
                    if (CCNode* coin1 = container->getChildByID("coin-icon-1")) {
                        coin1->setScale(0.65f);
                        coin1->setPosition(coin1->getPosition() + ccp(coins == 2 ? 1.5f : (coins == 3 ? 3.f : 0.f), 3));
                    }
                    if (CCNode* coin2 = container->getChildByID("coin-icon-2")) {
                        coin2->setScale(0.65f);
                        coin2->setPosition(coin2->getPosition() + ccp(coins == 2 ? -1.5f : 0.f, 3));
                    }
                    if (CCNode* coin3 = container->getChildByID("coin-icon-3")) {
                        coin3->setScale(0.65f);
                        coin3->setPosition(coin3->getPosition() + ccp(coins == 3 ? -3.f : 0.f, 3));
                    }
                }
            }
        }

        hideLoading();
        hideError();
        updateButtons();
    } else {
        return showError();
    }
}

void TSLListLayer::loadPage(const std::string& str) {
    if (str.empty()) return showError();

    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->m_levelManagerDelegate = this;
    glm->getOnlineLevels(GJSearchObject::create(SearchType::Type19, str));
}

void TSLListLayer::loadLevelsFinished(cocos2d::CCArray* levels, char const*, int) {
    m_listData->setCachedPage(m_currentPage, levels);
    showPage(levels);
}

void TSLListLayer::loadLevelsFailed(char const*, int) {
    showError();
}

void TSLListLayer::onNext(CCObject*) {
    m_currentPage++;

    showLoading();

    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void TSLListLayer::onPrev(CCObject*) {
    m_currentPage--;

    if (m_currentPage < 0) m_currentPage = 0;

    showLoading();

    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void TSLListLayer::updatePageLabels() {
    int pageMax = m_currentPage * m_listData->m_settings->levelsPerPage + m_listData->m_settings->levelsPerPage;
    m_pageLabel->setString(fmt::format(
        "{} to {} of {}",
        m_currentPage * m_listData->m_settings->levelsPerPage + 1,
        pageMax > m_listData->getLevelCount() ? m_listData->getLevelCount() : pageMax,
        m_listData->getLevelCount()
    ).c_str());

    m_pageButtonLabel->setString(std::to_string(m_currentPage + 1).c_str());
    m_pageButtonLabel->limitLabelWidth(28.f, 0.8f, 0.001f);
}

void TSLListLayer::updateButtons() {
    bool enableArrows = !m_isLoading;

    m_nextButton->setEnabled(enableArrows && m_currentPage != getLastPage() - 1);
    m_nextButton->setOpacity(enableArrows && m_currentPage != getLastPage() - 1 ? 255 : 125);

    m_prevButton->setEnabled(enableArrows && m_currentPage != 0);
    m_prevButton->setOpacity(enableArrows && m_currentPage != 0 ? 255 : 125);

    m_refreshButton->setEnabled(!m_isLoading);
    m_refreshButton->setOpacity(!m_isLoading ? 255 : 200);

    std::string editors = m_listData->generateStaffList();
    m_infoButton->setEnabled(!editors.empty());
    m_infoButton->setOpacity(!editors.empty() ? 255 : 125);
}

void TSLListLayer::showError() {
    m_errorLabel->setVisible(true);
    m_isError = true;

    hideLoading();
    updateButtons();
}

void TSLListLayer::hideError() {
    m_errorLabel->setVisible(false);
    m_isError = false;

    updateButtons();
}

void TSLListLayer::showLoading() {
    if (m_customListView) {
        m_customListView->removeFromParentAndCleanup(true);
        m_customListView = nullptr;
    }

    m_loadingCircle->setVisible(true);
    m_isLoading = true;

    hideError();
    updateButtons();
}

void TSLListLayer::hideLoading() {
    m_loadingCircle->setVisible(false);
    m_isLoading = false;

    updateButtons();
}

void TSLListLayer::onGoToPage(CCObject* sender) {
    auto popup = SetIDPopup::create(m_currentPage + 1, 1, getLastPage(), "Go to Page", "Search", true, 0, 0.f, true, true);
    popup->m_delegate = this;
    
    popup->show();
}

void TSLListLayer::setIDPopupClosed(SetIDPopup* popup, int id) {
    id = std::clamp(id, 1, getLastPage());

    m_currentPage = id - 1;

    showLoading();
    goToPage(m_currentPage);

    updateButtons();
    updatePageLabels();
}

void TSLListLayer::onLastPage(CCObject* sender) {
    showLoading();
    m_currentPage = getLastPage() - 1;
    goToPage(m_currentPage);
    
    updateButtons();
    updatePageLabels();
}

void TSLListLayer::onFirstPage(CCObject* sender) {
    showLoading();
    m_currentPage = 0;
    goToPage(m_currentPage);
    
    updateButtons();
    updatePageLabels();
}
