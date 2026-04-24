# Technical Overview: Godot Symbol Search Plugin

This document tries to provides an full technical deep-dive into the architecture and implementation of the **Godot Symbol Search** plugin.

## 1. Architecture Overview

The plugin is built as a **GDExtension** using the [godot-cpp](https://github.com/godotengine/godot-cpp) bindings. It integrates directly into the Godot Editor by inheriting from `EditorPlugin`.

### Key Components:
- **`SymbolSearch` Class**: The central controller that manages the UI lifecycle, input handling, and symbol indexing.
- **Registration**: Registered at `MODULE_INITIALIZATION_LEVEL_EDITOR` to access editor-specific APIs like `ScriptEditor`.
- **UI System**: Utilizes a pre-instantiated scene (`godot_symbol_search_panel.tscn`) containing a `PanelContainer`, `LineEdit` (for filtering), and `ItemList` (for display). It leverages **Scene Unique Names** (`%ItemList`, `%LineEdit`) for robust node retrieval.
- **Persistence Layer**: Tracks `original_line` and `original_column` to allow for "Restore on Cancel" functionality and smart initial selection.

---

## 2. Core Implementation Details

### Symbol Extraction (`_refresh_symbols`)
The plugin performs on-demand indexing of the currently active script:
- **Regex Parsing**: Uses Godot's `RegEx` engine with the pattern:
  `^\s*(?:static\s+)?(func|var|const|signal)\s+([a-zA-Z_0-9]+)`
- **Line-by-Line Scan**: Iterates through the source code strings, capturing the symbol type, name, and its exact column position for precision navigation.

### Fuzzy Matching & Smart Selection
- **Fuzzy Search**: A case-insensitive algorithm checks if the characters of the search pattern appear in order within the symbol name.
- **Closest Symbol Selection**: When the search modal is opened, the plugin calculates the distance between the current cursor line and every symbol. It automatically selects and highlights the symbol closest to the cursor (`p_keep_closest = true`), providing immediate context-aware focus.

### Navigation & Editor Interaction
- **Real-time Preview**: As the user navigates the `ItemList`, the plugin calls `script_editor->goto_line(s.line)` to provide an immediate visual preview in the editor.
- **Caret Precision**: To place the caret at the exact column of the symbol name, the plugin casts the editor's base control to a `CodeEdit` and uses `set_caret_line` and `set_caret_column`.
- **Focus Management**: Uses `grab_focus()` to transition between the search bar and the code editor seamlessly.

---

## 3. Advanced Input Handling

For smooth UX, the plugin implements multi-layer input handling:

1.  **Global Hook (`_input`)**: Listens for the `Ctrl+Shift+O` shortcut globally. It also detects clicks outside the popup to auto-close and restore the cursor.
2.  **Local Redirect (`_on_script_editor_input`)**: Specifically connected to the `ScriptEditor`'s `gui_input`. This ensures that even when the Script Editor is detached, the shortcut still functions correctly.
3.  **Event Acceptance**: Uses `accept_event()` and `set_input_as_handled()` to prevent shortcuts from leaking into the underlying editor when the search modal is active.

---

## 4. Lifecycle & Memory Management

- **Initialization**: The popup is instantiated once during `_enter_tree` via `ResourceLoader` and added as a child of the `ScriptEditor` (or the base control if the editor is unavailable).
- **Cleanup**: Uses `queue_free()` during `_exit_tree` to ensure all UI nodes are safely disposed of.
- **Binding**: Methods intended for signal connections (e.g., `_on_filter_changed`) are explicitly bound in `_bind_methods`.

---

## 5. Build System (CMake)

The project leverages a standard CMake workflow to compile the C++ source into a shared library.
- **`ADDON_DIR` Macro**: Defined as a `res://` path during compilation, allowing the C++ code to dynamically locate its `.tscn` resource file relative to the Godot project root.
- **Optimization**: Compiles with `CMAKE_BUILD_TYPE=Release` by for releases for maximum performance and minimal final lib size (`dll`, `.so`, `.dylib`).
