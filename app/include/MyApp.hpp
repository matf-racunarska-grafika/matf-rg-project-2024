//
// Created by strahinjas on 3/18/25.
//

#ifndef MYAPP_HPP
#define MYAPP_HPP

#include <engine/core/App.hpp>

namespace my_project {

class MyApp final : public engine::core::App {
public:
    void app_setup() override;
};
}

#endif //MYAPP_HPP
