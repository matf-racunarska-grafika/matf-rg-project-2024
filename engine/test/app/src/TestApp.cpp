#include <app/GUIController.hpp>
#include <app/TestApp.hpp>
#include <app/MainController.hpp>

namespace engine::test::app {
    void TestApp::app_setup() {
        auto main_controller = engine::controller::register_controller<MainController>();
        auto gui_controller  = engine::controller::register_controller<GUIController>();
        main_controller->after(engine::controller::get<controller::EngineControllersEnd>());
        gui_controller->after(main_controller);
    }
}

int main(int argc, char **argv) {
    return std::make_unique<engine::test::app::TestApp>()->run(argc, argv);
}
