#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/script_editor.hpp>
#include <godot_cpp/classes/item_list.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/reg_ex.hpp>

#include <vector>

using namespace godot;

class SymbolSearch : public EditorPlugin
{
        GDCLASS(SymbolSearch, EditorPlugin);

private:
        PanelContainer *popup;
        LineEdit *filter_edit;
        ItemList *item_list;
        ScriptEditor *script_editor;

        struct Symbol {
                String name;
                int line;
                int column;
                String type;
        };
        std::vector<Symbol> all_symbols;
        std::vector<Symbol> filtered_symbols;

        void _update_list();
        void _load_popup();
        void _refresh_symbols();
        void _filter_symbols(const String &p_filter);

protected:
        static void _bind_methods();

public:
        SymbolSearch();
        ~SymbolSearch();

        void _on_filter_changed(const String &p_text);
        void _on_item_activated(int p_index);

        virtual void _input(const Ref<InputEvent> &event) override;

        virtual void _enter_tree() override;

        virtual void _exit_tree() override;
};
