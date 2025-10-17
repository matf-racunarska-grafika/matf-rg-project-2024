#include <engine/core/Engine.hpp>
#include <memory>
#include <spdlog/spdlog.h>

class MyApp final : public engine::core::App {
protected:
    void app_setup() override {
        spdlog::info("\x1b[31mWorking example\x1b[0m");
    }
};


int main(int argc, char **argv) {
    auto app = std::make_unique<MyApp>();
    return app->run(argc, argv);
}
