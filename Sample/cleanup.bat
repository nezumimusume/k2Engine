
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

@popd

@exit /b


:end
@popd

