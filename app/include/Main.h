#ifndef MATF_RG_PROJECT_MAIN_H
#define MATF_RG_PROJECT_MAIN_H

#include <engine/core/App.hpp>
#include <spdlog/spdlog.h>


namespace app {
    class Main : public engine::core::App {
        public:
        void app_setup() override;
    };
}

#endif