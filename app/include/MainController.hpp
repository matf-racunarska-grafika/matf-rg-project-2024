#pragma once
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
public:
    std::string_view name() const override { return "MainController"; }

    void initialize() override;

    bool loop() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

private:
    void update_camera();
};

}// namespace app
