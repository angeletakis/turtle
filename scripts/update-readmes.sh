#!/bin/bash


# Looks for a .github directory and outputs a markdown file
turtle_root="~/turtle"
dirs=( . src include lib docs scripts )

for d in ${dirs[@]}; do
  mkdwnfile="$turtle_root/$d/.github/README.md"
  txtfile="$turtle_root/$d/README.txt"
  [ -f "$txtfile" ]   && rm "$txtfile"
  [ -f "$mkdwnfile" ] && pandoc --verbose --from markdown --to plain "$mkdwnfile" -o "$txtfile"
  echo "$?"
done