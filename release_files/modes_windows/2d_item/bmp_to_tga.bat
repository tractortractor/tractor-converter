@echo off
call "../path_to_vangers_data_dir.bat"

if not exist "./intermediate_tga" mkdir "./intermediate_tga"

"../../bin/tractor_converter" ^
  "-c../../configs/2d_item/bmp_to_tga/bmp_to_tga.cfg" %SOURCE_DIR_2D_ITEM%

pause
