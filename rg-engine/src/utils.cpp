//
// Created by spaske00 on 13.5.24..
//

#include <fstream>
#include <utility>
#include "engine/utils.hpp"
#include "spdlog/spdlog.h"
#include "engine/errors.hpp"

namespace rg {

    const Configuration::json &Configuration::config() const {
        return m_config;
    }

    void Configuration::initialize() {
        auto config_path = ArgParser::instance()->arg<std::string>("--configuration");
        rg::guarantee(config_path.has_value(),
                      "No configuration file provided. Please provide path to the engine config file using --configuration command line option.");
        std::ifstream f(config_path.value());
        if (!f.is_open()) {
            throw rg::FileNotFoundError(config_path.value(), "Failed to load configuration file.");
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
    }

    std::unique_ptr<Configuration> Configuration::create() {
        return std::make_unique<Configuration>();
    }

    std::string_view Configuration::name() const {
        return "Configuration";
    }

    std::string ConfigurationError::report() const {
        return std::format("ConfigurationError {}:{}. {}.", location().file_name(), location().line(), message());
    }

    ArgParser *ArgParser::instance() {
        static ArgParser arg_parser;
        return &arg_parser;
    }

    void ArgParser::initialize(int argc, char **argv) {
        m_argc = argc;
        m_argv = argv;
    }

    std::string ArgParser::get_arg_value(std::string_view arg_name) {
        for (int i = 0; i < m_argc; ++i) {
            std::string_view token(m_argv[i]);
            if (token == arg_name) {
                std::string arg_value(m_argv[i + 1]);
                rg::guarantee(!arg_value.starts_with("--"),
                              std::format("No get_arg_value for argument: \"{}\" provided.", arg_name));
                return arg_value;
            }
        }
        return "";
    }
}// namespace rg
