# Generates large gd script file for testing plugin

import random
import os

def generate_large_gdscript(filepath, num_lines=10000):
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, 'w') as f:
        f.write("extends Node\n")
        f.write("class_name LargeTestScript\n\n")

        f.write("# Constants\n")
        for i in range(20):
            f.write(f"const CONST_{i} = {i}\n")
        f.write("\n")

        f.write("# Enums\n")
        f.write("enum MyEnum {\n")
        for i in range(10):
            f.write(f"    VALUE_{i},\n")
        f.write("}\n\n")

        f.write("# Signals\n")
        for i in range(10):
            f.write(f"signal signal_{i}(param1, param2)\n")
        f.write("\n")

        f.write("# Variables\n")
        for i in range(50):
            f.write(f"var variable_{i} = \"value_{i}\"\n")
        f.write("\n")

        line_count = 50 # Approximate

        func_count = 0
        while line_count < num_lines:
            f.write(f"func function_{func_count}(arg1, arg2):\n")
            f.write(f"    var local_var = arg1 + arg2\n")
            # Add some filler lines
            filler_lines = random.randint(5, 20)
            for j in range(filler_lines):
                f.write(f"    local_var += {j}\n")
            f.write(f"    return local_var\n\n")

            line_count += filler_lines + 3
            func_count += 1

            # Occasionally add a nested class
            if func_count % 50 == 0:
                f.write(f"class InnerClass_{func_count // 50}:\n")
                f.write(f"    var inner_var = 1\n")
                f.write(f"    func inner_func():\n")
                f.write(f"        pass\n\n")
                line_count += 4

    print(f"Generated {filepath} with approx {line_count} lines and {func_count} functions.")

if __name__ == "__main__":
    generate_large_gdscript("godot/large_script.gd")
