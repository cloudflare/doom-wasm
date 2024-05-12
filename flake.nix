{
  description = "Doom WASM";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell
          {
            # Workaround taken from
            # https://discourse.nixos.org/t/emscripten-tries-to-write-to-nix/15263/2
            EM_CONFIG = pkgs.writeText ".emscripten" ''
              EMSCRIPTEN_ROOT = '${pkgs.emscripten}/share/emscripten'
              LLVM_ROOT = '${pkgs.emscripten.llvmEnv}/bin'
              BINARYEN_ROOT = '${pkgs.binaryen}'
              NODE_JS = '${pkgs.nodejs-18_x}/bin/node'
              # FIXME: Should use a better place, but needs to be an absolute path.
              CACHE = '/tmp/emscriptencache'
            '';
            buildInputs = [
              # build tools
              pkgs.emscripten
              pkgs.autoconf
              pkgs.automake
              pkgs.gnumake
              pkgs.python3
              pkgs.pkg-config
              pkgs.simple-http-server
              # libs
              pkgs.SDL2
              pkgs.SDL2_mixer
              pkgs.SDL2_net
            ];
          };
      });
}
