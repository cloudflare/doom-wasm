# Doom Wasm PWA App

This is a [Chocolate Doom][1] WebAssembly PWA App port with WebSockets [support][4].

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

Copy the shareware version of [doom1.wad][3] to [./src][9] (make sure it has the name doom1.wad)

Then:

```
cd src
python -m http.server
```

Then open your browser and point it to http://0.0.0.0:8000/

Doom should start (local mode, no network). Check [doom-workers][8] if you want to run multiplayer locally.

Inspect [src/index.html][6] for startup details.

Check our live multiplayer [demo][5] and [blog post][7].

## stdout procotol

To show important messages coming from the game while it's running we send the following formatted stdout messages, which can be parsed in the web page running the wasm:

```
doom: 1, failed to connect to websockets server
doom: 2, connected to %s
doom: 3, we're out of client addresses
doom: 4, ws error(eventType=%d, userData=%d)
doom: 5, ws close(eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%d)
doom: 6, failed to send ws packet, reconnecting
doom: 7, failed to connect to %s
doom: 8, uid is %d
doom: 9, disconnected from server
doom: 10, game started
doom: 11, entering fullscreen
doom: 12, client '%s' timed out and disconnected
```

# Build and Run with Docker?

### Dockerfile:
```Dockerfile
FROM emscripten/emsdk AS builder

WORKDIR /usr/doom

RUN apt update
RUN apt list --upgradable

RUN apt install automake git wget libsdl2-dev libsdl2-mixer-dev libsdl2-net-dev -y

# Add doom1.wad file URL
RUN wget <doom1.wad>

RUN git clone https://github.com/Saketh-Chandra/doom-wasm-pwa

RUN mv doom1.wad doom-wasm-pwa/src

RUN ./doom-wasm-pwa/scripts/clean.sh
RUN ./doom-wasm-pwa/scripts/build.sh

FROM python:alpine3.17 AS Final

COPY --from=builder /usr/doom/doom-wasm-pwa/ doom-wasm-pwa

EXPOSE 8000
ENTRYPOINT ["python3","-m","http.server","-d","./doom-wasm-pwa/src"]
```
#### Save the Docker Script into Dockerfile
### Build:
```bash
docker build -t doom_wasm_pwa .
```
### Run:
```bash
docker run --name doom_wasm_pwa -p 8000:8000 doom_wasm_pwa
```

## License

Chocolate Doom and this port are distributed under the GNU GPL. See the COPYING file for more information.

[1]: https://github.com/chocolate-doom/chocolate-doom
[2]: https://emscripten.org/
[3]: https://doomwiki.org/wiki/DOOM1.WAD
[4]: src/net_websockets.c
[5]: https://silentspacemarine.com
[6]: src/index.html
[7]: https://blog.cloudflare.com/doom-multiplayer-workers
[8]: https://github.com/cloudflare/doom-workers
[9]: src
