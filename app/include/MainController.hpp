//
// Created by matfrg on 11/1/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <MyApp.hpp>
#include <engine/core/Controller.hpp>

namespace my_project {
class MyController final : public engine::core::Controller {
public:
    [[nodiscard]] std::string_view name() const override { return typeid(*this).name(); };

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

};
}

#endif //MAINCONTROLLER_HPP
