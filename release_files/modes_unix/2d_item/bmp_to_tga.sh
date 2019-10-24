#! /usr/bin/env bash
source "../path_to_vangers_data_dir.sh"

mkdir -p "./intermediate_tga"

"../../bin/tractor_converter" \
  "-c../../configs/2d_item/bmp_to_tga/bmp_to_tga.cfg" "${SOURCE_DIR_2D_ITEM[@]}"

read -n1 -r -p "Press any key to continue..."
