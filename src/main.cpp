#include "engine/engine.hpp"

/**
 * Student implements rg::App for their application.
 */
class StudentsApp : public rg::App {
protected:
    void initialize() override {
    }

    bool loop() override {
        const auto input = rg::ControllerManager::get<rg::InputController>();
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
    }

private:
};

namespace rg {
    std::unique_ptr<App> create_app() {
        return std::make_unique<StudentsApp>();
    }
}// namespace rg

int main() {
    return rg::App::run();
}