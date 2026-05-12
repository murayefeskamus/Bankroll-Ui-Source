#pragma once
#include "../imgui/imgui.h"
#include <string>
#include <unordered_map>
#include <cctype>

namespace UI {

struct BindEntry {
    std::string key;
    bool        waiting     = false;
    int         start_frame = -1;
};

inline std::unordered_map<std::string, BindEntry> g_binds;

inline void InitBind(const char* id, const char* default_key) {
    if (g_binds.find(id) == g_binds.end())
        g_binds[id] = { default_key ? default_key : "", false, -1 };
}

inline bool IsAnyBindWaiting() {
    for (auto& [id, e] : g_binds) if (e.waiting) return true;
    return false;
}

inline bool IsBindWaiting(const char* id) {
    auto it = g_binds.find(id);
    return it != g_binds.end() && it->second.waiting;
}

inline void StartBindWaiting(const char* id) {
    for (auto& [bid, e] : g_binds) e.waiting = false;
    auto it = g_binds.find(id);
    if (it != g_binds.end()) {
        it->second.waiting     = true;
        it->second.start_frame = ImGui::GetFrameCount();
    }
}

inline const char* GetBindDisplay(const char* id) {
    static char buf[48];
    auto it = g_binds.find(id);
    if (it == g_binds.end()) return "";
    if (it->second.waiting)  return "press key";
    if (it->second.key.empty()) return "";
    snprintf(buf, sizeof(buf), "[%s]", it->second.key.c_str());
    return buf;
}

inline void UpdateBindCapture() {
    for (auto& [id, e] : g_binds) {
        if (!e.waiting) continue;
        if (ImGui::GetFrameCount() <= e.start_frame + 1) continue;

        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) { e.waiting = false; return; }

        if (io.MouseClicked[0]) { e.key = "M1"; e.waiting = false; return; }
        if (io.MouseClicked[1]) { e.key = "M2"; e.waiting = false; return; }
        if (io.MouseClicked[2]) { e.key = "M3"; e.waiting = false; return; }
        if (io.MouseClicked[3]) { e.key = "M4"; e.waiting = false; return; }
        if (io.MouseClicked[4]) { e.key = "M5"; e.waiting = false; return; }

        for (int k = (int)ImGuiKey_NamedKey_BEGIN; k < (int)ImGuiKey_NamedKey_END; ++k) {
            ImGuiKey key = (ImGuiKey)k;
            if (key == ImGuiKey_Escape) continue;
            if (!ImGui::IsKeyPressed(key)) continue;

            const char* raw = ImGui::GetKeyName(key);
            std::string name = raw;

            if (name == "LeftAlt"   || name == "RightAlt")   name = "ALT";
            else if (name == "LeftCtrl"  || name == "RightCtrl")  name = "CTRL";
            else if (name == "LeftShift" || name == "RightShift") name = "SHIFT";
            else if (name == "LeftSuper" || name == "RightSuper") name = "WIN";
            else if (name == "MouseLeft")   name = "M1";
            else if (name == "MouseRight")  name = "M2";
            else if (name == "MouseMiddle") name = "M3";
            else {
                for (auto& c : name) c = (char)toupper((unsigned char)c);
            }

            e.key     = name;
            e.waiting = false;
            return;
        }
    }
}

}
