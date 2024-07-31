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
        packages.doom-wasm = pkgs.callPackage ./doom-wasm.nix { };

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
            buildInputs = with pkgs; [
              # build tools
              emscripten
              ccls
              autoconf
              automake
              gnumake
              python3
              pkg-config
              simple-http-server
              # libs
              SDL2
              SDL2_mixer
              SDL2_net
            ];
          };
      });
}
