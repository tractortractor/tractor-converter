#! /usr/bin/env bash



FPS=15

# DITHERING must be number 0-3.
# '0' means no dithering,
# '1' means standard Floyd-Steinberg error diffusion,
# '2' means Floyd-Steinberg error diffusion with reduced bleeding,
# '3' means dithering based on pixel location ('Fixed' dithering).
DITHERING=1

PATH_TO_INPUT_VIDEOS="REPLACE_ME"
PATH_TO_GIMP="REPLACE_ME"
PATH_TO_FFMPEG="REPLACE_ME"



# Important! Contents of PATH_TO_INPUT_IMAGES and PATH_TO_OUTPUT_IMAGES
# folders may be deleted!
# Contents of PATH_TO_OUTPUT_VIDEOS may be overwritten.
# PATH_TO_INPUT_IMAGES must have
# \\ file separators on windows and / separators on unix.
PATH_TO_INPUT_IMAGES="../../tmp/2d_item_preview/in_pictures"
PATH_TO_OUTPUT_IMAGES="../../tmp/2d_item_preview/out_pictures"
PATH_TO_OUTPUT_VIDEOS="./out_avi"

mkdir -p "${PATH_TO_INPUT_IMAGES}"
mkdir -p "${PATH_TO_OUTPUT_IMAGES}"
mkdir -p "${PATH_TO_OUTPUT_VIDEOS}"

IMAGE_NAME="image."



for f in "$PATH_TO_INPUT_VIDEOS/"*
do

  AVI_BASE="${f##*/}"
  AVI_FILENAME="${AVI_BASE%.*}"



  find "$PATH_TO_INPUT_IMAGES" -type f -name "$IMAGE_NAME*.png" -delete
  find "$PATH_TO_OUTPUT_IMAGES" -type f -name "$IMAGE_NAME*.png" -delete



  "$PATH_TO_FFMPEG" \
    -i "$f" \
    -r "$FPS" \
    "$PATH_TO_INPUT_IMAGES/$IMAGE_NAME%09d.png"

  "$PATH_TO_GIMP" \
    --no-interface \
    --no-data \
    --no-fonts \
    --gimprc "../../gimp_scripts/gimprc_unix" \
    --batch-interpreter plug-in-script-fu-eval \
    --batch "(script-fu-tractor-converter-index-png \
                \"$PATH_TO_INPUT_IMAGES/$IMAGE_NAME*.png\" \
                \"$PATH_TO_OUTPUT_IMAGES\" \
                $DITHERING)" \
    --batch "(gimp-quit 0)"

  "$PATH_TO_FFMPEG" \
    -framerate "$FPS" \
    -i "$PATH_TO_OUTPUT_IMAGES/$IMAGE_NAME%09d.png" \
    -y \
    -codec copy \
    "$PATH_TO_OUTPUT_VIDEOS/$AVI_FILENAME.avi"

done



read -n1 -r -p "Press any key to continue..."
