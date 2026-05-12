#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "colors.h"
#include <string>

namespace UI {

namespace _cp {
    inline ImGuiID open_id = 0;
    inline ImVec2  open_pos = {};
    inline ImVec4* editing_color = nullptr;
    inline float   hue = 0.0f;
    inline float   sat = 1.0f;
    inline float   val = 1.0f;
    inline float   alpha = 1.0f;
    inline ImVec2  menu_pos_when_opened = {};
    inline bool    dragging_inside = false;
}

inline bool ColorPicker(const char* label, ImVec4* color, float right_edge, float offset_from_right = 0.0f) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

    const float picker_w = 18.0f;
    const float picker_h = 12.0f;
    const float spacing = 4.0f;

    float pos_x = right_edge - picker_w - offset_from_right;
    ImVec2 pos = ImVec2(pos_x, cursor_pos.y);

    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(label, ImVec2(picker_w, picker_h));
    bool hov = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    ImGuiID my_id = ImGui::GetID(label);

    if (clicked) {
        if (_cp::open_id == my_id) {
            _cp::open_id = 0;
        } else {
            _cp::open_id = my_id;
            _cp::open_pos = ImVec2(pos.x + picker_w * 0.5f, pos.y + picker_h * 0.5f);
            _cp::editing_color = color;
            _cp::menu_pos_when_opened = ImGui::GetWindowPos();

            ImGui::ColorConvertRGBtoHSV(color->x, color->y, color->z, _cp::hue, _cp::sat, _cp::val);
            _cp::alpha = color->w;
        }
    }

    ImVec2 p1 = ImVec2(pos.x + picker_w, pos.y + picker_h);

    dl->AddRectFilled(pos, p1, IM_COL32(16, 16, 16, 255), 2.0f);

    ImU32 col_tl = ImGui::ColorConvertFloat4ToU32(*color);
    ImVec4 darker = ImVec4(color->x * 0.5f, color->y * 0.5f, color->z * 0.5f, 1.0f);
    ImU32 col_br = ImGui::ColorConvertFloat4ToU32(darker);

    dl->AddRectFilledMultiColor(
        ImVec2(pos.x + 1, pos.y + 1),
        ImVec2(p1.x - 1, p1.y - 1),
        col_tl, col_tl,
        col_br, col_br);

    if (hov) {
        dl->AddRect(pos, p1, Colors::Accent, 2.0f, 0, 1.5f);
    }

    ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y));

    return false;
}

inline void RenderOpenColorPicker() {
    if (_cp::open_id == 0) return;
    if (_cp::editing_color == nullptr) {
        _cp::open_id = 0;
        return;
    }

    ImVec2 current_menu_pos = ImGui::GetWindowPos();
    if (fabsf(current_menu_pos.x - _cp::menu_pos_when_opened.x) > 0.1f ||
        fabsf(current_menu_pos.y - _cp::menu_pos_when_opened.y) > 0.1f) {
        _cp::open_id = 0;
        return;
    }

    ImDrawList* fdl = ImGui::GetForegroundDrawList();
    const float picker_w = 220.0f;
    const float picker_h = 220.0f;
    const float sv_size = 180.0f;
    const float hue_w = 14.0f;
    const float alpha_h = 14.0f;
    const float gap = 8.0f;

    ImVec2 p0 = _cp::open_pos;
    ImVec2 p1 = ImVec2(p0.x + picker_w, p0.y + picker_h);

    ImVec2 mouse = ImGui::GetIO().MousePos;
    bool mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool mouse_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    bool inside_picker = (mouse.x >= p0.x && mouse.x <= p1.x &&
                          mouse.y >= p0.y && mouse.y <= p1.y);

    if (inside_picker && mouse_clicked) {
        _cp::dragging_inside = true;
    }

    if (!mouse_down) {
        _cp::dragging_inside = false;
    }

    if (inside_picker || _cp::dragging_inside) {
        ImGui::GetIO().WantCaptureMouse = true;
    }

    if (mouse_clicked && !inside_picker) {
        _cp::open_id = 0;
        _cp::dragging_inside = false;
        return;
    }

    fdl->AddRectFilled(ImVec2(p0.x + 2, p0.y + 2), ImVec2(p1.x + 2, p1.y + 2),
                       IM_COL32(0, 0, 0, 60), 4.0f);
    fdl->AddRectFilled(p0, p1, Colors::Bg, 4.0f);
    fdl->AddRect(p0, p1, IM_COL32(22, 22, 22, 255), 4.0f, 0, 1.5f);

    ImVec2 sv_pos = ImVec2(p0.x + 10.0f, p0.y + 10.0f);
    ImVec2 sv_end = ImVec2(sv_pos.x + sv_size, sv_pos.y + sv_size);

    ImVec4 hue_col;
    ImGui::ColorConvertHSVtoRGB(_cp::hue, 1.0f, 1.0f, hue_col.x, hue_col.y, hue_col.z);
    ImU32 hue_u32 = ImGui::ColorConvertFloat4ToU32(ImVec4(hue_col.x, hue_col.y, hue_col.z, 1.0f));

    fdl->AddRectFilledMultiColor(sv_pos, sv_end,
        IM_COL32(255, 255, 255, 255), hue_u32,
        hue_u32, IM_COL32(0, 0, 0, 255));

    fdl->AddRectFilledMultiColor(sv_pos, sv_end,
        IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0),
        IM_COL32(0, 0, 0, 255), IM_COL32(0, 0, 0, 255));

    bool sv_hov = (mouse.x >= sv_pos.x && mouse.x <= sv_end.x &&
                   mouse.y >= sv_pos.y && mouse.y <= sv_end.y);

    if (sv_hov && mouse_down) {
        _cp::sat = ImClamp((mouse.x - sv_pos.x) / sv_size, 0.0f, 1.0f);
        _cp::val = 1.0f - ImClamp((mouse.y - sv_pos.y) / sv_size, 0.0f, 1.0f);

        if (_cp::editing_color != nullptr) {
            ImGui::ColorConvertHSVtoRGB(_cp::hue, _cp::sat, _cp::val,
                _cp::editing_color->x, _cp::editing_color->y, _cp::editing_color->z);
            _cp::editing_color->w = _cp::alpha;
        }
    }

    float cursor_x = sv_pos.x + _cp::sat * sv_size;
    float cursor_y = sv_pos.y + (1.0f - _cp::val) * sv_size;
    fdl->AddCircleFilled(ImVec2(cursor_x, cursor_y), 4.0f, IM_COL32(255, 255, 255, 255), 12);
    fdl->AddCircle(ImVec2(cursor_x, cursor_y), 4.0f, IM_COL32(0, 0, 0, 255), 12, 1.5f);

    ImVec2 hue_pos = ImVec2(sv_end.x + gap, sv_pos.y);
    ImVec2 hue_end = ImVec2(hue_pos.x + hue_w, sv_end.y);

    for (int i = 0; i < 6; i++) {
        float y0 = hue_pos.y + (sv_size / 6.0f) * i;
        float y1 = hue_pos.y + (sv_size / 6.0f) * (i + 1);

        ImVec4 c0, c1;
        ImGui::ColorConvertHSVtoRGB(i / 6.0f, 1.0f, 1.0f, c0.x, c0.y, c0.z);
        ImGui::ColorConvertHSVtoRGB((i + 1) / 6.0f, 1.0f, 1.0f, c1.x, c1.y, c1.z);

        ImU32 col0 = ImGui::ColorConvertFloat4ToU32(ImVec4(c0.x, c0.y, c0.z, 1.0f));
        ImU32 col1 = ImGui::ColorConvertFloat4ToU32(ImVec4(c1.x, c1.y, c1.z, 1.0f));

        fdl->AddRectFilledMultiColor(
            ImVec2(hue_pos.x, y0), ImVec2(hue_end.x, y1),
            col0, col0, col1, col1);
    }

    bool hue_hov = (mouse.x >= hue_pos.x && mouse.x <= hue_end.x &&
                    mouse.y >= hue_pos.y && mouse.y <= hue_end.y);

    if (hue_hov && mouse_down) {
        _cp::hue = ImClamp((mouse.y - hue_pos.y) / sv_size, 0.0f, 1.0f);

        if (_cp::editing_color != nullptr) {
            ImGui::ColorConvertHSVtoRGB(_cp::hue, _cp::sat, _cp::val,
                _cp::editing_color->x, _cp::editing_color->y, _cp::editing_color->z);
            _cp::editing_color->w = _cp::alpha;
        }
    }

    float hue_cursor_y = hue_pos.y + _cp::hue * sv_size;
    fdl->AddRectFilled(
        ImVec2(hue_pos.x - 2, hue_cursor_y - 2),
        ImVec2(hue_end.x + 2, hue_cursor_y + 2),
        IM_COL32(255, 255, 255, 255), 1.0f);
    fdl->AddRect(
        ImVec2(hue_pos.x - 2, hue_cursor_y - 2),
        ImVec2(hue_end.x + 2, hue_cursor_y + 2),
        IM_COL32(0, 0, 0, 255), 1.0f, 0, 1.5f);

    ImVec2 alpha_pos = ImVec2(sv_pos.x, sv_end.y + gap);
    ImVec2 alpha_end = ImVec2(sv_end.x, alpha_pos.y + alpha_h);

    ImVec4 current_rgb;
    ImGui::ColorConvertHSVtoRGB(_cp::hue, _cp::sat, _cp::val, current_rgb.x, current_rgb.y, current_rgb.z);
    ImU32 col_opaque = ImGui::ColorConvertFloat4ToU32(ImVec4(current_rgb.x, current_rgb.y, current_rgb.z, 1.0f));
    ImU32 col_transparent = ImGui::ColorConvertFloat4ToU32(ImVec4(current_rgb.x, current_rgb.y, current_rgb.z, 0.0f));

    fdl->AddRectFilledMultiColor(alpha_pos, alpha_end,
        col_transparent, col_opaque,
        col_opaque, col_transparent);

    bool alpha_hov = (mouse.x >= alpha_pos.x && mouse.x <= alpha_end.x &&
                      mouse.y >= alpha_pos.y && mouse.y <= alpha_end.y);

    if (alpha_hov && mouse_down) {
        _cp::alpha = ImClamp((mouse.x - alpha_pos.x) / sv_size, 0.0f, 1.0f);
        if (_cp::editing_color != nullptr) {
            _cp::editing_color->w = _cp::alpha;
        }
    }

    float alpha_cursor_x = alpha_pos.x + _cp::alpha * sv_size;
    fdl->AddRectFilled(
        ImVec2(alpha_cursor_x - 2, alpha_pos.y - 2),
        ImVec2(alpha_cursor_x + 2, alpha_end.y + 2),
        IM_COL32(255, 255, 255, 255), 1.0f);
    fdl->AddRect(
        ImVec2(alpha_cursor_x - 2, alpha_pos.y - 2),
        ImVec2(alpha_cursor_x + 2, alpha_end.y + 2),
        IM_COL32(0, 0, 0, 255), 1.0f, 0, 1.5f);
}

}
