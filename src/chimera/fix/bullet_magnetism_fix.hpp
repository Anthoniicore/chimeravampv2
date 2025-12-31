// bullet_magnetism_fix.hpp
#pragma once

#include "../command/command.hpp"
#include "../chimera.hpp"

namespace Chimera {
    bool bullet_magnetism_command(int argc, const char **argv) noexcept;
    void bullet_magnetism_fix();
}
