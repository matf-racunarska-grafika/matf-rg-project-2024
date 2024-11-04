#include <engine/render/AssetsController.hpp>
#include <spdlog/spdlog.h>

namespace rg {

    void AssetsController::initialize() {
        const auto &config = Configuration::config();
        spdlog::info(to_string(config["assets"]["models"]));
    }

    void AssetsController::terminate() {
    }
}// namespace rg