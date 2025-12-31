// SPDX-License-Identifier: GPL-3.0-only

#include "../../command.hpp"
#include "../../../signature/signature.hpp"
#include "../../../chimera.hpp"
#include "../../../output/output.hpp"

namespace Chimera {
    bool aim_assist_command(int argc, const char **argv) {
        static auto &active = **reinterpret_cast<char **>(
            get_chimera().get_signature("aim_assist_enabled_sig").data() + 1
        );

        // Fuerza siempre a true
        active = true;

        // Imprime el estado (siempre "true")
        console_output("true");
        return true;
    }
}
