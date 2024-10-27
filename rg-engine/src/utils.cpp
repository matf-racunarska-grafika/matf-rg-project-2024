//
// Created by spaske00 on 13.5.24..
//

#include <fstream>
#include "engine/utils.hpp"
#include "spdlog/spdlog.h"
#include "engine/errors.hpp"

namespace rg {


    void Configuration::initialize() {
        auto config_path = ArgParser::instance()->arg<std::string>("--configuration");
        RG_GUARANTEE(config_path.has_value(), "No configuration file provided. Please provide path to the engine config file using --configuration command line option.");
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

    Configuration *Configuration::instance() {
        static Configuration configuration;
        return &configuration;
    }

    Configuration::json &Configuration::config() {
        return Configuration::instance()->m_config;
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
                RG_GUARANTEE(!arg_value.starts_with("--"), "No get_arg_value for argument: \"{}\" provided.", arg_name);
                return arg_value;
            }
        }
        return "";
    }
}// namespace rg
