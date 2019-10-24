#! /usr/bin/env bash
PATH_TO_VANGERS_DATA_DIR="REPLACE_ME"



# https://stackoverflow.com/questions/3915040/bash-fish-command-to-print-absolute-path-to-a-file/23002317
get_abs_filename()
{
  # $1 : relative filename.
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}



invalid_path_to_vangers_data_dir()
{
  PATH_TO_VANGERS_DATA_DIR_SH=$(get_abs_filename "../path_to_vangers_data_dir.sh")
  echo "Please, open ${PATH_TO_VANGERS_DATA_DIR_SH} in text editor and set that variable to directory of Vangers which contains game.lst file."
  unset PATH_TO_VANGERS_DATA_DIR_SH
  unset PATH_TO_VANGERS_DATA_DIR
  unset PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS
  unset PATH_TO_VANGERS_END_SCREENS_IMGS
  unset SOURCE_DIR_3D
  unset SOURCE_DIR_2D_ITEM
  unset SOURCE_DIR_2D_END_SCREEN
}



# Define all needed paths only if PATH_TO_VANGERS_DATA_DIR points to existing directory.
if test ! -d "${PATH_TO_VANGERS_DATA_DIR}"
then
  echo "Value of PATH_TO_VANGERS_DATA_DIR is not path to directory."
  invalid_path_to_vangers_data_dir
else
  PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS="$(find "${PATH_TO_VANGERS_DATA_DIR}" -maxdepth 4 -mindepth 4 -type d -ipath "*/resource/actint/800x600/items" -print -quit)"
  PATH_TO_VANGERS_END_SCREENS_IMGS="$(find "${PATH_TO_VANGERS_DATA_DIR}" -maxdepth 3 -mindepth 3 -type d -ipath "*/resource/video/img" -print -quit)"
  if test -z "${PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS}" -o -z "${PATH_TO_VANGERS_END_SCREENS_IMGS}" 
  then
    echo "Value of PATH_TO_VANGERS_DATA_DIR is path to directory but some of Vangers folders are missing."
    invalid_path_to_vangers_data_dir
  else
    SOURCE_DIR_3D[0]="--source_dir=${PATH_TO_VANGERS_DATA_DIR}"
    SOURCE_DIR_2D_ITEM[0]="--source_dir=${PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS}"
    SOURCE_DIR_2D_END_SCREEN[0]="--source_dir=${PATH_TO_VANGERS_END_SCREENS_IMGS}"
  fi
fi
