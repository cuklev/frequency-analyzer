project('frequency-analyzer', 'c', 'cpp')

cxx = meson.get_compiler('cpp')

src_files = [
	'fft.cpp',
	'main.cpp',
	'stream.c',
]

ncurses_dep = dependency('ncurses')
pulse_dep = cxx.find_library('pulse-simple')
threads_dep = dependency('threads')
deps = [ncurses_dep, pulse_dep, threads_dep]

executable('analyzer', src_files, dependencies : deps)
