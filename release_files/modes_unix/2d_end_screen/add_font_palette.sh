#! /usr/bin/env bash

mkdir -p "./intermediate_tga_final"

"../../bin/tractor_converter" "-c../../configs/2d_end_screen/add_font_palette/tga_merge_unused_pal.cfg"
read -n1 -r -p "Press any key to continue..."
