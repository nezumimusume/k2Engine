@rem サンプルの各フォルダにtkEngineへのシンボリックリンクを作成する。
@setlocal
@set CURRENT_DIR=%~dp0
@pushd "%CURRENT_DIR%

@for /d %%a in (.\*) do (call :makeSymbolicLink "%%a")
@goto end


:makeSymbolicLink
@set SAMPLE_FOLDER=%~1
@pushd %~1

@xcopy /Y /I "%CURRENT_DIR%\..\GameTemplate\Game\Assets\modelData\preset" "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\modelData\preset"
@xcopy /Y /I /S "%CURRENT_DIR%\..\GameTemplate\Game\Assets\shader" "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\shader"
@xcopy /Y /I "%CURRENT_DIR%\..\GameTemplate\Game\Assets\font" "%CURRENT_DIR%\%SAMPLE_FOLDER%\Game\Assets\font"
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\tkEngine" "%CURRENT_DIR%\..\GameTemplate\tkEngine"
@mklink /D "%CURRENT_DIR%\%SAMPLE_FOLDER%\tkTools" "%CURRENT_DIR%\..\GameTemplate\tkTools"

@exit /b

:end
@popd
