#! /usr/bin/env bash

mkdir -p "../../tmp/2d_end_screen/final_pal/regular/full"
mkdir -p "./out_bmp/resource/video/img"

"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/tga_to_bmp/extract_tga_pal.cfg"
"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/tga_to_bmp/tga_pal_to_vangers_pal.cfg"
"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/tga_to_bmp/tga_to_bmp.cfg"
read -n1 -r -p "Press any key to continue..."
