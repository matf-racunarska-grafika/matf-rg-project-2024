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
#include <variant>
#include <rg/util/Errors.hpp>

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

namespace rg::util {
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
