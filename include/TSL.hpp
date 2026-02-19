#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "WeeklyPopup.hpp"
#include "TSLListLayer.hpp"

using namespace geode::prelude;
using namespace std::chrono;

namespace tsl {
    enum StaffRole {
        Owner = 0,
        Dev = 1,
        Admin = 2,
        Helper = 3,
        Other = 4
    };

    inline const std::unordered_map<StaffRole, std::string> roleColors = {
        {StaffRole::Owner, "r"},
        {StaffRole::Dev, "g"},
        {StaffRole::Admin, "y"},
        {StaffRole::Helper, "j"},
        {StaffRole::Other, "b"}
    };

    inline const std::unordered_map<StaffRole, std::string> roleStrings = {
        {StaffRole::Owner, "Owner"},
        {StaffRole::Dev, "Developer"},
        {StaffRole::Admin, "Admin"},
        {StaffRole::Helper, "Helper"},
        {StaffRole::Other, "Other"}
    };

    struct Staff {
        std::string m_name;
        StaffRole m_role;
        int m_accountID;

        static Staff* create(std::string name, StaffRole role, int accountID);
    };

    struct StaffTeam {
        std::vector<Staff*> m_members;

        void addMember(Staff* staff);
        void removeMember(size_t index);
        Staff* getMember(size_t index) const;
        size_t memberCount() const;
        static StaffTeam* create();
    };

    struct ListSettings {
        std::string name = "Example List";
        std::string listID = "example-list-id-must-be-unique";
        std::string endpoint = "https://raw.githubusercontent.com/ExampleName/ExampleRepo/refs/heads/main/data/";
        Mod* mod;
        int levelsPerPage = 10;
        bool weekly = false;
        bool staffInfo = true;
        StaffTeam* staff = StaffTeam::create();
        CCNode* icon = geode::createModLogo(Mod::get());
        CCNode* iconSmall = CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
    };

    class ListRegistry {
    public:
        std::vector<tsl::List*>& registry();
        static std::vector<List*>& getRegisteredLists();
        static void registerList(List* list);
    };

    class List {
    private:
        TSLListLayer* m_layer = nullptr;
        GJGameLevel* m_cachedWeekly = nullptr;
        int m_currentWeekly = 0;
        std::vector<std::string> m_levelNames;
        std::unordered_map<int, CCArray*> m_cachedPages;
        int m_tempCount = 0;
    public:
        ListSettings* m_settings;
        std::unordered_map<int, int> m_levelIds;

        static List* create(ListSettings* settings);
        const std::string& getName() const;
        const std::string& getEndpoint() const;
        std::string generateStaffList();
        int getLevelCount();
        void setCachedPage(int page, CCArray* levels);
        void clearAllCache();
        CCArray* getCachedPage(int page);
        void setCurrentWeekly(int id);
        int getCurrentWeekly();
        GJGameLevel* getCachedWeekly();
        void setCachedWeekly(GJGameLevel* level);
        void setLocalWeekly(int id);
        int getLocalWeekly();
        const std::vector<std::string>& getLevelNames();
        void setLayer(TSLListLayer* layer);
        TSLListLayer* getLayer();
        int getCount();
        void addCount();
        void setCount(int count);
        std::unordered_map<int, int> getLevelIds();
        int getLevelId(int index);
        void setLevelId(int index, int id);
        void setLevelNames(const std::vector<std::string>& names);
        std::string getRemainingTime();
        int getTopForLevelId(int id);
    };

    class Request {
    public:
        static void loadWeekly(bool shouldUpdate, tsl::List* list);
        void loadPage(int page, tsl::List* list);
        void loadLevelNames(bool shouldLoadLevels, int page, tsl::List* list);
        void loadPageLevels(int page, tsl::List* list);
        std::string getLevelsString(int page, tsl::List* list);

        void replace(std::string& str, char c, std::string r);
    };
}
