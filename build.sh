g++  -std=c++17 \
    src/base.cc src/player.cc src/sound.cc src/waveform.cc \
    `pkg-config --cflags --libs gtkmm-4.0` \
    `pkg-config --cflags --libs cairo` \
    `pkg-config --cflags --libs libpulse-simple` \
    `pkg-config --cflags --libs sndfile` \
    `pkg-config --cflags --libs rubberband` \
    -o rubber_player
