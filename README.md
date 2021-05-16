# Wasm Doom

This is a [Chocolate Doom][1] Web Assembly port with WebSockets [support][4].

## Requirements

You need to install Emscripten and a few other tools first:

```
brew install emscripten
brew install automake
brew install sdl2 sdl2_mixer sdl2_net
```

## Compiling

There are two scripts to facilitate compiling Wasm Doom:

```
./scripts/clean.sh
./scripts/build.sh
```

## Running

Copy the shareware version of [doom1.wad][3] do ./src (make sure it has the name doom1.wad)

Then:

```
cd src
python -m SimpleHTTPServer
```

Then open your browser and point it to http://0.0.0.0:8000/

Doom should start.

Inspect src/index.html for startup details.

Check our live multiplayer [demo][5].

## License

Chocolate Doom and this port are distributed under the GNU GPL. See the COPYING file for more information.

[1]: https://github.com/chocolate-doom/chocolate-doom
[2]: https://emscripten.org/
[3]: https://doomwiki.org/wiki/DOOM1.WAD
[4]: src/net_websockets.c
[5]: https://silentspacemarine.com
