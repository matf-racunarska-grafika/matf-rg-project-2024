#include <app/GUIController.hpp>
#include <app/Main.hpp>
#include <app/MainController.hpp>

namespace engine::app {
void Main::app_setup() {
    auto main_controller = register_controller<MainController>();
    auto gui_controller = register_controller<GUIController>();
    main_controller->after(core::Controller::get<core::EngineControllersEnd>());
    gui_controller->after(main_controller);
}
}

int main(int argc, char **argv) {
    return std::make_unique<engine::app::Main>()->run(argc, argv);
}
