#pragma once
#include <vector>
#include <string>
#include <functional>

namespace engine::core {
    struct ScheduledEvent {
        float trigger_time;
        std::string event_name;
    };

    class EventQueue {
    public:
        /// Закаже догађај у апсолутном тренутку
        void schedule(float trigger_time, const std::string &event_name) {
            m_events.push_back({trigger_time, event_name});
        }

        /// Закаже догађај након одложења од 'delay' од current_time
        void schedule_after(float current_time, float delay, const std::string &event_name) {
            schedule(current_time + delay, event_name);
        }

        /// Сваки фрејм: испоручи све догађаје чији је trigger_time <= current_time
        void update(float current_time,
                    const std::function<void(const std::string &)> &executor) {
            // сортирај по времену (опционо, за гарантован редослед)
            std::sort(m_events.begin(), m_events.end(),
                      [](auto &a, auto &b) {
                          return a.trigger_time < b.trigger_time;
                      });

            // испоручи и уклони све с доспелим trigger_time
            size_t idx = 0;
            while (idx < m_events.size() && m_events[idx].trigger_time <= current_time) {
                executor(m_events[idx].event_name);
                ++idx;
            }
            m_events.erase(m_events.begin(), m_events.begin() + idx);
        }

        void clear() {
            m_events.clear();
        }

    private:
        std::vector<ScheduledEvent> m_events;
    };
} // namespace engine::core
