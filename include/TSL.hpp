#pragma once

#include <Geode/Geode.hpp>
#include <WeeklyPopup.hpp>
#include <TSLListLayer.hpp>

using namespace geode::prelude;

namespace tsl {
    enum StaffRole {
        Owner = 0,
        Dev = 1,
        Admin = 2,
        Helper = 3,
        Other = 4
    };

    const std::unordered_map<StaffRole, std::string> roleColors = {
        {StaffRole::Owner, "r"},
        {StaffRole::Dev, "g"},
        {StaffRole::Admin, "y"},
        {StaffRole::Helper, "j"},
        {StaffRole::Other, "b"}
    };

    const std::unordered_map<StaffRole, std::string> roleStrings = {
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

        static Staff* create(std::string name, StaffRole role, int accountID) {
            Staff* staff = new Staff();
            staff->m_name = name;
            staff->m_role = role;
            staff->m_accountID = accountID;
            return staff;
        }
    };

    struct StaffTeam {
        std::vector<Staff*> m_members;

        void addMember(Staff* staff) {
            m_members.push_back(staff);
        }

        void removeMember(size_t index) {
            if (index < m_members.size()) {
                m_members.erase(m_members.begin() + index);
            }
        }

        Staff* getMember(size_t index) const {
            if (index < m_members.size()) {
                return m_members[index];
            }
            return nullptr;
        }

        size_t memberCount() const {
            return m_members.size();
        }

        static StaffTeam* create() {
            StaffTeam* staff = new StaffTeam();
            return staff;
        }
    };

    struct ListSettings {
        std::string name = "Example List";
        std::string listID = "example-list-id-must-be-unique";
        std::string endpoint = "https://raw.githubusercontent.com/ExampleName/ExampleRepo/refs/heads/main/data/";
        Mod* mod;
        int levelsPerPage = 10;
        bool classic = true;
        bool platformer = false;
        bool weekly = false;
        bool staffInfo = true;
        StaffTeam* staff = StaffTeam::create();
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
        std::string m_name;
        std::string m_endpoint;
        ListSettings m_settings;

        static inline std::vector<List*> s_registeredLists;

        static List* create(ListSettings settings) {
            List* list = new List();
            list->m_settings = settings;
            s_registeredLists.push_back(list);
            return list;
        }

        static const std::vector<List*>& getRegisteredLists() {
            return s_registeredLists;
        }

        const std::string& getName() const {
            return m_settings.name;
        }

        const std::string& getEndpoint() const {
            return m_settings.endpoint;
        }

        std::string generateStaffList() {
            std::unordered_map<tsl::StaffRole, std::vector<tsl::Staff*>> grouped;

            for (auto member : m_settings.staff->m_members) {
                grouped[member->m_role].push_back(member);
            }

            std::string editors;

            for (const auto& [role, roleMembers] : grouped) {
                editors += fmt::format("<c{}>", tsl::roleColors.at(role));
                for (auto member : roleMembers) {
                    editors += fmt::format("{} ({}). [See profile](user:{})", member->m_name, tsl::roleStrings.at(role), member->m_accountID);
                    if (member != roleMembers.back()) editors += "\n\n";
                }
                editors += "</c>\n\n";
            }

            return editors;
        }

        std::unordered_map<int, int> m_levelIds;
        int getLevelCount() {
            return m_levelNames.size();
        }
        void setCachedPage(int page, CCArray* levels) {
            m_cachedPages[page] = levels;
        }
        void clearAllCache() {
            m_levelNames.clear();
            m_levelIds.clear();
            m_cachedPages.clear();

            m_currentWeekly = 0;
            m_cachedWeekly = nullptr;
        }
        CCArray* getCachedPage(int page) {
            return m_cachedPages.contains(page) ? m_cachedPages.at(page) : nullptr;
        }
        void setCurrentWeekly(int id) {
            m_currentWeekly = id;
        }
        int getCurrentWeekly() {
            return m_currentWeekly;
        }
        GJGameLevel* getCachedWeekly() {
            return m_cachedWeekly;
        }
        void setCachedWeekly(GJGameLevel* level) {
            m_cachedWeekly = level;
        }
        void setLocalWeekly(int id) {
            Mod::get()->setSavedValue(fmt::format("local_weekly-{}", m_settings.listID), id);
        }
        int getLocalWeekly() {
            return Mod::get()->getSavedValue<int64_t>(fmt::format("local_weekly-{}", m_settings.listID));
        }
        const std::vector<std::string>& getLevelNames() {
            return m_levelNames;
        }
        void setLayer(TSLListLayer* layer) {
            m_layer = layer;
        }
        TSLListLayer* getLayer() {
            return m_layer;
        }
        int getCount() {
            return m_tempCount;
        }
        void addCount() {
            m_tempCount++;
        }
        void setCount(int count) {
            m_tempCount = count;
        }
        std::unordered_map<int, int> getLevelIds() {
            return m_levelIds;
        }
        int getLevelId(int index) {
            return m_levelIds.contains(index) ? m_levelIds.at(index) : 0;
        }
        void setLevelId(int index, int id) {
            m_levelIds[index] = id;
        }
        void setLevelNames(const std::vector<std::string>& names) {
            m_levelNames = names;
        }
        std::string getRemainingTime() {
            time_t now = time(nullptr);
            tm* tm_now = localtime(&now);

            int days_until_sunday = (7 - tm_now->tm_wday) % 7;
            if (days_until_sunday == 0)
                days_until_sunday = 7;

            tm target_tm = *tm_now;
            target_tm.tm_hour = 0;
            target_tm.tm_min = 0;
            target_tm.tm_sec = 0;
            target_tm.tm_mday += days_until_sunday;
            mktime(&target_tm);

            time_t target_time = mktime(&target_tm);
            double diff_seconds = difftime(target_time, now);

            int hours = diff_seconds / 3600;
            int remaining = static_cast<int>(diff_seconds) % 3600;
            int minutes = remaining / 60;
            int seconds = remaining % 60;

            std::string ret;

            if (hours >= 24) {
                int days = hours / 24;
                ret = fmt::format("{}day{} {}h", days, days > 1 ? "s" : "", hours % 24);
            } else if (hours > 0)
                ret = fmt::format("{}h {}min", hours, minutes);
            else if (minutes > 0)
                ret = fmt::format("{}m {}sec", minutes, seconds);
            else
                ret = fmt::format("{}sec", seconds);
            
            return ret;
        }
        int getTopForLevelId(int id) {
            for (const auto& pair : getLevelIds())
                if (pair.second == id) return pair.first + 1;

            return 0;
        }
    };

    class Request {
    public:
        static void loadWeekly(bool shouldUpdate, tsl::List* list) {
            auto req = web::WebRequest();
            req.header("Content-Type", "application/json");

            req.get(fmt::format("{}{}", list->m_settings.endpoint, "_WEEKLY")).listen([shouldUpdate, list](web::WebResponse* e) {
                auto res = e->string();

                if (res.isErr())
                    return log::error("1. Failed to load weekly: {}", res.unwrapErr());

                int id = utils::numFromString<int>(res.unwrapOr("0")).unwrapOr(0);

                if (id == 0)
                    return log::error("2. Failed to load weekly.");

                list->setCurrentWeekly(id);

                if (list->getLocalWeekly() == 0) list->setLocalWeekly(id);

                if (shouldUpdate && id != 0) {
                    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
                    if (WeeklyPopup* popup = scene->getChildByType<WeeklyPopup>(0))
                        popup->loadLevel();
                }
            });
        }

        void loadPage(int page, tsl::List* list) {
            if (list->getLevelNames().empty())
                return loadLevelNames(true, page, list);

            loadPageLevels(page, list);
        }

        void loadLevelNames(bool shouldLoadLevels, int page, tsl::List* list) {
            if (!list->getLevelNames().empty() && shouldLoadLevels)
                return loadPageLevels(page, list);

            auto req = web::WebRequest();

            req.header("Content-Type", "application/json");

            req.get(fmt::format("{}{}.json", list->m_settings.endpoint, "_list")).listen([&] (web::WebResponse* e) {
                auto res = e->json();
                TSLListLayer* layer = list->getLayer();

                if (res.isErr()) {
                    return log::error("1. Failed to load level names: {}", res.unwrapErr());
                }

                auto json = res.unwrap().asArray();

                if (json.isErr()) {
                    return log::error("2. Failed to load level names: {}", json.unwrapErr());
                }

                std::vector<std::string> names;
                std::vector<std::string> realnames;

                for (auto& value : json.unwrap()) {
                    std::string name = value.asString().unwrapOr("");
                    if (name.empty()) continue;
                    names.push_back(name);
                }

                list->setLevelNames(names);

                if (shouldLoadLevels) loadPageLevels(page, list);
            });
        }

        void loadPageLevels(int page, tsl::List* list) {
            std::vector<std::string> names = list->getLevelNames();
            int levelsPerPage = list->m_settings.levelsPerPage;

            list->setCount(0);

            for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++)
                if (i >= names.size()) list->addCount();
            
            for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
                if (i >= names.size()) continue;

                if (list->getLevelId(i) != 0) {
                    list->addCount();

                    if (list->getCount() >= levelsPerPage)
                        if (TSLListLayer* layer = list->getLayer())
                            layer->loadPage(getLevelsString(page, list));

                    continue;
                }

                auto req = web::WebRequest();
                req.header("Content-Type", "application/json");

                std::string name = names[i];

                replace(name, ' ', "%20");

                req.get(fmt::format("{}{}.json", list->m_settings.endpoint, name)).listen([&](web::WebResponse* e) {
                    auto res = e->json();
                    TSLListLayer* layer = list->getLayer();

                    if (res.isErr()) {
                        if (layer) layer->showError();

                        return log::error("1. Failed to load page levels: {}", res.unwrapErr());
                    }

                    auto json = res.unwrap();

                    int id = json["id"].asInt().unwrapOr(0);

                    if (id == 0) {
                        if (layer) layer->showError();

                        return log::error("2. Failed to load page levels: {}", res.unwrapErr());
                    }

                    list->addCount();
                    list->setLevelId(i, id);

                    if (list->getCount() >= levelsPerPage && layer)
                        layer->loadPage(getLevelsString(page, list));
                    
                });
            }
        }

        std::string getLevelsString(int page, tsl::List* list) {
            std::string str;
            int levelsPerPage = list->m_settings.levelsPerPage;
            log::info("getLevelsString called. Page: {}", page);

            // Log the size of the level names in Cache
            log::info("list->getLevelNames() size: {}", list->getLevelNames().size());

            for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
                log::info("Processing index: {}", i);
                
                // Check if the index is within the valid range
                if (i >= list->getLevelNames().size()) {
                    log::warn("Index {} is out of range. Skipping.", i);
                    continue;
                }

                std::string name = list->getLevelNames()[i];
                log::info("Level name at index {}: {}", i, name);

                // Fetch the level ID
                int id = list->getLevelId(i);
                log::info("Level ID at index {}: {}", i, id);

                // Check if the ID is valid
                if (id == 0) {
                    log::error("Failed to find level ID for index {}. ID is 0. Returning empty string.", i);
                    return "";
                }

                // Add ID to the result string
                str += std::to_string(id) + (i == page * levelsPerPage + levelsPerPage - 1 ? "" : ",");
                log::info("Current result string: {}", str);
            }

            log::info("Returning final string: {}", str);
            return str;
        }

        void replace(std::string& str, char c, std::string r) {
            size_t pos = 0;
            while ((pos = str.find(c, pos)) != std::string::npos) {
                str.replace(pos, 1, r);
                pos += 3;
            }
        }
    };
}
