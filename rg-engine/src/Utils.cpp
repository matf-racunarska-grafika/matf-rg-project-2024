//
// Created by spaske00 on 13.5.24..
//

#include "engine/Utils.h"
#include <spdlog/spdlog.h>

namespace rg::utils {
    void EngineError::guarantee(bool expr, std::string_view msg, std::source_location source_location) {
        if (!expr) {
           spdlog::error("RG_GUARANTEE failed in file:{:}:{:}. {:}", source_location.file_name(), source_location.line(), msg);
           exit(EXIT_FAILURE);
        }
    }

    void EngineError::should_not_reach_here(std::string_view msg, std::source_location source_location) {
        spdlog::error("RG_SHOULD_NOT_REACH_HERE in file:{:}:{:}. {:}", source_location.file_name(), source_location.line(), msg);
        exit(EXIT_FAILURE);
    }

    void EngineError::unimplemented(std::string_view msg, std::source_location source_location) {
        spdlog::error("RG_UNIMPLEMENTED in file:{:}:{:}. {:}", source_location.file_name(), source_location.line(), msg);
        exit(EXIT_FAILURE);
    }
}