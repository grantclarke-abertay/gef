set FBX2SCN_EXE=..\..\..\tools\fbx2scn\build\vs2015\x64\Release\fbx2scn.exe
set FBX2SCN_FLAGS=

set ASSET_NAME=triceratop
set FBX2SCN_FLAGS=-axis-preset mayaZup -ignore-skinning -strip-texture-path -texture-extension .png
%FBX2SCN_EXE% -o ..\..\media\%ASSET_NAME%.scn %FBX2SCN_FLAGS% %ASSET_NAME%.fbx



pause