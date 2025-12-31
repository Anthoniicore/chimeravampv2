// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "../command/command.hpp" 
#include "../chimera.hpp"

// Inicializa el fix de bullet magnetism
void bullet_magnetism_fix() noexcept;

// Comando de consola para activar/desactivar
ChimeraCommandError bullet_magnetism_command(size_t argc, const char **argv) noexcept;

