#include <Geode/Geode.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <TSL.hpp>

using namespace geode::prelude;

class InfoPopup : public geode::Popup {
public:
    static InfoPopup* create(tsl::List* list);
private:
    bool init(tsl::List* list);
};