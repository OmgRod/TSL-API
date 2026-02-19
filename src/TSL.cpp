#include <TSL.hpp>
#include "TSLListLayer.hpp"
#include "WeeklyPopup.hpp"
#include <Geode/Geode.hpp>

using namespace tsl;
using namespace geode::prelude;

Staff* Staff::create(std::string name, StaffRole role, int accountID) {
    Staff* staff = new Staff();
    staff->m_name = name;
    staff->m_role = role;
    staff->m_accountID = accountID;
    return staff;
}

void StaffTeam::addMember(Staff* staff) {
    m_members.push_back(staff);
}

void StaffTeam::removeMember(size_t index) {
    if (index < m_members.size()) {
        m_members.erase(m_members.begin() + index);
    }
}

Staff* StaffTeam::getMember(size_t index) const {
    if (index < m_members.size()) {
        return m_members[index];
    }
    return nullptr;
}

size_t StaffTeam::memberCount() const {
    return m_members.size();
}

StaffTeam* StaffTeam::create() {
    StaffTeam* staff = new StaffTeam();
    return staff;
}

std::vector<List*>& ListRegistry::getRegisteredLists() {
    static std::vector<List*> s_registeredLists;
    return s_registeredLists;
}

void ListRegistry::registerList(List* list) {
    getRegisteredLists().push_back(list);
}

std::vector<tsl::List*>& ListRegistry::registry() {
    static auto* reg = new std::vector<tsl::List*>();
    return *reg;
}

List* List::create(ListSettings* settings) {
    List* list = new List();
    list->m_settings = settings;
    ListRegistry::registerList(list);
    return list;
}

const std::string& List::getName() const {
    return m_settings->name;
}

const std::string& List::getEndpoint() const {
    return m_settings->endpoint;
}

std::string List::generateStaffList() {
    std::unordered_map<tsl::StaffRole, std::vector<tsl::Staff*>> grouped;

    for (auto member : m_settings->staff->m_members) {
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

int List::getLevelCount() {
    return m_levelNames.size();
}

void List::setCachedPage(int page, CCArray* levels) {
    m_cachedPages[page] = levels;
}

void List::clearAllCache() {
    m_levelNames.clear();
    m_levelIds.clear();
    m_cachedPages.clear();

    m_currentWeekly = 0;
    m_cachedWeekly = nullptr;
}

CCArray* List::getCachedPage(int page) {
    return m_cachedPages.contains(page) ? m_cachedPages.at(page) : nullptr;
}

void List::setCurrentWeekly(int id) {
    m_currentWeekly = id;
}

int List::getCurrentWeekly() {
    return m_currentWeekly;
}

GJGameLevel* List::getCachedWeekly() {
    return m_cachedWeekly;
}

void List::setCachedWeekly(GJGameLevel* level) {
    m_cachedWeekly = level;
}

void List::setLocalWeekly(int id) {
    Mod::get()->setSavedValue(fmt::format("local_weekly-{}", m_settings->listID), id);
}

int List::getLocalWeekly() {
    return Mod::get()->getSavedValue<int64_t>(fmt::format("local_weekly-{}", m_settings->listID));
}

const std::vector<std::string>& List::getLevelNames() {
    return m_levelNames;
}

void List::setLayer(TSLListLayer* layer) {
    m_layer = layer;
}

TSLListLayer* List::getLayer() {
    return m_layer;
}

int List::getCount() {
    return m_tempCount;
}

void List::addCount() {
    m_tempCount++;
}

void List::setCount(int count) {
    m_tempCount = count;
}

std::unordered_map<int, int> List::getLevelIds() {
    return m_levelIds;
}

int List::getLevelId(int index) {
    return m_levelIds.contains(index) ? m_levelIds.at(index) : 0;
}

void List::setLevelId(int index, int id) {
    m_levelIds[index] = id;
}

void List::setLevelNames(const std::vector<std::string>& names) {
    m_levelNames = names;
}

std::string List::getRemainingTime() {
    auto now = system_clock::now();
    std::time_t now_time_t = system_clock::to_time_t(now);
    std::tm tm_now;
    #ifdef _WIN32
    localtime_s(&tm_now, &now_time_t);
    #else
    localtime_r(&now_time_t, &tm_now);
    #endif

    int days_until_sunday = (7 - tm_now.tm_wday) % 7;
    if (days_until_sunday == 0)
        days_until_sunday = 7;

    std::tm tm_sunday = tm_now;
    tm_sunday.tm_mday += days_until_sunday;
    tm_sunday.tm_hour = 0;
    tm_sunday.tm_min = 0;
    tm_sunday.tm_sec = 0;
    std::time_t sunday_midnight = mktime(&tm_sunday);

    double diff_seconds = difftime(sunday_midnight, now_time_t);

    int hours_left = static_cast<int>(diff_seconds) / 3600;
    int remaining = static_cast<int>(diff_seconds) % 3600;
    int minutes = remaining / 60;
    int seconds = remaining % 60;

    if (hours_left >= 24) {
        int days = hours_left / 24;
        return fmt::format("{}day{} {}h", days, days > 1 ? "s" : "", hours_left % 24);
    } else if (hours_left > 0)
        return fmt::format("{}h {}min", hours_left, minutes);
    else if (minutes > 0)
        return fmt::format("{}m {}sec", minutes, seconds);
    else
        return fmt::format("{}sec", seconds);
}

int List::getTopForLevelId(int id) {
    for (const auto& pair : getLevelIds())
        if (pair.second == id) return pair.first + 1;

    return 0;
}

void Request::loadWeekly(bool shouldUpdate, List* list) {
    auto req = web::WebRequest();
    req.header("Content-Type", "application/json");

    static async::TaskHolder<web::WebResponse> holder;
    holder.spawn(
        req.get(fmt::format("{}{}", list->m_settings->endpoint, "_WEEKLY")),
        [shouldUpdate, list](web::WebResponse e) {
            auto res = e.string();

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
        }
    );
}

void Request::loadPage(int page, List* list) {
    if (list->getLevelNames().empty())
        return loadLevelNames(true, page, list);

    loadPageLevels(page, list);
}

void Request::loadLevelNames(bool shouldLoadLevels, int page, List* list) {
    if (!list->getLevelNames().empty() && shouldLoadLevels)
        return loadPageLevels(page, list);

    auto req = web::WebRequest();
    req.header("Content-Type", "application/json");

    static async::TaskHolder<web::WebResponse> holder;
    holder.spawn(
        req.get(fmt::format("{}{}.json", list->m_settings->endpoint, "_list")),
        [&, shouldLoadLevels, page, list](web::WebResponse e) {
            auto res = e.json();
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
        }
    );
}

void Request::loadPageLevels(int page, List* list) {
    std::vector<std::string> names = list->getLevelNames();
    int levelsPerPage = list->m_settings->levelsPerPage;

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

        static async::TaskHolder<web::WebResponse> holder;
        holder.spawn(
            req.get(fmt::format("{}{}.json", list->m_settings->endpoint, name)),
            [&, i, page, list](web::WebResponse e) {
                auto res = e.json();
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
            }
        );
    }
}

std::string Request::getLevelsString(int page, List* list) {
    std::string str;
    int levelsPerPage = list->m_settings->levelsPerPage;
    log::info("getLevelsString called. Page: {}", page);

    log::info("list->getLevelNames() size: {}", list->getLevelNames().size());

    for (int i = page * levelsPerPage; i <= page * levelsPerPage + levelsPerPage - 1; i++) {
        log::info("Processing index: {}", i);
        
        if (i >= list->getLevelNames().size()) {
            log::warn("Index {} is out of range. Skipping.", i);
            continue;
        }

        std::string name = list->getLevelNames()[i];
        log::info("Level name at index {}: {}", i, name);

        int id = list->getLevelId(i);
        log::info("Level ID at index {}: {}", i, id);

        if (id == 0) {
            log::error("Failed to find level ID for index {}. ID is 0. Returning empty string.", i);
            return "";
        }

        str += std::to_string(id) + (i == page * levelsPerPage + levelsPerPage - 1 ? "" : ",");
        log::info("Current result string: {}", str);
    }

    log::info("Returning final string: {}", str);
    return str;
}

void Request::replace(std::string& str, char c, std::string r) {
    size_t pos = 0;
    while ((pos = str.find(c, pos)) != std::string::npos) {
        str.replace(pos, 1, r);
        pos += 3;
    }
}
