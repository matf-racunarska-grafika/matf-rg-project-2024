//
// Created by filip on 1/22/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

}



#endif //MAINCONTROLLER_HPP
