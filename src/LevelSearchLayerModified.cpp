#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include "TSLBrowserLayer.hpp"

using namespace geode::prelude;

class $modify(LevelSearchLayerModified, LevelSearchLayer) {
public:
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        CCNode* menu = getChildByID("other-filter-menu");
        if (menu) {
            CircleButtonSprite* sprite = CircleButtonSprite::create(
                CCLabelBMFont::create("Lists", "bigFont.fnt"),
                CircleBaseColor::Green,
                CircleBaseSize::SmallAlt
            );
            
            CCMenuItemSpriteExtra* button = CCMenuItemSpriteExtra::create(
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
        CCScene* scene = TSLBrowserLayer::scene();
        CCTransitionFade* transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }
};
