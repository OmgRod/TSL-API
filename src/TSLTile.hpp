#include <Geode/Geode.hpp>
#include <TSL.hpp>

using namespace geode::prelude;

class TSLTile : public CCNode {
public:
    tsl::List* m_list;
    
    static TSLTile* create(tsl::List* list, CCSize bounds);
    bool init(tsl::List* list, CCSize bounds);
};
