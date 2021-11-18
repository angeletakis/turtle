#!/bin/zsh


# Looks for a .github directory and outputs a markdown file
turtle_root=$( git rev-parse --show-toplevel )
dirs=( . src include lib docs scripts build bin )

for d in "${dirs[@]}"; do
  mkdwnfile="$turtle_root/$d/.github/README.md"
  txtfile="$turtle_root/$d/README.txt"
  [ -f "$mkdwnfile" ] && /usr/bin/pandoc --from markdown --to plain "$mkdwnfile" -o "$txtfile"
done
