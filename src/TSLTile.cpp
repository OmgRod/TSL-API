#include "TSLTile.hpp"

TSLTile* TSLTile::create(tsl::List* list, CCSize bounds) {
    TSLTile* instance = new TSLTile();
    if (instance && instance->init(list, bounds)) {
        instance->autorelease();
        return instance;
    }

    delete instance;
    return nullptr;
}

bool TSLTile::init(tsl::List* list, CCSize bounds) {
    if (!CCNode::init()) return false;

    m_list = list;

    CCMenu* container = CCMenu::create();
    container->setContentSize({ bounds.width * 0.3f, bounds.height * 0.75f });
    
    CCScale9Sprite* bg = CCScale9Sprite::create("GJ_square02.png");
    bg->setContentSize(container->getContentSize());
    container->addChild(bg);

    return true;
}