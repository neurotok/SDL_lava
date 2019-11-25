cc = meson.get_compiler('c')
vulkandep = dependency('vulkan')
sdldep = dependency('SDL2')
mdep = cc.find_library('m', required : false)
src = ['main.c','SDL_lava.c','SDL_lava_utils.c', 'SDL_lava_memory.c', 'SDL_lava_import.c']
incdir = include_directories('../include')
executable('../lava', src,  include_directories : incdir, dependencies :[mdep, sdldep, vulkandep])
