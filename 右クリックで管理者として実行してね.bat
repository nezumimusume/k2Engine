@echo off
@rem サンプルの各フォルダにk2Engineへのシンボリックリンクを作成する。
@setlocal
@set CURRENT_DIR=%~dp0

pushd %CURRENT_DIR%

@call .\Sample\cleanup.bat
@call .\Sample\setup.bat

@popd
@echo セットアップ終了

@pause