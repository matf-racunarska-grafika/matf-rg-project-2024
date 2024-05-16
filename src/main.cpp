#include "engine/App.hpp"
#include "spdlog/spdlog.h"

/**
 * Student implements rg::App for their application.
 */
class StudentsApp : public rg::App {
protected:
    void initialize() override {
        spdlog::info("StudentsApp::initialize");
        m_max_loop_count = 3;
    }

    bool loop() override {
        spdlog::info("StudentsApp::loop");
        return m_loop_count < m_max_loop_count;
    }

    void update() override {
        spdlog::info("StudentsApp::update");
        ++m_loop_count;
    }

    void draw() override {
        spdlog::info("StudentsApp::draw");
        spdlog::info("{:}/{:}", (m_loop_count), m_max_loop_count);
    }

    void terminate() override {
        spdlog::info("StudentsApp::terminate");
        m_loop_count = -1;
    }

private:
    int m_loop_count = 0;
    int m_max_loop_count = 0;
};


namespace rg {
    std::unique_ptr<App> create_app() {
        return std::make_unique<StudentsApp>();
    }
}

int main() {
    return rg::App::run();
}