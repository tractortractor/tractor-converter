@echo off
setlocal enableextensions
call "../path_to_vangers_data_dir.bat"

if not exist "../../tmp/2d_end_screen/original_pal/regular/full" (
  mkdir "../../tmp/2d_end_screen/original_pal/regular/full"
)
if not exist "../../tmp/2d_end_screen/original_pal/regular/used" (
  mkdir "../../tmp/2d_end_screen/original_pal/regular/used"
)
if not exist "../../tmp/2d_end_screen/original_pal/regular/unused" (
  mkdir "../../tmp/2d_end_screen/original_pal/regular/unused"
)
if not exist "../../tmp/2d_end_screen/original_pal/usage" (
  mkdir "../../tmp/2d_end_screen/original_pal/usage"
)
if not exist "./intermediate_tga_no_font_palette" (
  mkdir "./intermediate_tga_no_font_palette"
)

"../../bin/tractor_converter" ^
  "-c../../configs/2d_end_screen/get_tga_screens/vangers_pal_to_tga_pal.cfg" ^
  %SOURCE_DIR_2D_END_SCREEN%
"../../bin/tractor_converter" ^
  "-c../../configs/2d_end_screen/get_tga_screens/usage_pal.cfg" ^
  %SOURCE_DIR_2D_END_SCREEN%
"../../bin/tractor_converter" ^
  "-c../../configs/2d_end_screen/get_tga_screens/remove_not_used_pal.cfg"
"../../bin/tractor_converter" ^
  "-c../../configs/2d_end_screen/get_tga_screens/bmp_to_tga.cfg" ^
  %SOURCE_DIR_2D_END_SCREEN%

endlocal
pause
