#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "colors.h"
#include <string>

namespace UI {

namespace _dd {
    inline ImGuiID      open_id    = 0;
    inline int          open_frame = -1;
    inline ImVec2       open_pos   = {};
    inline float        open_w     = 0.f;
    inline const char** items      = nullptr;
    inline int          count      = 0;
    inline int*         selected   = nullptr;
}

inline bool Dropdown(const char* label, int* sel, const char** items, int count, float col_w = -1.f) {
    ImDrawList* dl  = ImGui::GetWindowDrawList();
    ImVec2      pos = ImGui::GetCursorScreenPos();

    const float h = 17.0f;
    const float w = (col_w > 0.f) ? col_w : ImGui::GetContentRegionAvail().x;

    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(label, ImVec2(w, h));
    bool hov     = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    ImGuiID my_id = ImGui::GetID(label);

    if (clicked) {
        if (_dd::open_id == my_id) {
            _dd::open_id = 0;
        } else {
            _dd::open_id    = my_id;
            _dd::open_frame = ImGui::GetFrameCount();
            _dd::open_pos   = ImVec2(pos.x, pos.y + h);
            _dd::open_w     = w;
            _dd::items      = items;
            _dd::count      = count;
            _dd::selected   = sel;
        }
    }

    bool is_open = (_dd::open_id == my_id);

    dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), Colors::Bg, 2.0f);

    ImVec2 grad_top = pos;
    ImVec2 grad_bot = ImVec2(pos.x + w, pos.y + h);
    dl->AddRectFilledMultiColor(
        grad_top, grad_bot,
        IM_COL32(20, 20, 20, 255), IM_COL32(20, 20, 20, 255),
        IM_COL32(9, 9, 9, 255), IM_COL32(9, 9, 9, 255));

    const char* cur = (*sel >= 0 && *sel < count) ? items[*sel] : "";
    float ty = pos.y + (h - ImGui::GetTextLineHeight()) * 0.5f;
    dl->AddText(ImVec2(pos.x + 7.0f, ty), Colors::Text, cur);

    float ax = pos.x + w - 14.0f;
    float ay = pos.y + h * 0.5f;
    dl->AddTriangleFilled(
        ImVec2(ax,        ay - 2.0f),
        ImVec2(ax + 5.0f, ay - 2.0f),
        ImVec2(ax + 2.5f, ay + 2.0f),
        is_open ? Colors::Accent : Colors::TextBind);

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + h + 3.0f));
    return false;
}

inline void RenderOpenDropdown() {
    if (_dd::open_id == 0) return;

    ImDrawList* fdl    = ImGui::GetForegroundDrawList();
    const float item_h = 16.0f;
    float       total  = (float)_dd::count * item_h + 4.0f;
    ImVec2      p0     = _dd::open_pos;
    ImVec2      p1     = ImVec2(p0.x + _dd::open_w, p0.y + total);
    ImVec2      mouse  = ImGui::GetIO().MousePos;

    fdl->AddRectFilled(ImVec2(p0.x + 2, p0.y + 2), ImVec2(p1.x + 2, p1.y + 2),
                       IM_COL32(0, 0, 0, 60), 3.0f);
    fdl->AddRectFilled(p0, p1, Colors::Bg, 3.0f);

    int  click_sel  = -1;

    bool click_any  = ImGui::IsMouseClicked(ImGuiMouseButton_Left)
                   && (ImGui::GetFrameCount() > _dd::open_frame);

    for (int i = 0; i < _dd::count; i++) {
        ImVec2 ip0 = ImVec2(p0.x + 1.0f, p0.y + 2.0f + (float)i * item_h);
        ImVec2 ip1 = ImVec2(p1.x - 1.0f, ip0.y + item_h);

        bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x &&
                      mouse.y >= ip0.y && mouse.y <= ip1.y);
        bool sel_i = (*_dd::selected == i);

        if (hov_i) fdl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);

        float ty = ip0.y + (item_h - ImGui::GetTextLineHeight()) * 0.5f;
        ImU32 tc = sel_i ? Colors::Accent : (hov_i ? Colors::TextBright : Colors::Text);
        fdl->AddText(ImVec2(ip0.x + 7.0f, ty), tc, _dd::items[i]);

        if (hov_i && click_any) click_sel = i;
    }

    if (click_sel >= 0) {
        *_dd::selected = click_sel;
        _dd::open_id   = 0;
        return;
    }

    if (click_any) {
        bool inside = (mouse.x >= p0.x && mouse.x <= p1.x &&
                       mouse.y >= p0.y && mouse.y <= p1.y);
        if (!inside) _dd::open_id = 0;
    }
}

}
