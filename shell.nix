{ pkgs ? import <nixpkgs> {} }:

with pkgs;

let
  tex = (pkgs.texlive.combine {
    inherit (pkgs.texlive) scheme-small
      fvextra;
  });
in
mkShell {
  buildInputs = [
    pkgs.pandoc
    tex
  ];
}
