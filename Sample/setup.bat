@rem �T���v���̊e�t�H���_��tkEngine�ւ̃V���{���b�N�����N���쐬����B
@setlocal
@set CURRENT_DIR=%~dp0
@pushd "%CURRENT_DIR%

@for /d %%a in (.\*) do (call :makeSymbolicLink "%%a")
@goto end


:makeSymbolicLink
@set SAMPLE_FOLDER=%~1
@pushd %~1

@xcopy /Y /I "%CURRENT_DIR%\..\GameTemplate\Game\Assets\font" "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\font"
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\k2Engine" "%CURRENT_DIR%\..\GameTemplate\k2Engine"
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\k2EngineLow" "%CURRENT_DIR%\..\GameTemplate\k2EngineLow"
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\modelData\preset" "%CURRENT_DIR%\..\GameTemplate\Game\Assets\modelData\preset" 
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\shader" "%CURRENT_DIR%\..\GameTemplate\Game\Assets\shader" 

@exit /b

:end
@popd
