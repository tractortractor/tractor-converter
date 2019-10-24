@echo off 
setlocal enableextensions



set "FPS=15"

rem DITHERING must be number 0-3.
rem '0' means no dithering,
rem '1' means standard Floyd-Steinberg error diffusion,
rem '2' means Floyd-Steinberg error diffusion with reduced bleeding,
rem '3' means dithering based on pixel location ('Fixed' dithering).
set "DITHERING=1"

set "PATH_TO_INPUT_VIDEOS=REPLACE_ME"
set "PATH_TO_GIMP=REPLACE_ME"
set "PATH_TO_FFMPEG=REPLACE_ME"



rem Important! Contents of PATH_TO_INPUT_IMAGES and PATH_TO_OUTPUT_IMAGES
rem folders may be deleted!
rem Contents of PATH_TO_OUTPUT_VIDEOS may be overwritten.
rem PATH_TO_INPUT_IMAGES must have
rem \\ file separators on windows and / separators on unix.
set "PATH_TO_INPUT_IMAGES=..\\..\\tmp\\2d_item_preview\\in_pictures"
set "PATH_TO_OUTPUT_IMAGES=..\\..\\tmp\\2d_item_preview\\out_pictures"
set "PATH_TO_OUTPUT_VIDEOS=.\\out_avi"

mkdir "%PATH_TO_INPUT_IMAGES%"
mkdir "%PATH_TO_OUTPUT_IMAGES%"
mkdir "%PATH_TO_OUTPUT_VIDEOS%"

set "IMAGE_NAME=image."



for %%f in (%PATH_TO_INPUT_VIDEOS%\*) do (



  del /Q ^"%PATH_TO_INPUT_IMAGES%\%IMAGE_NAME%*.png^"
  del /Q ^"%PATH_TO_OUTPUT_IMAGES%\%IMAGE_NAME%*.png^"



  "%PATH_TO_FFMPEG%" ^
    -i ^"%%f^" ^
    -r %FPS% ^
    ^"%PATH_TO_INPUT_IMAGES%/%IMAGE_NAME%%%09d.png^"

  "%PATH_TO_GIMP%" ^
    --no-interface ^
    --no-data ^
    --no-fonts ^
    --gimprc ../../gimp_scripts/gimprc_windows ^
    --batch-interpreter plug-in-script-fu-eval ^
    --batch ^"^(script-fu-tractor-converter-index-png ^
                  \"%PATH_TO_INPUT_IMAGES%\\%IMAGE_NAME%*.png\" ^
                  \"%PATH_TO_OUTPUT_IMAGES%\" ^
                  %DITHERING%^)^" ^
    --batch ^"^(gimp-quit 0^)^"

  "%PATH_TO_FFMPEG%" ^
    -framerate %FPS% ^
    -i ^"%PATH_TO_OUTPUT_IMAGES%/%IMAGE_NAME%%%09d.png^" ^
    -y ^
    -codec copy ^
    ^"%PATH_TO_OUTPUT_VIDEOS%/%%~nf.avi^"



)



endlocal
pause
