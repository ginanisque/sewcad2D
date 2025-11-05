# Seamly2D Linux Setup (No Root Required)

The repository expects a Qt 6 toolchain together with several system libraries.
When `sudo` is unavailable you can stage every dependency inside your home
directory and build Seamly2D entirely from source.

The commands below assume the project lives in `~/sewcad2D`.

## 1. Python helpers

```bash
python3 -m pip install --user --break-system-packages aqtinstall cmake
```

Ensure `~/.local/bin` is on your `PATH` (`echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc`).

## 2. Qt 6.7.2

```bash
~/.local/bin/aqt install-qt -O "$HOME/Qt" linux desktop 6.7.2 linux_gcc_64 \
  -m qt5compat qtmultimedia qtimageformats qtsvg qttools qttranslations
```

This installs Qt under `~/Qt/6.7.2/gcc_64`.

## 3. Xerces-C++ 3.2.5

```bash
mkdir -p "$HOME/build" && cd "$HOME/build"
curl -LO https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.2.5.tar.xz
tar xf xerces-c-3.2.5.tar.xz
~/.local/bin/cmake -S xerces-c-3.2.5 -B xerces-c-3.2.5/build \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"
~/.local/bin/cmake --build xerces-c-3.2.5/build -j"$(nproc)"
~/.local/bin/cmake --install xerces-c-3.2.5/build
```

## 4. Extract required system libraries

Qt Multimedia pulls in OpenGL, PulseAudio and codec stacks. Without root we grab
the official Debian packages and unpack them into `~/.local`.

```bash
cd ~/sewcad2D
pkgs=(
  mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libglvnd-dev libgl-dev
  libpulse-dev libpulse0 libasyncns-dev libasyncns0 libsndfile1 libsndfile1-dev
  libmp3lame0 libmp3lame-dev libogg0 libogg-dev libvorbis0a libvorbisenc2 libvorbis-dev
  libflac12t64 libflac-dev libopus0 libopus-dev libmpg123-0t64 libmpg123-dev
)
for pkg in "${pkgs[@]}"; do
  apt-get download "$pkg"
done
for deb in *.deb; do
  dpkg -x "$deb" "$HOME/.local"
done
```

Remove the downloaded `.deb` files afterwards if disk space is tight.

## 5. Configure the build environment

Source the helper script before running `qmake`, `make`, or the binaries:

```bash
source scripts/seamly-env.sh
```

The script exports `PATH`, `LD_LIBRARY_PATH`, `LIBRARY_PATH`, and other include
paths so the locally installed toolchain is visible.

## 6. Build Seamly2D

```bash
source scripts/seamly-env.sh
mkdir -p "$HOME/build/seamly2d"
cd "$HOME/build/seamly2d"
qmake ../../sewcad2D/Seamly2D.pro
make -j"$(nproc)"
```

The binaries appear under:

- `~/build/seamly2d/src/app/seamly2d/bin/seamly2d`
- `~/build/seamly2d/src/app/seamlyme/bin/seamlyme`

For headless environments use `QT_QPA_PLATFORM=offscreen`.

## 7. Quick smoke test

```bash
source scripts/seamly-env.sh
QT_QPA_PLATFORM=offscreen ~/build/seamly2d/src/app/seamly2d/bin/seamly2d --help
```

You may still see a warning about `pdftops`; install `poppler-utils` (or unpack
its Debian package) if PDF export is required.
