
@tkmOptimizer.exe %1 %1
@if %errorlevel%==1 goto ERR
@goto SUCCESS

@:ERR
@echo tkm�t�@�C���̍œK���Ɏ��s�B
@echo tkm�t�@�C���̕ۑ��Ɏ��s���܂����B

@goto END

@:SUCCESS
@echo tkm�t�@�C���̕ۑ��������܂����B
@:END

pause