
#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include <engine/core/Engine.hpp>

namespace engine::test::app {
class TestApp final : public engine::core::App {
    void app_setup() override;
};
}// namespace engine::test::app
#endif//MAINAPP_HPP
