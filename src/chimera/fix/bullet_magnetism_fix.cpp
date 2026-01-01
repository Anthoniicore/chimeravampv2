// SPDX-License-Identifier: GPL-3.0-only

#include "bullet_magnetism_fix.hpp"
#include "../signature/signature.hpp"
#include "../halo_data/table.h"
#include "../halo_data/object.hpp"
#include "../hooks/tick.h"
#include "../command/command.hpp"
#include "../output/output.hpp"

namespace Chimera {
    static bool bullet_magnetism_enabled = false;
    static float magnetism_angle = 45.0f; // grados de tolerancia

    // Hook: ajusta la dirección de la bala
    static void apply_bullet_magnetism(uint32_t shooter_id, Vector3D *bullet_dir) noexcept {
        if(!bullet_magnetism_enabled) return;

        auto &ot = ObjectTable::get_object_table();
        auto *shooter = ot.get_dynamic_object(shooter_id);
        if(!shooter) return;

        float best_dot = cosf(magnetism_angle * (3.14159f / 180.0f));
        Vector3D best_dir = *bullet_dir;

        for(auto &obj : ot.objects) {
            if(!obj.is_valid()) continue;
            if(obj.object_id == shooter_id) continue;

            Vector3D to_target = obj.position - shooter->position;
            normalize_vector(&to_target);

            float dot = dot_product(*bullet_dir, to_target);
            if(dot > best_dot) {
                best_dot = dot;
                best_dir = to_target;
            }
        }

        *bullet_dir = best_dir;
    }

    // Activa el fix
    void bullet_magnetism_fix() noexcept {
        auto *addr = get_signature("bullet_magnetism_sig").address();

        // Crear codecave que llama a apply_bullet_magnetism
        static BasicCodecave bullet_code(apply_bullet_magnetism);

        DWORD old_protect;
        VirtualProtect(addr, 6, PAGE_READWRITE, &old_protect);
        memset(addr, 0x90, 6);
        addr[0] = 0xE8;
        *I32PTR(addr + 1) = I32(bullet_code.data) - I32(addr + 1 + 4);
        VirtualProtect(addr, 6, old_protect, &old_protect);

        bullet_magnetism_enabled = true;
    }

    // Comando de consola
    bool bullet_magnetism_command(int argc, const char **argv) noexcept {
        if(argc == 1) {
            bool new_value = STR_TO_BOOL(argv[0]);
            if(new_value != bullet_magnetism_enabled) {
                if(new_value) {
                    bullet_magnetism_fix();
                } else {
                    get_signature("bullet_magnetism_sig").undo();
                }
                bullet_magnetism_enabled = new_value;
            }
        }
        console_output(BOOL_TO_STR(bullet_magnetism_enabled));
        return true;
    }
}


