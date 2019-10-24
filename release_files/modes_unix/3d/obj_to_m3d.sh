#! /usr/bin/env bash

mkdir -p "./out_m3d"

"../../bin/tractor_converter" "-c./obj_to_m3d.cfg"
read -n1 -r -p "Press any key to continue..."
