//
// Created by strahinjas on 3/23/25.
//

#include <iostream>
#include <chrono>
#include <thread>

void trigger_event() { std::cout << "Event triggered!" << std::endl; }

class Timer {
public:
    explicit Timer(float delay_seconds) : delay_time(delay_seconds)
                                      , start_time(std::chrono::steady_clock::now()) {}

    bool has_expired() const {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::duration<float> >(now - start_time).count();
        return elapsed >= delay_time;
    }

private:
    float delay_time;
    std::chrono::steady_clock::time_point start_time;
};

int main(int argc, char *argv[]) {
    Timer timer(3.0f);
    int i = 0;
    while (true) {
        if (timer.has_expired()) {
            trigger_event();
            break;
        }
        std::cout << "Waiting until end of delay..." << std::endl;
        std::cout << "Iteration: " << i++ << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
