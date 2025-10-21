#include <engine/core/App.hpp>
#include <engine/core/Controller.hpp>
#include <spdlog/spdlog.h>

#include "MainController.hpp"

namespace app {

class MyApp : public engine::core::App {
    void app_setup() override {
        spdlog::info("App setup...");
        auto mainc = register_controller<MainController>();

        mainc->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
};

}// namespace app

int main(int argc, char **argv) {
    app::MyApp app;
    return app.run(argc, argv);
}
