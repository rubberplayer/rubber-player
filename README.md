# Rubber Player

Rubber Player is a simple tool for rehearsing music that plays slowed-down looped fragments of audio via an easy graphical interface.

The software is available under the GNU General Public License (GPL) v3 (see LICENSE)

## Usage

### open a file

- **Open** button or **drag and drop** on the program window to open an audio file.
- Play/Pause file using **Play** button.

### on the waveform

- **left mouse button** sets play head and draws selections by dragging. 
- **mouse scroll wheel** or **resize gesture** zooms in and out.
- **right mouse button** moves the view by dragging.

### time ratio (music playing speed)

Use the **time ratio slider** to slow down/accelerate playing speed (without changing pitch).

## Limitations

The audio file is fully loaded in memory and the visualisation might become slow for long audio files.

Libsndfile is able to open lots of formats (wav, aif, mp3, ogg, flac) but not everything.

## Compile

### using a direct command

    `./build.sh`
    ./rubber_player

### using meson

    meson setup builddir
    meson compile -C builddir    
    # alternatively:
    ninja -C builddir
    ./builddir/rubber_player

## Integrated libraries

- libsndfile by Erik de Castro Lopo and the libsndfile team (GNU General Public License v2.0)

    https://github.com/libsndfile/libsndfile

- rubberband by Chris Cannam (GNU General Public License v2.0)

    https://breakfastquay.com/rubberband/

- gtk4 (GNU General Public License v2.0)
- pulseaudio (GNU LESSER GENERAL PUBLIC LICENSE Version 2.1)