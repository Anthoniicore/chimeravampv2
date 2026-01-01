// SPDX-License-Identifier: GPL-3.0-only

#include "bullet_magnetism_fix.hpp"
#include "../signature/hook.hpp"
#include "../signature/signature.hpp"
#include "../halo_data/table.hpp"
#include "../halo_data/object.hpp"
#include "../command/command.hpp"
#include "../output/output.hpp"
#include "../chimera.hpp"
#include <cmath>

namespace Chimera {
    static bool bullet_magnetism_enabled = false;
    static float magnetism_angle = 45.0f;

    inline void normalize_point(Point3D &p) {
        float len = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
        if(len > 0) {
            p.x /= len;
            p.y /= len;
            p.z /= len;
        }
    }

    inline float dot_product(const Point3D &a, const Point3D &b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static void apply_bullet_magnetism(uint32_t shooter_id, Point3D *bullet_dir) noexcept {
        if(!bullet_magnetism_enabled) return;

        auto &ot = ObjectTable::get_object_table();
        auto *shooter = ot.get_dynamic_object(shooter_id);
        if(!shooter) return;

        float best_dot = std::cos(magnetism_angle * (3.14159f / 180.0f));
        Point3D best_dir = *bullet_dir;

        for(std::size_t i = 0; i < ot.current_size; i++) {
            auto *obj = ot.get_dynamic_object(i);
            if(!obj) continue;
            if(obj->full_object_id().whole_id == shooter_id) continue;

            Point3D to_target {
                obj->position.x - shooter->position.x,
                obj->position.y - shooter->position.y,
                obj->position.z - shooter->position.z
            };
            normalize_point(to_target);

            float dot = dot_product(*bullet_dir, to_target);
            if(dot > best_dot) {
                best_dot = dot;
                best_dir = to_target;
            }
        }

        *bullet_dir = best_dir;
    }

    void bullet_magnetism_fix() noexcept {
        static Hook bullet_hook;
        auto *bullet_addr = get_chimera().get_signature("bullet_magnetism_sig").data();
        write_function_override(
            bullet_addr,
            bullet_hook,
            reinterpret_cast<const void *>(apply_bullet_magnetism),
            nullptr
        );
        bullet_magnetism_enabled = true;
    }

    bool bullet_magnetism_command(int argc, const char **argv) noexcept {
        if(argc == 1) {
            bool new_value = STR_TO_BOOL(argv[0]);
            if(new_value != bullet_magnetism_enabled) {
                if(new_value) {
                    bullet_magnetism_fix();
                } else {
                    static Hook bullet_hook;
                    bullet_hook.rollback();
                }
                bullet_magnetism_enabled = new_value;
            }
        }
        console_output(BOOL_TO_STR(bullet_magnetism_enabled));
        return true;
    }
} // namespace Chimera
