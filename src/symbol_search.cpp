#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/reg_ex_match.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/script_editor_base.hpp>
#include <godot_cpp/classes/code_edit.hpp>
#include <godot_cpp/classes/window.hpp>

#include "symbol_search.hpp"

using namespace godot;

SymbolSearch::SymbolSearch() : popup(nullptr), filter_edit(nullptr), item_list(nullptr), script_editor(nullptr) {}
SymbolSearch::~SymbolSearch() {}

void SymbolSearch::_bind_methods()
{
        ClassDB::bind_method(D_METHOD("_on_filter_changed", "text"), &SymbolSearch::_on_filter_changed);
        ClassDB::bind_method(D_METHOD("_on_item_activated", "index"), &SymbolSearch::_on_item_activated);
        ClassDB::bind_method(D_METHOD("_on_filter_gui_input", "event"), &SymbolSearch::_on_filter_gui_input);
        ClassDB::bind_method(D_METHOD("_on_script_editor_input", "event"), &SymbolSearch::_on_script_editor_input);
}

void SymbolSearch::_load_popup()
{
        popup = memnew(PanelContainer);
        popup->set_as_top_level(true);

        if (script_editor) {
                script_editor->add_child(popup);

                // listen to input directly from script editor to handle detached windows
                if (!script_editor->is_connected("gui_input", Callable(this, "_on_script_editor_input"))) {
                        script_editor->connect("gui_input", Callable(this, "_on_script_editor_input"));
                }
        } else {
                EditorInterface::get_singleton()->get_base_control()->add_child(popup);
        }

        VBoxContainer *vbox = memnew(VBoxContainer);
        popup->add_child(vbox);

        filter_edit = memnew(LineEdit);
        filter_edit->set_placeholder("Search symbol...");
        vbox->add_child(filter_edit);
        filter_edit->connect("text_changed", Callable(this, "_on_filter_changed"));
        filter_edit->connect("gui_input", Callable(this, "_on_filter_gui_input"));

        item_list = memnew(ItemList);
        item_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
        item_list->set_custom_minimum_size(Vector2(600, 400));
        vbox->add_child(item_list);
        item_list->connect("item_activated", Callable(this, "_on_item_activated"));

        popup->hide();
}

void SymbolSearch::_enter_tree()
{
        script_editor = EditorInterface::get_singleton()->get_script_editor();
        this->_load_popup();
}

void SymbolSearch::_exit_tree()
{
        if (popup)
        {
                popup->queue_free();
                popup = nullptr;
        }
}

void SymbolSearch::_refresh_symbols()
{
        all_symbols.clear();
        if (!script_editor) return;

        Ref<Script> script = script_editor->get_current_script();
        if (script.is_null()) return;

        String source = script->get_source_code();
        PackedStringArray lines = source.split("\n");

        Ref<RegEx> regex;
        regex.instantiate();
        regex->compile("^\\s*(?:static\\s+)?(func|var|const|signal)\\s+([a-zA-Z_0-9]+)");

        for (int i = 0; i < lines.size(); ++i)
        {
                Ref<RegExMatch> match = regex->search(lines[i]);
                if (match.is_valid())
                {
                        Symbol s;
                        s.type = match->get_string(1);
                        s.name = match->get_string(2);
                        s.line = i;
                        s.column = match->get_start(2);
                        all_symbols.push_back(s);
                }
        }
}

static bool fuzzy_match(const String &p_pattern, const String &p_str)
{
        if (p_pattern.is_empty())
                return true;

        String pattern = p_pattern.to_lower();
        String str = p_str.to_lower();

        int p_idx = 0;
        for (int s_idx = 0; s_idx < str.length() && p_idx < pattern.length(); s_idx++)
        {
                if (pattern[p_idx] == str[s_idx])
                {
                        p_idx++;
                }
        }
        return p_idx == pattern.length();
}

void SymbolSearch::_filter_symbols(const String &p_filter)
{
        filtered_symbols.clear();
        for (const auto &s : all_symbols)
        {
                if (fuzzy_match(p_filter, s.name))
                {
                        filtered_symbols.push_back(s);
                }
        }
        _update_list();
}

void SymbolSearch::_update_list()
{
        item_list->clear();
        for (const auto &s : filtered_symbols)
        {
                int idx = item_list->add_item("[" + s.type + "] " + s.name);
                item_list->set_item_metadata(idx, s.line);
        }

        if (item_list->get_item_count() > 0)
        {
                item_list->select(0);
        }
}

void SymbolSearch::_on_filter_changed(const String &p_text)
{
        _filter_symbols(p_text);
}

void SymbolSearch::_on_item_activated(int p_index)
{
        if (p_index < 0 || p_index >= (int)filtered_symbols.size()) return;

        const Symbol &s = filtered_symbols[p_index];
        script_editor->goto_line(s.line);

        ScriptEditorBase *current_editor = script_editor->get_current_editor();
        if (current_editor) {
                Control *base_editor = current_editor->get_base_editor();
                if (base_editor) {
                        base_editor->grab_focus();

                        CodeEdit *code_edit = Object::cast_to<CodeEdit>(base_editor);
                        if (code_edit) {
                                code_edit->set_caret_line(s.line);
                                code_edit->set_caret_column(s.column);
                        }
                }
        }

        popup->hide();
}

void SymbolSearch::_on_filter_gui_input(const Ref<InputEvent> &p_event)
{
        Ref<InputEventKey> key_event = p_event;
        if (!key_event.is_valid() || !key_event->is_pressed() || key_event->is_echo()) return;

        auto key = key_event->get_keycode();
        if (key == KEY_ESCAPE)
        {
                popup->hide();
                filter_edit->accept_event();
        }
        else if (key == KEY_UP || key == KEY_DOWN)
        {
                int count = item_list->get_item_count();
                if (count > 0)
                {
                        int current = -1;
                        if (item_list->get_selected_items().size() > 0)
                        {
                                current = item_list->get_selected_items()[0];
                        }
                        int next = (key == KEY_UP) ? (current - 1) : (current + 1);
                        next = (next + count) % count;
                        item_list->select(next);
                        item_list->ensure_current_is_visible();
                }
                filter_edit->accept_event();
        }
        else if (key == KEY_ENTER || key == KEY_KP_ENTER)
        {
                if (item_list->get_selected_items().size() > 0)
                {
                        _on_item_activated(item_list->get_selected_items()[0]);
                }
                filter_edit->accept_event();
        }
}

void SymbolSearch::_on_script_editor_input(const Ref<InputEvent> &p_event)
{
        Ref<InputEventKey> key_event = p_event;
        if (!key_event.is_valid() || !key_event->is_pressed() || key_event->is_echo()) return;

        if (key_event->get_keycode() == KEY_O && key_event->is_command_or_control_pressed() && key_event->is_shift_pressed())
        {
                if (!popup->is_visible())
                {
                        _refresh_symbols();
                        _filter_symbols("");
                        popup->show();
                        popup->set_anchors_and_offsets_preset(Control::PRESET_CENTER, Control::PRESET_MODE_KEEP_SIZE);
                        filter_edit->set_text("");
                        filter_edit->grab_focus();
                }
                else
                {
                        popup->hide();
                }
                script_editor->accept_event();
        }
}

void SymbolSearch::_input(const Ref<InputEvent> &event)
{
        // Still keep this for when the script editor is in the main window and has no gui_input handled
        if (!script_editor || !script_editor->is_visible_in_tree()) return;

        Ref<InputEventKey> key_event = event;
        if (!key_event.is_valid() || !key_event->is_pressed() || key_event->is_echo()) return;

        if (key_event->get_keycode() == KEY_O && key_event->is_command_or_control_pressed() && key_event->is_shift_pressed())
        {
                if (!popup->is_visible())
                {
                        _refresh_symbols();
                        _filter_symbols("");
                        popup->show();
                        popup->set_anchors_and_offsets_preset(Control::PRESET_CENTER, Control::PRESET_MODE_KEEP_SIZE);
                        filter_edit->set_text("");
                        filter_edit->grab_focus();
                        get_viewport()->set_input_as_handled();
                }
        }
}
