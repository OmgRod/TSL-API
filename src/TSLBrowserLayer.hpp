#pragma once

#include <Geode/Geode.hpp>
#include <TSL.hpp>

using namespace geode::prelude;

class TSLBrowserLayer : public CCLayer {
public:
    bool init();
    static TSLBrowserLayer* create();
    static CCScene* scene();
    void keyBackClicked();
    void onBack(CCObject*);
    void reloadTiles();
    void onRefresh(CCObject*);
private:
    CCContentLayer* m_sContents = nullptr;
    ScrollLayer* m_scroll = nullptr;
    CCMenu* m_topMenu = nullptr;
};
