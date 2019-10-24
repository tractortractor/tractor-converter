#! /usr/bin/env bash

mkdir -p "./out_bmp/resource/actint/"{"800x600/items","iscreen/items"}

"../../bin/tractor_converter" "-c../../configs/2d_item/tga_to_bmp/tga_to_bmp.cfg"
read -n1 -r -p "Press any key to continue..."
