//
// Created by bojana on 10/8/25.
//

#ifndef MATF_RG_PROJECT_MYCONTROLLER_HPP
#define MATF_RG_PROJECT_MYCONTROLLER_HPP
#include <engine/core/Controller.hpp>

namespace app {

class MyController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
public:
    std::string_view name() const override {
        return "MyController";
    }
};

}// namespace app

#endif//MATF_RG_PROJECT_MYCONTROLLER_HPP
