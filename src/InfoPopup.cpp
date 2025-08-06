#include "InfoPopup.hpp"

InfoPopup* InfoPopup::create(tsl::List* list) {
    InfoPopup* ret = new InfoPopup();
    if (ret->initAnchored(270, 250, list, "square01_001.png", CCRectZero)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool InfoPopup::setup(tsl::List* list) {
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