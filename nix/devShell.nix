{
  perSystem = { pkgs, ... }: {

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
  };
}
