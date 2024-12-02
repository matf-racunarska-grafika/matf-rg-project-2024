//
// Created by spaske00 on 13.5.24..
//

#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <format>
#include <memory>
#include <source_location>
#include <vector>
#include <queue>
#include <mutex>
#include <json.hpp>
#include <variant>
#include <engine/util/Errors.hpp>

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename Func>
struct DeferImpl {
    DeferImpl(Func f) : f(f) {
    }

    ~DeferImpl() {
        f();
    }

    Func f;
};

struct MakeDeferImpl {
};

template<typename Func>
DeferImpl<Func> operator<<(MakeDeferImpl, Func f) {
    return DeferImpl(f);
}

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define defer auto CONCAT(defer_stmt_, __LINE__) = MakeDeferImpl() << [&]

#define range(container) std::begin(container), std::end(container)
#define crange(container) std::cbegin(container), std::cend(container)

namespace rg {
    template<typename Value, typename Error>
    class Result {
    public:
        Result(const Value &value) : m_variant(value) {
        }

        Result(const Error &error) : m_variant(error) {
        }

        bool has_value() const {
            return std::holds_alternative<Value>(m_variant);
        };

        bool has_error() const {
            return !has_value();
        }

        Value &value(std::source_location location = std::source_location::current()) {
            RG_GUARANTEE(has_value(), "Trying to extract value from Result while it holds an error at {}:{}.",
                         location.file_name(), location.line());
            return std::get<Value>(m_variant);
        }

        Error error() {
            RG_GUARANTEE(!has_value(), "Trying to extract error from Result while it holds a value. ");
            return std::get<Error>(m_variant);
        }

        const Value &value() const {
            RG_GUARANTEE(has_value(), "Trying to extract value from Result while it holds an error.");
            return std::get<Value>(m_variant);
        }

        const Error &error() const {
            RG_GUARANTEE(!has_value(), "Trying to extract error from Result while it holds a value. ");
            return std::get<Error>(m_variant);
        }

        Value *try_value() {
            return std::get_if<Value>(&m_variant);
        }

        Error *try_error() {
            return std::get_if<Error>(&m_variant);
        }

        const Value *try_value() const {
            return std::get_if<Value>(&m_variant);
        }

        const Error *try_error() const {
            return std::get_if<Error>(&m_variant);
        }

    private:
        std::variant<Value, Error> m_variant;
    };

    class Configuration {
        friend class App;

    public:
        using json = nlohmann::json;

        static json &config();

        static Configuration *instance();

        void initialize();

    private:
        static std::filesystem::path get_config_path();

        static std::filesystem::path create_default();

        constexpr static std::string_view CONFIG_FILE_NAME = "config.json";

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

        void initialize(int argc, char **argv);

    private:
        std::string get_arg_value(std::string_view arg_name);

        ArgParser() = default;

        int m_argc    = 0;
        char **m_argv = nullptr;
    };

    std::string read_text_file(const std::filesystem::path &path);

    template<typename Action>
    void once(Action action) {
        static std::once_flag once;
        std::call_once(once, action);
    };

    namespace alg {
        template<typename Container, typename T>
        bool contains(const Container &container, const T &value) {
            return std::find(std::cbegin(container), std::cend(container), value) != std::cend(container);
        }
    } // namespace alg

    namespace ds {
    }} // namespace rg

#endif//MATF_RG_PROJECT_UTILS_HPP
