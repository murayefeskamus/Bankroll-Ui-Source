#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "colors.h"
#include <string>
#include <cstdio>
#include <cmath>

namespace UI {

inline bool SliderFloat(const char* label, float* v, float vmin, float vmax,
                        const char* suffix = "", const char* fmt = "%.0f",
                        float col_w = -1.f) {
    ImDrawList* dl      = ImGui::GetWindowDrawList();
    ImVec2      pos     = ImGui::GetCursorScreenPos();
    float       avail_w = (col_w > 0.f) ? col_w : ImGui::GetContentRegionAvail().x;

    const float lbl_h   = ImGui::GetTextLineHeight();
    const float track_h = 3.0f;
    const float val_w   = 40.0f;
    const float minus_w = 12.0f;
    const float plus_w  = 12.0f;
    const float gap     = 6.0f;
    const float track_w = avail_w - val_w - minus_w - plus_w - gap * 3;
    const float total_h = lbl_h + 4.0f + track_h + 5.0f;

    dl->AddText(pos, Colors::TextDim, label);

    ImVec2 tp0 = ImVec2(pos.x + minus_w + gap, pos.y + lbl_h + 4.0f);
    ImVec2 tp1 = ImVec2(tp0.x + track_w, tp0.y + track_h);

    std::string id = std::string("##sl") + label;
    ImGui::SetCursorScreenPos(ImVec2(tp0.x, tp0.y - 4.0f));
    ImGui::InvisibleButton(id.c_str(), ImVec2(track_w, track_h + 8.0f));
    bool held    = ImGui::IsItemActive();
    bool changed = false;

    if (held) {
        float mx = ImGui::GetIO().MousePos.x;
        float t  = ImClamp((mx - tp0.x) / track_w, 0.0f, 1.0f);
        float nv = vmin + t * (vmax - vmin);
        if (fabsf(nv - *v) > 1e-5f) { *v = nv; changed = true; }
    }

    float t = (vmax > vmin) ? ImClamp((*v - vmin) / (vmax - vmin), 0.0f, 1.0f) : 0.0f;

    dl->AddRectFilledMultiColor(tp0, tp1,
        IM_COL32(16, 16, 16, 255), IM_COL32(16, 16, 16, 255),
        IM_COL32(23, 23, 23, 255), IM_COL32(23, 23, 23, 255));

    if (t > 0.001f) {
        ImVec2 fe = ImVec2(tp0.x + t * track_w, tp1.y);

        ImVec4 acc  = ImGui::ColorConvertU32ToFloat4(Colors::Accent);
        ImVec4 dark = ImVec4(acc.x * 0.62f, acc.y * 0.62f, acc.z * 0.62f, 1.0f);
        ImU32  dim  = ImGui::ColorConvertFloat4ToU32(dark);

        dl->AddRectFilledMultiColor(tp0, fe,
            dim,           Colors::Accent,
            Colors::Accent, dim);
    }

    char buf[32];
    snprintf(buf, sizeof(buf), fmt, *v);
    char full[48];
    snprintf(full, sizeof(full), "%s%s", buf, suffix);

    float vy = tp0.y + (track_h - ImGui::GetTextLineHeight()) * 0.5f;
    dl->AddText(ImVec2(pos.x, vy), Colors::TextBind, "-");

    ImVec2 text_size = ImGui::CalcTextSize(full);
    float text_x = tp0.x + (track_w * t) - (text_size.x * 0.5f);
    text_x = ImClamp(text_x, tp0.x, tp0.x + track_w - text_size.x);
    float text_y = vy + 2.0f;

    dl->AddText(ImVec2(text_x - 1, text_y), IM_COL32(0, 0, 0, 180), full);
    dl->AddText(ImVec2(text_x + 1, text_y), IM_COL32(0, 0, 0, 180), full);
    dl->AddText(ImVec2(text_x, text_y - 1), IM_COL32(0, 0, 0, 180), full);
    dl->AddText(ImVec2(text_x, text_y + 1), IM_COL32(0, 0, 0, 180), full);
    dl->AddText(ImVec2(text_x, text_y), Colors::TextBright, full);

    dl->AddText(ImVec2(tp0.x + track_w + gap, vy), Colors::TextBind, "+");

    std::string minus_id = std::string("##minus") + label;
    std::string plus_id = std::string("##plus") + label;

    ImGui::SetCursorScreenPos(ImVec2(pos.x, vy - 4.0f));
    ImGui::InvisibleButton(minus_id.c_str(), ImVec2(minus_w, lbl_h + 8.0f));
    if (ImGui::IsItemClicked()) {
        float step = (vmax - vmin) * 0.01f;
        *v = ImClamp(*v - step, vmin, vmax);
        changed = true;
    }

    ImGui::SetCursorScreenPos(ImVec2(tp0.x + track_w + gap, vy - 4.0f));
    ImGui::InvisibleButton(plus_id.c_str(), ImVec2(plus_w + 8.0f, lbl_h + 8.0f));
    if (ImGui::IsItemClicked()) {
        float step = (vmax - vmin) * 0.01f;
        *v = ImClamp(*v + step, vmin, vmax);
        changed = true;
    }

    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + total_h));
    return changed;
}

}
