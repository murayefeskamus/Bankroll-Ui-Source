#include "menu.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "elements/elements.h"
#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

static const float MENU_W  = 448.0f;
static const float MENU_H  = 420.0f;
static const float TAB_H   = 26.0f;
static const float TITLE_H = 22.0f;
static const float PADDING = 10.0f;
static const float L_W     = 200.0f;
static const float GAP     = 14.0f;
static const float R_X     = PADDING + L_W + GAP;
static const float R_W     = MENU_W - R_X - PADDING;
static const float BOX_PAD = 6.0f;

static struct State {
    bool  aimbot          = true;
    bool  backtrack       = false;
    bool  triggerbot      = false;
    bool  magnet          = false;
    bool  penetration     = false;
    bool  override_shared = false;
    int   hitbox_sel      = 0;
    int   hitgroup_sel    = 0;
    int   weapon_sel      = 0;
    bool  silent          = false;
    bool  recoil_ctrl     = false;
    float fov             = 90.0f;
    float smooth          = 0.0f;
    bool  seed_pred       = false;
    float delay           = 0.0f;
    float hitchance       = 0.0f;
    int   active_tab      = 0;
    int   visuals_subtab  = 0;
    bool   box             = true;
    ImVec4 box_color       = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   health          = true;
    ImVec4 health_color    = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    ImVec4 health_color2   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   name            = true;
    ImVec4 name_color      = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   weapon          = true;
    ImVec4 weapon_color    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   ammo            = false;
    ImVec4 ammo_color      = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    ImVec4 ammo_color2     = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   out_of_view     = false;
    ImVec4 oov_color       = ImVec4(0.3f, 0.5f, 1.0f, 1.0f);
    bool   skeleton        = false;
    ImVec4 skeleton_color  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   flags_esp       = false;
    ImVec4 flags_color1    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 flags_color2    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 flags_color3    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   visible         = false;
    ImVec4 visible_color   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   occluded        = false;
    ImVec4 occluded_color  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool   history         = false;
    bool   glow            = false;
    ImVec4 glow_color1     = ImVec4(0.85f, 0.1f, 0.1f, 1.0f);
    ImVec4 glow_color2     = ImVec4(0.85f, 0.1f, 0.1f, 1.0f);

    bool   edge_jump        = false;
    bool   ladder_edge_jump = false;
    bool   long_jump        = false;
    bool   long_jump_edge   = true;
    bool   mini_jump        = false;
    bool   null_strafe      = true;
    bool   mousespeed_lim   = false;

    bool   chat_logs        = true;
    int    chat_logs_type   = 0;
    bool   healthshot       = false;
    bool   sounds           = true;
    int    sounds_type      = 0;
    float  sounds_volume    = 30.0f;

    float  ind_offset       = 67.0f;
    bool   ind_keybinds     = false;
    ImVec4 keybinds_col1    = ImVec4(0.2f, 0.85f, 0.2f, 1.0f);
    ImVec4 keybinds_col2    = ImVec4(1.0f, 1.0f,  1.0f, 1.0f);
    bool   vel_text         = false;
    ImVec4 vel_text_col1    = ImVec4(0.9f, 0.35f, 0.4f, 1.0f);
    ImVec4 vel_text_col2    = ImVec4(1.0f, 0.7f,  0.2f, 1.0f);
    ImVec4 vel_text_col3    = ImVec4(0.2f, 0.85f, 0.2f, 1.0f);
    bool   keystrokes       = false;
    ImVec4 keystrokes_col   = ImVec4(1.0f, 1.0f,  1.0f, 1.0f);
    bool   vel_graph        = false;
    ImVec4 vel_graph_col1   = ImVec4(1.0f, 1.0f,  1.0f, 1.0f);
    ImVec4 vel_graph_col2   = ImVec4(1.0f, 1.0f,  1.0f, 1.0f);

    bool   toggle_menu      = false;
    ImVec4 menu_color       = ImVec4(0.686f, 0.392f, 0.784f, 1.0f);
} s;

static const char* k_hitboxes[]  = { "head", "neck", "chest", "stomach", "pelvis" };
static const char* k_hitgroups[] = { "head, chest...", "head only", "all", "chest, arms" };
static const char* k_weapons[]   = { "shared", "pistols", "rifles", "snipers", "smg" };
static const char* k_tabs[]      = { "aim", "visuals", "skins", "misc", "config", "lua" };
static const char* k_visuals_subtabs[] = { "enemy", "friendly", "world", "extra" };
static const char* k_chat_types[]      = { "edgebug", "killfeed", "both" };
static const char* k_sound_types[]     = { "hit", "kill", "hurt" };

static std::vector<std::string> s_cfg_list;
static int                      s_cfg_sel   = -1;
static char                     s_cfg_input[64] = {};
static bool                     s_cfg_dirty = true;

static const char* k_cfg_dir = "C:\\bankroll\\";
static const char* k_cfg_ext = ".$$$";

static void CfgEnsureDir() {
    CreateDirectoryA(k_cfg_dir, nullptr);
}

static void CfgScan() {
    std::string prev;
    if (s_cfg_sel >= 0 && s_cfg_sel < (int)s_cfg_list.size())
        prev = s_cfg_list[s_cfg_sel];
    s_cfg_list.clear();
    WIN32_FIND_DATAA fd;
    std::string pattern = std::string(k_cfg_dir) + "*" + k_cfg_ext;
    HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            std::string name = fd.cFileName;
            size_t ext_pos = name.rfind(k_cfg_ext);
            if (ext_pos != std::string::npos) name = name.substr(0, ext_pos);
            s_cfg_list.push_back(name);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    s_cfg_sel   = -1;
    s_cfg_dirty = false;
    if (!prev.empty()) {
        for (int i = 0; i < (int)s_cfg_list.size(); i++) {
            if (s_cfg_list[i] == prev) { s_cfg_sel = i; break; }
        }
    }
}

static void CfgCreate() {
    if (s_cfg_input[0] == '\0') return;
    CfgEnsureDir();
    std::string name = s_cfg_input;
    std::string path = std::string(k_cfg_dir) + name + k_cfg_ext;
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    CloseHandle(h);
    CfgScan();
    for (int i = 0; i < (int)s_cfg_list.size(); i++) {
        if (s_cfg_list[i] == name) { s_cfg_sel = i; break; }
    }
    s_cfg_input[0] = '\0';
}

static void CfgSave() {
    if (s_cfg_sel < 0 || s_cfg_sel >= (int)s_cfg_list.size()) return;
    CfgEnsureDir();
    std::string path = std::string(k_cfg_dir) + s_cfg_list[s_cfg_sel] + k_cfg_ext;
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
}

static void CfgLoad() {

}

static void CfgOpenDir() {
    CfgEnsureDir();
    ShellExecuteA(nullptr, "open", k_cfg_dir, nullptr, nullptr, SW_SHOW);
}

static void DrawBox(ImDrawList* dl, ImVec2 wpos,
                    float lx, float box_top, float box_bottom, float w,
                    const char* label, ImU32 label_col) {
    auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };

    float lh   = ImGui::GetTextLineHeight();
    float gap  = 5.0f;

    ImVec2 ts  = ImGui::CalcTextSize(label);
    float  tx  = lx + (w - ts.x) * 0.5f;
    float  ty  = box_top - lh * 0.5f;

    dl->AddLine(S(lx, box_top), S(tx - gap, box_top), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(tx + ts.x + gap, box_top), S(lx + w, box_top), Colors::SectionBorder, 1.0f);

    dl->AddLine(S(lx, box_top), S(lx, box_bottom), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(lx + w, box_top), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(lx, box_bottom), S(lx + w, box_bottom), Colors::SectionBorder, 1.0f);

    dl->AddText(S(tx, ty), label_col, label);
}

static void DrawInnerSep(ImDrawList* dl, ImVec2 wpos,
                         float lx, float ly, float w, const char* label) {
    auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };
    float  lh  = ImGui::GetTextLineHeight();
    float  gap = 5.0f;
    ImVec2 ts  = ImGui::CalcTextSize(label);
    float  tx  = lx + (w - ts.x) * 0.5f;
    float  ty  = ly - lh * 0.5f;

    dl->AddLine(S(lx, ly), S(tx - gap, ly), Colors::SectionBorder, 1.0f);
    dl->AddLine(S(tx + ts.x + gap, ly), S(lx + w, ly), Colors::SectionBorder, 1.0f);

    dl->AddText(S(tx, ty), Colors::Section, label);
}

void Menu::Render() {

    UI::UpdateBindCapture();

    {
        ImVec4 mc = s.menu_color;
        Colors::Accent      = ImGui::ColorConvertFloat4ToU32(mc);
        ImVec4 hov(ImMin(mc.x * 1.09f, 1.0f), ImMin(mc.y * 1.09f, 1.0f),
                   ImMin(mc.z * 1.09f, 1.0f), mc.w);
        Colors::AccentHover  = ImGui::ColorConvertFloat4ToU32(hov);
        ImVec4 dark(mc.x * 0.32f, mc.y * 0.32f, mc.z * 0.32f, mc.w);
        Colors::AccentDark   = ImGui::ColorConvertFloat4ToU32(dark);
        ImVec4 bh(mc.x * 0.82f, mc.y * 0.82f, mc.z * 0.82f, mc.w);
        Colors::CbBorderHov  = ImGui::ColorConvertFloat4ToU32(bh);
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2((io.DisplaySize.x - MENU_W) * 0.5f,
               (io.DisplaySize.y - (MENU_H + TAB_H)) * 0.5f),
        ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(MENU_W, MENU_H + TAB_H));

    ImGuiWindowFlags wf =
        ImGuiWindowFlags_NoTitleBar        |
        ImGuiWindowFlags_NoResize          |
        ImGuiWindowFlags_NoScrollbar       |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground      |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(0, 0));
    ImGui::Begin("##bankroll", nullptr, wf);
    ImGui::PopStyleVar(2);

    ImDrawList* dl   = ImGui::GetWindowDrawList();
    ImVec2      wpos = ImGui::GetWindowPos();
    auto S = [&](float x, float y) { return ImVec2(wpos.x + x, wpos.y + y); };

    float total_h = MENU_H + TAB_H;

    dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + total_h), Colors::Bg, 4.0f);

    dl->AddRectFilled(wpos, ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H), Colors::TitleBg, 0.0f);

    ImVec2 grad_top = wpos;
    ImVec2 grad_bot = ImVec2(wpos.x + MENU_W, wpos.y + TITLE_H);
    dl->AddRectFilledMultiColor(
        grad_top, grad_bot,
        IM_COL32(20, 20, 20, 255), IM_COL32(20, 20, 20, 255),
        IM_COL32(7, 7, 7, 255), IM_COL32(7, 7, 7, 255));

    UI::DrawGradientLine(dl, wpos.x, wpos.y + TITLE_H, MENU_W, 1.0f);

    {
        const char* title = "bankroll mafia";
        ImVec2 ts = ImGui::CalcTextSize(title);
        dl->AddText(S((MENU_W - ts.x) * 0.5f, (TITLE_H - ts.y) * 0.5f),
                    IM_COL32(185, 182, 196, 255), title);
    }

    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::InvisibleButton("##titlebar", ImVec2(MENU_W, TITLE_H));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        ImVec2 new_pos = ImVec2(wpos.x + delta.x, wpos.y + delta.y);
        ImGui::SetWindowPos(new_pos);
    }

    const float cy   = TITLE_H + 10.0f;
    const float lh   = ImGui::GetTextLineHeight();

    if (s.active_tab == 1) {
        float subtab_y = TITLE_H + 4.0f;
        s.visuals_subtab = UI::SubTabBar(k_visuals_subtabs, 4, s.visuals_subtab,
                                         ImVec2(wpos.x, wpos.y + subtab_y), PADDING);

        float content_y = TITLE_H + lh + 18.0f;

        ImGui::SetCursorPos(ImVec2(0, content_y));

        if (s.visuals_subtab == 0) {
            const float lh2 = ImGui::GetTextLineHeight() + 5.0f;
            float l_sr = wpos.x + PADDING + L_W - BOX_PAD;
            float r_sr = wpos.x + R_X + R_W - BOX_PAD;

            float lbox_top = content_y;
            float ly = lbox_top + BOX_PAD + 5.0f;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("box", &s.box, nullptr, nullptr, L_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##box_col", &s.box_color, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("health", &s.health, nullptr, nullptr, L_W - BOX_PAD * 2 - 44.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##health_col1", &s.health_color,  l_sr, 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##health_col2", &s.health_color2, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("name", &s.name, nullptr, nullptr, L_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##name_col", &s.name_color, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("weapon", &s.weapon, nullptr, nullptr, L_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##weapon_col", &s.weapon_color, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("ammo", &s.ammo, nullptr, nullptr, L_W - BOX_PAD * 2 - 44.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##ammo_col1", &s.ammo_color,  l_sr, 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##ammo_col2", &s.ammo_color2, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("out of view indicator", &s.out_of_view, nullptr, nullptr, L_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##oov_col", &s.oov_color, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("skeleton", &s.skeleton, nullptr, nullptr, L_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##skeleton_col", &s.skeleton_color, l_sr, 0.0f);
            ly += lh2;

            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::Checkbox("flags", &s.flags_esp, nullptr, nullptr, L_W - BOX_PAD * 2 - 66.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##flags_col1", &s.flags_color1, l_sr, 44.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##flags_col2", &s.flags_color2, l_sr, 22.0f);
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            UI::ColorPicker("##flags_col3", &s.flags_color3, l_sr, 0.0f);
            ly += ImGui::GetTextLineHeight() + 5.0f;

            float lbox_bottom = ly + BOX_PAD;
            DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "enemy esp", Colors::ColHdr);

            float rbox_top = content_y;
            float ry = rbox_top + BOX_PAD + 3.0f;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("visible", &s.visible, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##visible_col", &s.visible_color, r_sr, 0.0f);
            ry += lh2;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("occluded", &s.occluded, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##occluded_col", &s.occluded_color, r_sr, 0.0f);
            ry += lh2;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("history", &s.history, nullptr, nullptr, R_W - BOX_PAD * 2);
            ry += lh2;

            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::Checkbox("glow", &s.glow, nullptr, nullptr, R_W - BOX_PAD * 2 - 44.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##glow_col1", &s.glow_color1, r_sr, 22.0f);
            ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
            UI::ColorPicker("##glow_col2", &s.glow_color2, r_sr, 0.0f);
            ry += ImGui::GetTextLineHeight() + 3.0f;

            float rbox_bottom = ry + BOX_PAD;
            DrawBox(dl, wpos, R_X, rbox_top, rbox_bottom, R_W, "enemy chams", Colors::ColHdr);
        }
        else if (s.visuals_subtab == 1) {
            float lbox_top = content_y;
            float ly = lbox_top + BOX_PAD;
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            ImGui::Dummy(ImVec2(L_W - BOX_PAD * 2, ImGui::GetTextLineHeight()));
            ly += ImGui::GetTextLineHeight();
            float lbox_bottom = ly + BOX_PAD;
            DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "friendly", Colors::ColHdr);
        }
        else if (s.visuals_subtab == 2) {
            float lbox_top = content_y;
            float ly = lbox_top + BOX_PAD;
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            ImGui::Dummy(ImVec2(L_W - BOX_PAD * 2, ImGui::GetTextLineHeight()));
            ly += ImGui::GetTextLineHeight();
            float lbox_bottom = ly + BOX_PAD;
            DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "world", Colors::ColHdr);
        }
        else if (s.visuals_subtab == 3) {
            float lbox_top = content_y;
            float ly = lbox_top + BOX_PAD;
            ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
            ImGui::Dummy(ImVec2(L_W - BOX_PAD * 2, ImGui::GetTextLineHeight()));
            ly += ImGui::GetTextLineHeight();
            float lbox_bottom = ly + BOX_PAD;
            DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "extra", Colors::ColHdr);
        }
    }

    if (s.active_tab == 0)
    {

    float lbox_top = cy;
    float ly = lbox_top + BOX_PAD;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Dropdown("##wpn", &s.weapon_sel, k_weapons, 5, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("override shared", &s.override_shared, nullptr, nullptr, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 4.0f;

    DrawInnerSep(dl, wpos, PADDING + BOX_PAD, ly, L_W - BOX_PAD * 2, "general");
    ly += lh + 4.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("aimbot", &s.aimbot, "aimbot", "M1", L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Dropdown("##htbx", &s.hitbox_sel, k_hitboxes, 5, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("backtrack", &s.backtrack, nullptr, nullptr, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("triggerbot", &s.triggerbot, "triggerbot", "ALT", L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Dropdown("##htgr", &s.hitgroup_sel, k_hitgroups, 4, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("magnet", &s.magnet, nullptr, nullptr, L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y + 2.0f;

    ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
    UI::Checkbox("penetration", &s.penetration, "penetration", "ALT", L_W - BOX_PAD * 2);
    ly = ImGui::GetCursorPos().y;

    float lbox_bottom = ly + BOX_PAD;
    DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "weapon group", Colors::ColHdr);

    float rbox1_top = cy;
    float ry = rbox1_top + BOX_PAD;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
    UI::Checkbox("silent", &s.silent, nullptr, nullptr, R_W - BOX_PAD * 2);
    ry = ImGui::GetCursorPos().y + 3.0f;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
    UI::SliderFloat("field of view", &s.fov, 0.0f, 180.0f, "", "%.1f", R_W - BOX_PAD * 2);
    ry = ImGui::GetCursorPos().y + 3.0f;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
    UI::SliderFloat("smooth", &s.smooth, 0.0f, 100.0f, "%", "%.0f", R_W - BOX_PAD * 2);
    ry = ImGui::GetCursorPos().y + 3.0f;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
    UI::Checkbox("recoil control", &s.recoil_ctrl, nullptr, nullptr, R_W - BOX_PAD * 2);
    ry = ImGui::GetCursorPos().y;

    float rbox1_bottom = ry + BOX_PAD;
    DrawBox(dl, wpos, R_X, rbox1_top, rbox1_bottom, R_W, "aimbot params", Colors::ColHdr);

    float rbox2_top = rbox1_bottom + 10.0f;
    float ry2 = rbox2_top + BOX_PAD;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry2));
    UI::Checkbox("seed prediction", &s.seed_pred, nullptr, nullptr, R_W - BOX_PAD * 2);
    ry2 = ImGui::GetCursorPos().y + 3.0f;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry2));
    UI::SliderFloat("delay", &s.delay, 0.0f, 500.0f, "ms", "%.0f", R_W - BOX_PAD * 2);
    ry2 = ImGui::GetCursorPos().y + 3.0f;

    ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry2));
    UI::SliderFloat("hitchance", &s.hitchance, 0.0f, 100.0f, "%", "%.0f", R_W - BOX_PAD * 2);
    ry2 = ImGui::GetCursorPos().y;

    float rbox2_bottom = ry2 + BOX_PAD;
    DrawBox(dl, wpos, R_X, rbox2_top, rbox2_bottom, R_W, "triggerbot params", Colors::ColHdr);

    }

    if (s.active_tab == 3) {
        const float gap2 = 2.0f;

        float lbox_top = cy;
        float ly = lbox_top + BOX_PAD;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("edge jump", &s.edge_jump, "edge_jump", "disabled", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("ladder edge jump", &s.ladder_edge_jump, "ladder_edge_jump", "disabled", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("long jump", &s.long_jump, "long_jump", "disabled", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("long jump on edge", &s.long_jump_edge, "long_jump_edge", "M3", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("mini jump", &s.mini_jump, "mini_jump", "M3", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("null strafe", &s.null_strafe, "null_strafe", "always on", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("mousespeed limiter", &s.mousespeed_lim, "mousespeed_lim", "none", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y;

        float lbox_bottom = ly + BOX_PAD;
        DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "movement", Colors::ColHdr);

        float mbox_top = lbox_bottom + 8.0f;
        float my = mbox_top + BOX_PAD;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::Checkbox("chat logs", &s.chat_logs, nullptr, nullptr, L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::Dropdown("##chat_type", &s.chat_logs_type, k_chat_types, 3, L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::Checkbox("healthshot", &s.healthshot, nullptr, nullptr, L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::Checkbox("sounds", &s.sounds, nullptr, nullptr, L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::Dropdown("##sound_type", &s.sounds_type, k_sound_types, 3, L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y + gap2;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, my));
        UI::SliderFloat("volume", &s.sounds_volume, 0.0f, 100.0f, "", "%.0f", L_W - BOX_PAD * 2);
        my = ImGui::GetCursorPos().y;

        float mbox_bottom = my + BOX_PAD;
        DrawBox(dl, wpos, PADDING, mbox_top, mbox_bottom, L_W, "miscellaneous", Colors::ColHdr);

        float r_sr    = wpos.x + R_X + R_W - BOX_PAD;
        float rbox_top = cy;
        float ry = rbox_top + BOX_PAD;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::SliderFloat("offset", &s.ind_offset, 0.0f, 200.0f, "", "%.0f", R_W - BOX_PAD * 2);
        ry = ImGui::GetCursorPos().y + gap2 + 3.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::Checkbox("keybinds", &s.ind_keybinds, nullptr, nullptr, R_W - BOX_PAD * 2 - 44.0f);
        float ry_next = ImGui::GetCursorPos().y + gap2;
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##kb_col1", &s.keybinds_col1, r_sr, 22.0f);
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##kb_col2", &s.keybinds_col2, r_sr, 0.0f);
        ry = ry_next;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::Checkbox("velocity text", &s.vel_text, nullptr, nullptr, R_W - BOX_PAD * 2 - 66.0f);
        ry_next = ImGui::GetCursorPos().y + gap2;
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##vt_col1", &s.vel_text_col1, r_sr, 44.0f);
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##vt_col2", &s.vel_text_col2, r_sr, 22.0f);
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##vt_col3", &s.vel_text_col3, r_sr, 0.0f);
        ry = ry_next;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::Checkbox("keystrokes", &s.keystrokes, nullptr, nullptr, R_W - BOX_PAD * 2 - 22.0f);
        ry_next = ImGui::GetCursorPos().y + gap2;
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##ks_col", &s.keystrokes_col, r_sr, 0.0f);
        ry = ry_next;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::Checkbox("velocity graph", &s.vel_graph, nullptr, nullptr, R_W - BOX_PAD * 2 - 44.0f);
        ry_next = ImGui::GetCursorPos().y;
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##vg_col1", &s.vel_graph_col1, r_sr, 22.0f);
        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::ColorPicker("##vg_col2", &s.vel_graph_col2, r_sr, 0.0f);
        ry = ry_next;

        float rbox_bottom = ry + BOX_PAD;
        DrawBox(dl, wpos, R_X, rbox_top, rbox_bottom, R_W, "indicators", Colors::ColHdr);
    }

    if (s.active_tab == 4) {
        if (s_cfg_dirty) CfgScan();

        float lbox_top = cy;
        float ly = lbox_top + BOX_PAD;
        float l_sr = wpos.x + PADDING + L_W - BOX_PAD;

        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::Checkbox("toggle menu", &s.toggle_menu, "toggle_menu", "INS", L_W - BOX_PAD * 2);
        ly = ImGui::GetCursorPos().y + 2.0f;

        dl->AddText(S(PADDING + BOX_PAD, ly + (16.0f - lh) * 0.5f), Colors::Text, "menu color");
        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly));
        UI::ColorPicker("##menu_col", &s.menu_color, l_sr, 0.0f);
        ImGui::SetCursorPos(ImVec2(PADDING + BOX_PAD, ly + 19.0f));
        ly += 19.0f;

        float lbox_bottom = ly + BOX_PAD;
        DrawBox(dl, wpos, PADDING, lbox_top, lbox_bottom, L_W, "menu", Colors::ColHdr);

        float rbox_top    = cy;
        float rbox_bottom = MENU_H - 8.0f;
        float ry = rbox_top + BOX_PAD;

        const float item_h   = 17.0f;
        const float btn_h    = 18.0f;
        const float input_h  = 17.0f;
        const float below    = 4.0f + input_h + 4.0f
                             + btn_h + 3.0f + btn_h + 3.0f
                             + btn_h + 3.0f + btn_h
                             + BOX_PAD + 8.0f;
        float list_h = rbox_bottom - ry - below;

        float  list_w   = R_W - BOX_PAD * 2;
        ImVec2 list_pos = S(R_X + BOX_PAD, ry);
        ImVec2 list_end = ImVec2(list_pos.x + list_w, list_pos.y + list_h);

        dl->AddRectFilled(list_pos, list_end, IM_COL32(6, 5, 7, 255), 3.0f);
        dl->AddRect(list_pos, list_end, Colors::SectionBorder, 3.0f, 0, 1.0f);

        ImVec2 mouse   = ImGui::GetIO().MousePos;
        int    hov_idx = -1;
        int    max_vis = (int)(list_h / item_h);

        for (int i = 0; i < (int)s_cfg_list.size() && i < max_vis; i++) {
            ImVec2 ip0 = ImVec2(list_pos.x + 1, list_pos.y + 1 + i * item_h);
            ImVec2 ip1 = ImVec2(list_end.x - 1, ip0.y + item_h);

            bool hov_i = (mouse.x >= ip0.x && mouse.x <= ip1.x &&
                          mouse.y >= ip0.y && mouse.y <= ip1.y);
            bool sel_i = (s_cfg_sel == i);

            if (hov_i) hov_idx = i;

            if (sel_i)      dl->AddRectFilled(ip0, ip1, IM_COL32(35, 32, 44, 255), 2.0f);
            else if (hov_i) dl->AddRectFilled(ip0, ip1, IM_COL32(22, 20, 27, 255), 2.0f);

            float ty2 = ip0.y + (item_h - lh) * 0.5f;
            ImU32 tc  = sel_i ? Colors::Accent : (hov_i ? Colors::TextBright : Colors::Text);
            dl->AddText(ImVec2(ip0.x + 6.0f, ty2), tc, s_cfg_list[i].c_str());
        }

        ImGui::SetCursorScreenPos(list_pos);
        ImGui::InvisibleButton("##cfg_list", ImVec2(list_w, list_h));
        if (ImGui::IsItemClicked() && hov_idx >= 0)
            s_cfg_sel = hov_idx;

        ry += list_h + 4.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        UI::TextInput("##cfg_name", s_cfg_input, sizeof(s_cfg_input), list_w);
        ry = ImGui::GetCursorPos().y + 4.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        if (UI::Button("create", list_w)) CfgCreate();
        ry = ImGui::GetCursorPos().y + 3.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        if (UI::Button("save", list_w)) CfgSave();
        ry = ImGui::GetCursorPos().y + 3.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        if (UI::Button("load", list_w)) CfgLoad();
        ry = ImGui::GetCursorPos().y + 3.0f;

        ImGui::SetCursorPos(ImVec2(R_X + BOX_PAD, ry));
        if (UI::Button("open directory", list_w)) CfgOpenDir();

        DrawBox(dl, wpos, R_X, rbox_top, rbox_bottom, R_W, "configs", Colors::ColHdr);

        ImGui::SetCursorPos(ImVec2(PADDING, rbox_bottom + 1.0f));
        ImGui::Dummy(ImVec2(1.0f, 1.0f));
    }

    s.active_tab = UI::TabBar(k_tabs, 6, s.active_tab,
                               ImVec2(wpos.x, wpos.y + MENU_H), MENU_W, TAB_H);

    {
        static bool s_drag = false;
        ImVec2 mp = ImGui::GetIO().MousePos;
        bool overlay_open = (UI::_cp::open_id != 0) || (UI::_dd::open_id != 0);
        bool over_tabbar  = (mp.y >= wpos.y + MENU_H);

        if (!overlay_open && !over_tabbar &&
            ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_drag = true;
        }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            s_drag = false;

        if (s_drag && !overlay_open) {
            ImVec2 d = ImGui::GetIO().MouseDelta;
            ImGui::SetWindowPos(ImVec2(wpos.x + d.x, wpos.y + d.y));
        }
    }

    UI::RenderOpenDropdown();
    UI::RenderOpenColorPicker();

    ImGui::End();
}
