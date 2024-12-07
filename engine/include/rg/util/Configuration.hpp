//
// Created by spaske on 12/7/24.
//

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
#include <json.hpp>
namespace rg::util {
 class Configuration {
        friend class App;

    public:
        using json = nlohmann::json;

        static json &config();

        static Configuration *instance();

        void initialize();

    private:
        static std::filesystem::path get_config_path();

        static json create_default();

        constexpr static std::string_view CONFIG_FILE_NAME = "config.json";

        json m_config;
    };
}
#endif //CONFIGURATION_HPP
