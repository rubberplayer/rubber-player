project('rubber-player-meson', 'cpp')
alldeps = [ 
    dependency('gtkmm-4.0'), 
    dependency('libpulse-simple'),
    dependency('sndfile'), 
    dependency('rubberband'), 
    dependency('sqlite3')
]
sources = [
    'src/main.cc',
    'src/rpapplication.cc',
    'src/view.cc', 
    'src/db.cc',
    'src/player.cc',  
    'src/rboptions.cc',  
    'src/selections.cc',  
    'src/sound.cc',  
    'src/waveform.cc', 
]
executable('rubber-player', sources, dependencies : alldeps)

