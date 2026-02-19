#pragma once

#include <Geode/Geode.hpp>

namespace tsl { class List; }

using namespace geode::prelude;

class TSLListLayer : public CCLayer, public LevelManagerDelegate, public SetIDPopupDelegate {
private:
    GJListLayer* m_list = nullptr;
    CustomListView* m_customListView = nullptr;
    CCLabelBMFont* m_errorLabel = nullptr;
    CCLabelBMFont* m_pageLabel = nullptr;
    CCLabelBMFont* m_pageButtonLabel = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_refreshButton = nullptr;
    CCMenuItemSpriteExtra* m_infoButton = nullptr;
    tsl::List* m_listData;
    int m_currentPage = 0;
    bool m_isLoading = true;
    bool m_isError = false;
    bool m_didRefresh = false;
    CCLabelBMFont* m_pageCounter = nullptr;
    std::chrono::steady_clock::time_point m_lastRefresh = std::chrono::steady_clock::now();
    bool init(tsl::List* list);
    void keyBackClicked() override;
    void goToPage(int page);
    void showPage(CCArray* levels);
    void onBack(CCObject*);
    void onInfo(CCObject*);
    void onNext(CCObject*);
    void onPrev(CCObject*);
    void onRefresh(CCObject*);
    void onGoToPage(CCObject*);
    void onLastPage(CCObject*);
    void onFirstPage(CCObject*);
    void onWeekly(CCObject*);
    void hideError();
    void showLoading();
    void hideLoading();
    void updatePageLabels();
    int getLastPage();
    virtual void loadLevelsFinished(CCArray* levels, char const*, int) override;
    virtual void loadLevelsFailed(char const*, int) override;
    virtual void setIDPopupClosed(SetIDPopup* popup, int id) override;
public:
    static TSLListLayer* create(tsl::List* list);
    void loadPage(const std::string& str);
    void showError();
    void updateButtons();
    CCScene* scene(tsl::List* list);
};