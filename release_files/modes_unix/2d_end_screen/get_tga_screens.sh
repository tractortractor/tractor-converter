#! /usr/bin/env bash
source "../path_to_vangers_data_dir.sh"

mkdir -p \
  "../../tmp/2d_end_screen/original_pal/"{"regular/"{"full","used","unused"},"usage"}
mkdir -p "./intermediate_tga_no_font_palette"

"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/get_tga_screens/vangers_pal_to_tga_pal.cfg" \
  "${SOURCE_DIR_2D_END_SCREEN[@]}"
"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/get_tga_screens/usage_pal.cfg" \
  "${SOURCE_DIR_2D_END_SCREEN[@]}"
"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/get_tga_screens/remove_not_used_pal.cfg"
"../../bin/tractor_converter" \
  "-c../../configs/2d_end_screen/get_tga_screens/bmp_to_tga.cfg" \
  "${SOURCE_DIR_2D_END_SCREEN[@]}"
read -n1 -r -p "Press any key to continue..."
