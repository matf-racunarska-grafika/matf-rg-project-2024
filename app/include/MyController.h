//
// Created by nikola on 10/8/25.
//

#ifndef MATF_RG_PROJECT_MYCONTROLLER_H
#define MATF_RG_PROJECT_MYCONTROLLER_H
#include "engine/core/Controller.hpp"

namespace app {

class MyController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
public:
    std::string_view name() const override {
        return "app::MyController";
    }

};

}// namespace app

#endif//MATF_RG_PROJECT_MYCONTROLLER_H
