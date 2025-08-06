#pragma once

#include <Geode/Geode.hpp>

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
        std::string endpoint = "https://raw.githubusercontent.com/ExampleName/ExampleRepo/refs/heads/main/data/";
        int levelsPerPage = 10;
        bool classic = true;
        bool platformer = false;
        bool weekly = false;
        bool staffInfo = true;
        StaffTeam* staff = StaffTeam::create();
    };

    class List {
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
    };

    class Cache {
        
    };
}
