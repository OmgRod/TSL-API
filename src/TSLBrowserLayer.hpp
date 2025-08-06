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
};
