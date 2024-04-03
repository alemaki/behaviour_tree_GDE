import os

#TODO: Clean

# Include necessary environment
env = SConscript('godot_cpp/SConstruct')

# Append the doctest path
env.Append(CPPPATH=["doctest/"])

# Append the source directory to the include path
env.Append(CPPPATH=["behaviour_tree/"])


#for libraries
#env.Append(LIB="...")
#env.Append(LIBPATH="...")

# Function to collect CPP files recursively from a directory
def collect_cpp_files(root):
    cpp_files = []
    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            if filename.endswith('.cpp'):
                cpp_files.append(os.path.join(dirpath, filename))
    return cpp_files

# Collect all CPP files
src = []
src += collect_cpp_files("doctest/doctest")
src += collect_cpp_files("behaviour_tree")

env.Object(src)

Return("env")