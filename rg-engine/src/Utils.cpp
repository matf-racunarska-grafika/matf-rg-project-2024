//
// Created by spaske00 on 13.5.24..
//

#include "engine/Utils.hpp"
#include <spdlog/spdlog.h>

namespace rg {
    void error::guarantee(bool expr, std::string_view msg, std::source_location source_location) {
        if (!expr) {
            throw GuaranteeViolation(msg, source_location);
        }
    }

    void error::should_not_reach_here(std::string_view msg, std::source_location source_location) {
        throw ShouldNotReachHere(msg, source_location);
    }

    void error::unimplemented(std::string_view msg, std::source_location source_location) {
        throw Unimplemented(msg, source_location);
    }

    std::string error::Unimplemented::report() const {
        return std::format(
                "Unimplemented: {}:{}. {}. This error should be used only as a placeholder. Ensure that the code path is implemented or use error::ShouldNotReachHere.",
                location().file_name(), location().line(), message());
    }

    std::string error::ShouldNotReachHere::report() const {
        return std::format(
                "ShouldNotReach: {}:{}. {}. There is a logic error in the program. The execution should never reach this point.",
                location().file_name(), location().line(), message());
    }

    std::string error::GuaranteeViolation::report() const {
        return std::format(
                "GuaranteeViolation in: {}:{}. {}. There is a logic error in the program. Please ensure that the Guarantee is never violated.",
                location().file_name(), location().line(), message());
    }
}