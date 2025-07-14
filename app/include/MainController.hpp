//
// Created by ognje on 7/14/2025.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/core/Controller.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

};// app

#endif //MAINCONTROLLER_HPP
