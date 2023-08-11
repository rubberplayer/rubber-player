g++  -std=c++17 \
    src/main.cc src/rpapplication.cc src/view.cc src/db.cc src/player.cc src/rboptions.cc src/selections.cc src/sound.cc src/waveform.cc \
    `pkg-config --cflags --libs gtkmm-4.0` \
    `pkg-config --cflags --libs libpulse-simple` \
    `pkg-config --cflags --libs sndfile` \
    `pkg-config --cflags --libs rubberband` \
    `pkg-config --cflags --libs sqlite3` \
    -o rubber-player
