
@set CURRENT_DIR=%~dp0

pushd %CURRENT_DIR%


powershell -NoProfile -ExecutionPolicy Unrestricted .\Setup.ps1

popd
pause

