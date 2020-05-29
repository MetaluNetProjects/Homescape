#!/usr/bin/python

# sudo apt-get install fontforge python-fontforge

import fontforge
import sys

font = fontforge.open(sys.argv[1]+".otf")
font.generate(sys.argv[1]+".ttf")

