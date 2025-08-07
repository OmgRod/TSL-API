#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include "TSLBrowserLayer.hpp"

using namespace geode::prelude;

class $modify(LevelSearchLayerModified, LevelSearchLayer) {
public:
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        auto menu = getChildByID("other-filter-menu");
        if (menu) {
            auto sprite = CircleButtonSprite::create(
                CCLabelBMFont::create("Lists", "bigFont.fnt"),
                CircleBaseColor::Green,
                CircleBaseSize::SmallAlt
            );

            auto button = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(LevelSearchLayerModified::onTSL)
            );
            menu->addChild(button);
            menu->updateLayout();
        }

        return true;
    }

    void onTSL(CCObject*) {
        auto scene = TSLBrowserLayer::scene();
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }
};
