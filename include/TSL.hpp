#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tsl {
    struct ListSettings {
        int levelsPerPage = 10;
        bool classic = true;
        bool platformer = false;
        bool weekly = false;
    };

    class List {
    private:
        std::string m_name;
        std::string m_endpoint;
        ListSettings m_settings;

    public:
        static List* create(std::string name, std::string endpoint, ListSettings settings) {
            List* list = new List();
            list->m_name = name;
            list->m_endpoint = endpoint;
            list->m_settings = settings;
            return list;
        }
    };
}
