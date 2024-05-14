#include "engine/engine.hpp"

/**
 * Student implements rg::App for their application.
 */
class StudentsApp : public rg::App {
protected:
    void initialize() override {
    }

    bool loop() override {
        auto input = rg::utils::ServiceProvider::get<rg::platform::InputController>();
        if (input->key(rg::platform::KEY_ESCAPE).pressed()) {
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
}

int main() {
    return rg::App::run();
}