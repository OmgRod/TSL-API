#pragma once

#include <Geode/Geode.hpp>
#include "TSL.hpp"

using namespace geode::prelude;

namespace tsl {
    class List;
}

class WeeklyCell : public LevelCell  {
public:
    void draw() override {}
    static WeeklyCell* create();
    WeeklyCell(char const* p0, float p1, float p2) : LevelCell(p0, p1, p2) {}
};

class WeeklyPopup : public geode::Popup<tsl::List*>, public LevelManagerDelegate {
public:
    tsl::List* m_list;
    static WeeklyPopup* create(tsl::List* list);
    void showError();
    void loadLevel();
private:
    WeeklyCell* m_cell = nullptr;
    CCNode* m_cellContainer = nullptr;
    CCLabelBMFont* m_timeLabel = nullptr;
    CCLabelBMFont* m_errorLabel = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_discardButton = nullptr;
    bool m_isLoading = true;
    bool m_isError = false;
    ~WeeklyPopup();
    bool setup(tsl::List* list) override;
    void showLevel(GJGameLevel* level);
    void showLoading();
    void hideLoading();
    void hideError();
    void updateTime();
    void onDiscard(CCObject*);
    virtual void loadLevelsFinished(cocos2d::CCArray* levels, char const*, int) override;
    virtual void loadLevelsFailed(char const*, int) override;
};