// include/app/MyApp.hpp
#ifndef MYAPP_HPP
#define MYAPP_HPP

#include <engine/core/Engine.hpp>

namespace engine::myapp {
class MyApp final : public engine::core::App {
    void app_setup() override;
};
}

#endif // MYAPP_HPP
