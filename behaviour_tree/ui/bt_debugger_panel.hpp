#ifndef BT_DEBUGGER_PANEL_HPP
#define BT_DEBUGGER_PANEL_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>

class BTDebuggerPanel : public godot::Control
{
    GDCLASS(BTDebuggerPanel, godot::Control);

private:
    godot::Label* label = nullptr;
public:
    BTDebuggerPanel() {}

    void _ready()
    {
        this->label = memnew(godot::Label);
        label->set_text("Debugger Panel");
        add_child(label);
    }

protected:
    static void _bind_methods() {}
};

#endif /* BT_DEBUGGER_PANEL_HPP */