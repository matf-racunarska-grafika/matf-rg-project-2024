
#ifndef MAINAPP_HPP
#define MAINAPP_HPP
#include <engine/Engine.hpp>

namespace engine::test::app {
    class MainApp final : public engine::core::App {
        void setup() override;
    };
}
#endif //MAINAPP_HPP
