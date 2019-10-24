@echo off
setlocal enableextensions

if not exist "./out_bmp/resource/actint/800x600/items" mkdir "./out_bmp/resource/actint/800x600/items"
if not exist "./out_bmp/resource/actint/iscreen/items" mkdir "./out_bmp/resource/actint/iscreen/items"

"../../bin/tractor_converter" "-c../../configs/2d_item/tga_to_bmp/tga_to_bmp.cfg"

endlocal
pause
