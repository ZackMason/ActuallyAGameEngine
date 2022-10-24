// Author: Zackery Mason-Blaug
// Date: 2022-10-24
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include "Util/color.hpp"

#include "Engine/resource_handle.hpp"

#include "Graphics/texture2d.hpp"

#include "UI/theme.hpp"

namespace zui {
    struct widget_t {
        sid_t id{};
        sid_t text{};

        v2f padding{};
        v2f margin{};

        resource_handle_t<texture2d_t> texture;

        enum struct type_e {
            BUTTON, CHECKBOX, 
            INT_SLIDER, 
            FLOAT_SLIDER, 
            TEXTBOX, 
            SIZE
        } type;

        union {
            bool as_bool;
            f32 as_f32;
            i32 as_i32;
        } data;
        enum struct data_type_e {
            BOOL, FLOAT, INT, SIZE
        };
        data_type_e data_type = data_type_e::SIZE;

        [[nodiscard]] v2f size() const {
            if (has_texture()) {
                return texture.get().get_image_size() + padding * 2.0f + margin * 2.0f;
            } else {
                return padding + margin;                
            }
        }

        [[nodiscard]] bool has_texture() const {
            return texture.valid();
        }

        virtual ~widget_t() = default;
    };
};

