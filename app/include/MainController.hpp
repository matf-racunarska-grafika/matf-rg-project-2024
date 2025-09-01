//
// Created by filipn on 9/1/25.
//

#ifndef MAINCONTROLER_HPP
#define MAINCONTROLER_HPP
#include <engine/core/Controller.hpp>

namespace app {
class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

public:
    std::string_view name() const override { return "MainController"; }
};
}
#endif //MAINCONTROLER_HPP
