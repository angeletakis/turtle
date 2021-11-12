#!/bin/bash


# Looks for a .github directory and outputs a markdown file
find . -maxdepth 2 -type d -exec bash -c "pandoc --from markdown --to plain .github/README.md -o README.txt" \;
