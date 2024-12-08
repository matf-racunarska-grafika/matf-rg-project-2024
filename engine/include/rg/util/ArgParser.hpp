//
// Created by spaske on 12/7/24.
//

#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP
#include <string_view>
#include <optional>
#include <string>

namespace rg::util {
    class ArgParser {
        friend class App;

    public:
        static ArgParser *instance();

        template<typename T>
        std::optional<T> arg(std::string_view name, std::optional<T> default_value = {}) {
            std::string arg_value = get_arg_value(name);
            if (arg_value.empty()) {
                return default_value.has_value() ? default_value.value() : T{};
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

        void initialize(int argc, char **argv);

    private:
        std::string get_arg_value(std::string_view arg_name);

        ArgParser() = default;

        int m_argc    = 0;
        char **m_argv = nullptr;
    };
}
#endif //ARGPARSER_HPP
