
@tkmOptimizer.exe %1 %1
@if %errorlevel%==1 goto ERR
@goto SUCCESS

@:ERR
@echo tkmファイルの最適化に失敗。

@goto END

@:SUCCESS
@echo tkmファイルの保存成功しました。
@:END

pause