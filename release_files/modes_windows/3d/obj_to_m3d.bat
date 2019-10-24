@echo off

if not exist "./out_m3d" mkdir "./out_m3d"

"../../bin/tractor_converter" "-c./obj_to_m3d.cfg"

pause
