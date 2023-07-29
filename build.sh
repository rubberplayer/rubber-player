g++ src/base.cc src/player.cc src/sound.cc src/waveform.cc -o base \
    `pkg-config --cflags --libs gtkmm-4.0` \
    `pkg-config --cflags --libs cairo` \
    `pkg-config --cflags --libs libpulse-simple` \
    `pkg-config --cflags --libs sndfile` \
    `pkg-config --cflags --libs rubberband` \
    -std=c++17
