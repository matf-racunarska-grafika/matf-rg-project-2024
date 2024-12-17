
#ifndef MATF_RG_PROJECT_UTILS_HPP
#define MATF_RG_PROJECT_UTILS_HPP

#include <format>
#include <source_location>
#include <vector>
#include <mutex>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <type_traits>

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

        template<typename It, typename Adjacent>
        void topological_sort(It first, It last, Adjacent adjacent) {
            using ElementType = std::remove_reference_t<decltype(*first)>;
            std::unordered_set<ElementType> visited;
            std::vector<ElementType> stack;

            std::function<void(ElementType &)> visit = [&](ElementType &current) mutable -> void {
                visited.emplace(current);
                for (auto next: adjacent(current)) {
                    if (!visited.contains(next)) {
                        visit(next);
                    }
                }
                stack.push_back(current);
            };

            auto it = first;
            while (it != last) {
                if (!visited.contains(*it)) {
                    visit(*it);
                }
                ++it;
            }
            std::move(stack.rbegin(), stack.rend(), first);
        }

        template<typename It, typename Adjacent, typename OutputIt = std::nullptr_t>
        bool has_cycle(It first, It last, Adjacent adjacent, OutputIt cycle_output = nullptr) {
            using ElementType = std::remove_reference_t<decltype(*first)>;
            std::unordered_set<ElementType> visited;
            std::unordered_set<ElementType> path;
            std::function<bool(ElementType &)> visit = [&](ElementType &current) -> bool {
                visited.emplace(current);
                path.emplace(current);
                for (ElementType &next: adjacent(current)) {
                    if (!visited.contains(next) && visit(next)) {
                        return true;
                    }
                    if (path.contains(next)) {
                        return true;
                    }
                }
                path.erase(current);
                return false;
            };

            for (auto root = first; root != last; ++root) {
                if (!visited.contains(*root) && visit(*root)) {
                    if constexpr (!std::is_same_v<OutputIt, std::nullptr_t>) {
                        std::move(path.begin(), path.end(), cycle_output);
                    };
                    return true;
                }
            }
            return false;
        }
    } // namespace alg

    namespace ds {
    }
} // namespace engine

#endif//MATF_RG_PROJECT_UTILS_HPP
