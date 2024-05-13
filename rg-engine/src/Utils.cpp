//
// Created by oracle on 13.5.24..
//

#include "engine/Utils.h"
#include <spdlog/spdlog.h>

namespace rg::utils {
    void EngineError::guarantee(bool expr, std::string_view expr_str, std::string_view msg, std::string_view file,
                                int line) {
        if (!expr) {
           spdlog::error("RG_GUARANTEE ({:}) failed in file:{:}:{:}. {:}", expr_str, file, line, msg);
           exit(EXIT_FAILURE);
        }
    }

    void EngineError::should_not_reach_here(std::string_view msg, std::string_view file, int line) {
        spdlog::error("RG_SHOULD_NOT_REACH_HERE in file:{:}:{:}. {:}", file, line, msg);
        exit(EXIT_FAILURE);
    }

    void EngineError::unimplemented(std::string_view msg, std::string_view file, int line) {
        spdlog::error("RG_UNIMPLEMENTED in file:{:}:{:}. {:}", file, line, msg);
        exit(EXIT_FAILURE);
    }
}