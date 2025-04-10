{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let
  tex = (pkgs.texlive.combine {
    inherit (pkgs.texlive) scheme-small
      fvextra;
  });
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
  ];

  nativeBuildInputs = [
    pkgs.clang-tools_18
    pkgs.icu.dev
  ];
}
