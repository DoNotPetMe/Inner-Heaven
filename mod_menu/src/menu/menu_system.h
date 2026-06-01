#pragma once
#include <vector>

struct MenuNode {
    enum Type { Sub, Toggle, Int, Float, Enum, Cmd, Sep };

    Type type         = Sub;
    const char* name  = "";
    const char* help  = "";

    bool*  boolVal    = nullptr;
    int*   intVal     = nullptr;
    float* floatVal   = nullptr;

    int   iMin = 0, iMax = 100, iStep = 1;
    float fMin = 0, fMax = 1.0f,  fStep = 0.1f;
    const char* suffix = "";

    const char** enumNames = nullptr;
    int enumCount          = 0;

    void(*cmdFunc)()  = nullptr;

    std::vector<MenuNode> children;
};

MenuNode MakeSub   (const char* name, const char* help, std::vector<MenuNode> ch);
MenuNode MakeToggle(const char* name, const char* help, bool* val);
MenuNode MakeInt   (const char* name, const char* help, int* val, int mn, int mx, int step = 1, const char* sfx = "");
MenuNode MakeFloat (const char* name, const char* help, float* val, float mn, float mx, float step = 0.1f, const char* sfx = "");
MenuNode MakeEnum  (const char* name, const char* help, int* val, const char** names, int count);
MenuNode MakeCmd   (const char* name, const char* help, void(*func)());
MenuNode MakeSep   ();

class MenuSystem {
public:
    static MenuSystem& Get();

    void SetRoot(MenuNode root);
    void Render();

    bool IsOpen() const  { return m_open; }
    void SetOpen(bool o) { m_open = o; }
    void Toggle()        { m_open = !m_open; }

private:
    struct NavEntry { MenuNode* node; int index; };

    MenuNode m_root;
    MenuNode* m_current = nullptr;
    int  m_selected     = 0;
    bool m_open         = false;
    std::vector<NavEntry> m_stack;

    void DrawBreadcrumb();
    void DrawItem(const MenuNode& item, int index);
    void HandleInput();
    void EnterSelected();
    void GoBack();
    void ChangeValue(int dir);
    void MoveSelection(int dir);
    void EnsureVisible();
};
