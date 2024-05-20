//
// Created by spaske00 on 13.5.24..
//

#include <fstream>
#include <utility>
#include "engine/utils.hpp"
#include "spdlog/spdlog.h"

namespace rg {
    void guarantee(bool expr, std::string msg, std::source_location source_location) {
        if (!expr) {
            throw GuaranteeViolation(std::move(msg), source_location);
        }
    }

    void should_not_reach_here(std::string msg, std::source_location source_location) {
        throw ShouldNotReachHere(std::move(msg), source_location);
    }

    void unimplemented(std::string msg, std::source_location source_location) {
        throw Unimplemented(std::move(msg), source_location);
    }

    std::string Unimplemented::report() const {
        return std::format("Unimplemented: {}:{}. {}. This error should be used only as a placeholder. Ensure that the "
                           "code path is implemented or use ShouldNotReachHere.", location().file_name(),
                           location().line(), message());
    }

    std::string ShouldNotReachHere::report() const {
        return std::format("ShouldNotReach: {}:{}. {}. There is a logic error in the program. The execution should "
                           "never reach this point.", location().file_name(), location().line(), message());
    }

    std::string GuaranteeViolation::report() const {
        return std::format("GuaranteeViolation in: {}:{}. {}. There is a logic error in the program. Please ensure "
                           "that the Guarantee is never violated.", location().file_name(), location().line(),
                           message());
    }

    std::string FileNotFoundError::report() const {
        return std::format(
                "FileNotFound {}:{}. {}. The file: \"{}\" is necessary for program's execution, please ensure that the file is available.",
                location().file_name(), location().line(), message(), file_path());
    }

    std::string_view FileNotFoundError::file_path() const {
        return m_path;
    }


    Configuration *Configuration::instance() {
        static Configuration configuration;
        return &configuration;
    }

    const Configuration::json &Configuration::config() const {
        return m_config;
    }

    void Configuration::initialize() {
        std::ifstream f("config.json");
        if (!f.is_open()) {
            throw rg::FileNotFoundError("config.json", "Failed to load configuration file.");
        }
        // Handle parsing exception
        try {
            m_config = json::parse(f);
        } catch (const std::exception &e) {
            std::string message(e.what());
            throw rg::ConfigurationError(std::format(
                    "Error \"{}\" occurred while parsing the configuration file. Please make sure that the file is in the correct json format.",
                    message));
        }
        spdlog::info("Configuration::initialize");
    }

    std::string ConfigurationError::report() const {
        return std::format("ConfigurationError {}:{}. {}.", location().file_name(), location().line(), message());
    }
}// namespace rg
