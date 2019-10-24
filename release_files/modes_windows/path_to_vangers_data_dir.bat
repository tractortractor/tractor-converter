@echo off
setlocal enableextensions enabledelayedexpansion
set "PATH_TO_VANGERS_DATA_DIR=REPLACE_ME"



rem Define all needed paths only if PATH_TO_VANGERS_DATA_DIR points to existing directory.
call :check_path_status "%PATH_TO_VANGERS_DATA_DIR%" PATH_TO_VANGERS_DATA_DIR_STATUS
if "%PATH_TO_VANGERS_DATA_DIR_STATUS%" neq "folder" (
  echo Value of PATH_TO_VANGERS_DATA_DIR is not path to directory.
  call :invalid_path_to_vangers
  goto:eof
) else (
  set "REL_PATH_TO_ITEMS_OUTSIDE_IMGS=resource\actint\800x600\items"
  set "REL_PATH_TO_VANGERS_END_SCREENS_IMGS=resource\video\img"

  call :get_folder_case_insensitive_part ^
          PATH_TO_VANGERS_DATA_DIR ^
          REL_PATH_TO_ITEMS_OUTSIDE_IMGS ^
          PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS
  call :get_folder_case_insensitive_part ^
          PATH_TO_VANGERS_DATA_DIR ^
          REL_PATH_TO_VANGERS_END_SCREENS_IMGS ^
          PATH_TO_VANGERS_END_SCREENS_IMGS

  if "!PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS!" neq "" if "!PATH_TO_VANGERS_END_SCREENS_IMGS!" neq "" goto :rel_paths_exists
  :rel_paths_missing
    echo Value of PATH_TO_VANGERS_DATA_DIR is path to directory but some of Vangers folders are missing.
    call :invalid_path_to_vangers
	goto:eof
  :rel_paths_exists
    set "SOURCE_DIR_3D=--source_dir="!PATH_TO_VANGERS_DATA_DIR!""
    set "SOURCE_DIR_2D_ITEM=--source_dir="!PATH_TO_VANGERS_ITEMS_OUTSIDE_IMGS!""
    set "SOURCE_DIR_2D_END_SCREEN=--source_dir="!PATH_TO_VANGERS_END_SCREENS_IMGS!""
)
(endlocal
  set "SOURCE_DIR_3D=%SOURCE_DIR_3D%"
  set "SOURCE_DIR_2D_ITEM=%SOURCE_DIR_2D_ITEM%"
  set "SOURCE_DIR_2D_END_SCREEN=%SOURCE_DIR_2D_END_SCREEN%"
)
goto:eof



:invalid_path_to_vangers
  set "PATH_TO_VANGERS_DATA_DIR_BAT=%~dpnx0"
  echo Please, open %PATH_TO_VANGERS_DATA_DIR_BAT% in text editor and set that variable to directory of Vangers which contains game.lst file.
goto:eof



rem https://stackoverflow.com/questions/6359820/how-to-set-commands-output-as-a-variable-in-a-batch-file/6359924#6359924
rem First argument  - variable which stores command to execute.
rem Second argument - variable to store last line of output of executed command.
:set_var_from_command_out
setlocal enableextensions enabledelayedexpansion
for /f "tokens=*" %%a in (
  '!%1!'
) do (
  set "OUTPUT=%%a"
)
(endlocal
  if "%~2" neq "" set "%~2=%OUTPUT%"
)
goto:eof



rem https://stackoverflow.com/questions/8666225/how-to-test-if-a-path-is-a-file-or-directory-in-windows-batch-file/8669636#8669636
rem First argument  - path to check.
rem Second argument - variable to store path status.
:check_path_status
setlocal enableextensions disabledelayedexpansion
for /f "tokens=1,2 delims=d" %%A in ("-%~a1") do if "%%B" neq "" (
  set "PATH_STATUS=folder"
) else if "%%A" neq "-" (
  set "PATH_STATUS=file"
) else (
  set "PATH_STATUS=does not exist"
)
(endlocal
  if "%~2" neq "" set "%~2=%PATH_STATUS%"
)
goto:eof



rem First argument  - variable which holds case sensitive part of the path.
rem Second argument - variable which holds case insensitive part of the path.
rem Third argument  - variable to store path to folder.
:get_folder_case_insensitive_part
setlocal enableextensions enabledelayedexpansion
set "FULL_PATH=!%~1!\!%~2!"

set "GET_PATH_COMMAND=dir /b /s "!%~1!" | findstr /i /e /c:"%FULL_PATH%""

call :set_var_from_command_out GET_PATH_COMMAND EXACT_PATH

set "EXACT_PATH_STATUS=not set"

call :check_path_status "%EXACT_PATH%" EXACT_PATH_STATUS

if "%EXACT_PATH_STATUS%" neq "folder" set "EXACT_PATH="

(endlocal
  if "%~3" neq "" set "%~3=%EXACT_PATH%"
)
goto:eof
