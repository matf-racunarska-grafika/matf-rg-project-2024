#include <app/MyApp.hpp>
#include <app/MainController.hpp>
#include <app/GUIController.hpp>

namespace engine::myapp {
void MyApp::app_setup() {
    auto main_controller = register_controller<myapp::MainController>();
    auto gui_controller  = register_controller<myapp::GUIController>();

    main_controller->after(core::Controller::get<core::EngineControllersEnd>());
    gui_controller ->after(main_controller);
}
}

int main(int argc, char **argv) {
    return std::make_unique<engine::myapp::MyApp>()->run(argc, argv);
}
