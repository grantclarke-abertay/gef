set FBX2SCN_EXE=..\..\..\tools\fbx2scn\build\vs2015\x64\Release\fbx2scn.exe
set OUTPUT_DIR=..\..\media

set ASSET_NAME=Y_Bot
set FBX2SCN_FLAGS=-enable-skinning -scaling-factor 0.01 -strip-texture-path -texture-extension .png
%FBX2SCN_EXE% -o %OUTPUT_DIR%\%ASSET_NAME%.scn %FBX2SCN_FLAGS% %ASSET_NAME%.fbx

set ASSET_NAME=Y_Bot
set FBX2SCN_FLAGS=-scaling-factor 0.01 -strip-texture-path -texture-extension .png
%FBX2SCN_EXE% -o %OUTPUT_DIR%\%ASSET_NAME%_no_skinning.scn %FBX2SCN_FLAGS% %ASSET_NAME%.fbx

rem anims
set ASSET_NAME=idle
set FBX2SCN_FLAGS=-scaling-factor 0.01 -animation-only
%FBX2SCN_EXE% -o %OUTPUT_DIR%\%ASSET_NAME%.scn %FBX2SCN_FLAGS% %ASSET_NAME%.fbx

set ASSET_NAME=running_InPlace
set FBX2SCN_FLAGS=-scaling-factor 0.01 -animation-only
%FBX2SCN_EXE% -o %OUTPUT_DIR%\%ASSET_NAME%.scn %FBX2SCN_FLAGS% %ASSET_NAME%.fbx

pause