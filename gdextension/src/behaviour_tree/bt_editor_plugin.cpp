#include "bt_editor_plugin.hpp"

BTEditorPlugin::BTEditorPlugin()
{
    this->main_container = memnew(godot::SplitContainer);
    this->graph_editor= memnew(godot::GraphEdit);

    this->main_container->add_child(this->graph_editor);

    this->bottom_panel_button = 
        add_control_to_bottom_panel(this->main_container,
                                    this->bottom_panel_button_name);


    this->_make_visible(false);
}

BTEditorPlugin::~BTEditorPlugin()
{
    memfree(this->graph_editor);
    memfree(this->main_container);
}

void BTEditorPlugin::_make_visible(bool visible)
{
    if (visible)
    {
        this->bottom_panel_button->show();
        this->make_bottom_panel_item_visible(this->main_container);
    }
    else
    {
        if (this->main_container->is_visible_in_tree())
        {
            this->hide_bottom_panel();
        }
        this->bottom_panel_button->hide();
    }
}

bool BTEditorPlugin::_handles(Object* object) const
{
    return godot::Object::cast_to<BTTask>(object) != nullptr;
}

void BTEditorPlugin::_bind_methods()
{

}