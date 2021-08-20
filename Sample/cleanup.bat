
@rem クリーンアップ
@setlocal
@echo off
@set CURRENT_DIR=%~dp0
@pushd %CURRENT_DIR%
@for /d %%a in (.\*) do (call :deleteSymbolicLink "%%a")
@goto end

@rem シンボリックリンクの削除。
:deleteSymbolicLink
@set SAMPLE_FOLDER=%~1
pushd %~1

@rmdir /s /q k2Engine
@rmdir /s /q k2EngineLow
@del k2Engine
@del k2EngineLow
@rmdir /s /q Game\\Assets\\shader
@rmdir /s /q Game\\Assets\\modelData\\preset
@del /s /q Game\\Assets\\shader
@del /s /q Game\\Assets\\modelData\\preset

@popd

@exit /b


:end
@popd

