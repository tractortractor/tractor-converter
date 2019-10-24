@echo off

if not exist "./intermediate_tga_final" mkdir "./intermediate_tga_final"

"../../bin/tractor_converter" "-c../../configs/2d_end_screen/add_font_palette/tga_merge_unused_pal.cfg"
pause
