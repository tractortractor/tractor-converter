@echo off
call "../path_to_vangers_data_dir.bat"

if not exist "./intermediate_obj" mkdir "./intermediate_obj"

"../../bin/tractor_converter" "-c./m3d_to_obj.cfg" %SOURCE_DIR_3D%

pause
