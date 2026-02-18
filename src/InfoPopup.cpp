#include "InfoPopup.hpp"

InfoPopup* InfoPopup::create(tsl::List* list) {
    InfoPopup* ret = new InfoPopup();
    ret->m_bgSprite = NineSlice::create("square01_001.png");
    if (ret->init(list)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool InfoPopup::init(tsl::List* list) {
    if (!Popup::init(270, 250)) return false;

    setTitle("Staff Team");
    m_closeBtn->setVisible(false);

    MDTextArea* textArea = MDTextArea::create(list->generateStaffList(), {220, 150});

    m_mainLayer->addChild(textArea);
    textArea->setPosition(m_size / 2 + ccp(0, 13));
    textArea->setAnchorPoint({0.5f, 0.5f});

    CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Ok"),
        this,
        menu_selector(InfoPopup::onClose)
    );
    closeButton->setPosition({m_size.width / 2, 37});

    m_buttonMenu->addChild(closeButton);

    return true;
}