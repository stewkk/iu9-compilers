{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let
  tex = (pkgs.texlive.combine {
    inherit (pkgs.texlive) scheme-small
      fvextra;
  });
  pythonWithPytest = pkgs.python310.buildEnv.override {
    extraLibs = with pkgs.python310Packages; [
      pip
      virtualenv
      pytest
    ];
    ignoreCollisions = true;
  };
in
mkShell.override { stdenv = pkgs.llvmPackages_18.stdenv; } {
  buildInputs = [
    pkgs.pandoc
    tex
    pkgs.gopls
    pkgs.go_1_21
    pkgs.cmake
    pkgs.icu.dev
    pkgs.clojure
    pkgs.clojure-lsp
    pkgs.leiningen
    pkgs.flex
    pythonWithPytest
    pkgs.pyright
  ];

  nativeBuildInputs = [
    pkgs.clang-tools_18
    pkgs.icu.dev
  ];

  NIX_LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
    pkgs.stdenv.cc.cc
    pkgs.zlib
  ];
  NIX_LD = pkgs.lib.fileContents "${pkgs.stdenv.cc}/nix-support/dynamic-linker";
  shellHook = ''
    export VENV_DIR="$PWD/.venv"
    if [ ! -d "$VENV_DIR" ]; then
      ${pythonWithPytest}/bin/python -m venv $VENV_DIR
      source $VENV_DIR/bin/activate
      pip install pip setuptools wheel
    else
      source $VENV_DIR/bin/activate
    fi

    export LD_LIBRARY_PATH=$NIX_LD_LIBRARY_PATH
    export PYTHONPATH="${pythonWithPytest}/lib/python3.10/site-packages:$PYTHONPATH"

    if [ -f requirements.txt ]; then
      pip install -r requirements.txt
    fi
    pip install pytest lark

    python --version
  '';
}
