#! /usr/bin/env bash
source "../path_to_vangers_data_dir.sh"

mkdir -p "./intermediate_obj"

"../../bin/tractor_converter" "-c./m3d_to_obj.cfg" "${SOURCE_DIR_3D[@]}"

read -n1 -r -p "Press any key to continue..."
