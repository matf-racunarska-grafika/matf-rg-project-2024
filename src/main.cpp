#include "engine/controller/Controller.hpp"
#include "engine/ecs/EntityController.hpp"
#include <engine/Engine.hpp>
#include <memory>
#include <spdlog/spdlog.h>
/**
 * Student implements rg::App for their application.
 */

class Controller1 : public rg::Controller {
public:
    std::string_view name() const override {
        return "Controller1";
    }
    static std::unique_ptr<Controller1> create() {
        return std::make_unique<Controller1>();
    }

private:
    void initialize() override {
        spdlog::trace("Controller1");
    }
};
class Controller2 : public rg::Controller {
public:
    std::string_view name() const override {
        return "Controller2";
    }
    static std::unique_ptr<Controller2> create() {
        return std::make_unique<Controller2>();
    }

private:
    void initialize() override {
        spdlog::trace("Controller2");
    }
};

class Controller3 : public rg::Controller {
public:
    std::string_view name() const override {
        return "Controller3";
    }
    static std::unique_ptr<Controller3> create() {
        return std::make_unique<Controller3>();
    }

private:
    void initialize() override {
        spdlog::trace("Controller3");
    }
};

class StudentsApp : public rg::App {
protected:
    void initialize() override {
        auto manager = rg::ControllerManager::singleton();
        auto c2 = manager->register_controller<Controller2>();
        auto c3 = manager->register_controller<Controller3>();
        auto c1 = manager->register_controller<Controller1>();
        auto entity = manager->get<rg::EntityController>();
        c1->before(c3);
        c2->before(c3);
        c2->before(c1);
    }

    void after_initialize() override {
        auto entity_controller = rg::ControllerManager::singleton()->get<rg::EntityController>();
        m_backpack = entity_controller->create_entity();
        auto *model = m_backpack->add_component<rg::Model>();
    }

    bool loop() override {
        const auto input = rg::ControllerManager::singleton()->get<rg::PlatformController>();
        if (input->key(rg::KeyId::KEY_ESCAPE).state() == rg::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void update() override {
    }

    void draw() override {
    }

    void terminate() override {
        m_backpack->destroy();
    }

private:
    rg::Entity *m_backpack;
};

namespace rg {
    std::unique_ptr<App> create_app() {
        return std::make_unique<StudentsApp>();
    }
}// namespace rg

int main(int argc, char **argv) {
    return rg::App::run(argc, argv);
}