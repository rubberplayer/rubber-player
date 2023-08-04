project('rubber-player-meson', 'cpp')
alldeps = [ 
    dependency('gtkmm-4.0'), 
    dependency('libpulse-simple'),
    dependency('sndfile'), 
    dependency('rubberband'), 
    dependency('jsoncpp'),
    dependency('sqlite3')
]
sources = ['src/base.cc', 'src/rboptions.cc',  'src/player.cc',  'src/sound.cc',  'src/waveform.cc', 'src/db.cc'

]
executable('rubber-player', sources, dependencies : alldeps)

