//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <format>
#include <memory>
#include <source_location>
#include <vector>

#include <json.hpp>

#include "core.hpp"


namespace rg {

    class Configuration {
        friend class App;
    public:
        using json = nlohmann::json;

        static json &config();

        static Configuration *instance();

    private:
        void initialize();

        json m_config;
    };

    class ArgParser {
        friend class App;

    public:
        static ArgParser *instance();

        template<typename T>
        std::optional<T> arg(std::string_view name) {
            std::string arg_value = get_arg_value(name);
            if (arg_value.empty()) {
                return {};
            }
            std::size_t parsed = 0;
            if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, int32_t>) {
                return std::stoi(arg_value, &parsed);
            } else if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, int64_t>) {
                return std::stoll(arg_value, &parsed);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::stof(arg_value, &parsed);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::stod(arg_value, &parsed);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg_value;
            } else {
                static_assert(false, "This type is not supported!");
            }
        }

    private:
        void initialize(int argc, char **argv);

        std::string get_arg_value(std::string_view arg_name);

        ArgParser() = default;

        int m_argc = 0;
        char **m_argv = nullptr;
    };

    namespace alg {
        template<typename Container, typename T>
        bool contains(const Container &container, const T &value) {
            return std::find(std::cbegin(container), std::cend(container), value) != std::cend(container);
        }
    }// namespace alg

    namespace ds {}

}// namespace rg

#endif//MATF_RG_PROJECT_UTILS_HPP
