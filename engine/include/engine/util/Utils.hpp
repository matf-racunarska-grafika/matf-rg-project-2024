
#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <format>
#include <source_location>
#include <vector>
#include <mutex>
#include <filesystem>

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

#define STR(a) #a
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define defer auto CONCAT(defer_stmt_, __LINE__) = MakeDeferImpl() << [&]

#define range(container) std::begin(container), std::end(container)
#define crange(container) std::cbegin(container), std::cend(container)

namespace engine::util {
    void tracing_on();

    void tracing_off();

    void trace(std::source_location location = std::source_location::current());

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
    }
} // namespace engine

#endif//MATF_RG_PROJECT_UTILS_HPP
