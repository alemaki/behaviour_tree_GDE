import os
from SCons.Script import ARGUMENTS, SConscript, Return
env = None

# Build own godot-cpp and hand back a fully configured env with itself already linked into env['LIBS']:
#
#   env = SConscript(os.path.join('path', 'to', 'behaviour_tree_GDE', 'SConscript'))
env = SConscript(os.path.join('godot-cpp', 'SConstruct'))

if 'target' not in ARGUMENTS:
    ARGUMENTS['target'] = 'template_debug'

target = ARGUMENTS.get('target', 'template_debug')

# Godot doesn't like ".obj"
env['OBJSUFFIX'] = '.o'

if target == 'template_debug':
    env.Append(CPPDEFINES=['DEBUG'])
else:
    env.Append(CPPDEFINES=['NDEBUG'])

own_dir = os.path.abspath('.')

def collect_cpp_files(root, exclude_dirs):
    cpp_files = []
    for dirpath, _, filenames in os.walk(root):
        for filename in filenames:
            if any(excluded in dirpath for excluded in exclude_dirs):
                continue
            if filename.endswith('.cpp'):
                cpp_files.append(os.path.join(dirpath, filename))
    return cpp_files

# Mirror godot-cpp's own convention:
# Append directories to the cpp path so children can consume the content of the files
env.AppendUnique(CPPPATH=[own_dir])

behaviour_tree_env = env.Clone()

#  test_utils already its own static library, see tests/test_utils/SConscript.
exclude_dirs = [
    os.path.join(own_dir, 'godot-cpp'),
    os.path.join(own_dir, 'tests'),
]
sources = collect_cpp_files(own_dir, exclude_dirs)

library_name = 'libbehaviour_tree' + env['suffix'] + env['LIBSUFFIX']
behaviour_tree_lib = behaviour_tree_env.StaticLibrary(target=env.File(os.path.join('bin', library_name)), source=sources)

# Mirror godot-cpp's own convention:
env.AppendUnique(LIBS=[behaviour_tree_lib])

Return('env')
