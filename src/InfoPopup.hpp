#include <Geode/Geode.hpp>
#include <TSL.hpp>

using namespace geode::prelude;

class InfoPopup : public geode::Popup<tsl::List*> {
public:
    static InfoPopup* create(tsl::List* list);
private:
    bool setup(tsl::List* list);
};