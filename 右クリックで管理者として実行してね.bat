@echo off
@rem �T���v���̊e�t�H���_��k2Engine�ւ̃V���{���b�N�����N���쐬����B
@setlocal
@set CURRENT_DIR=%~dp0

pushd %CURRENT_DIR%

@call .\Sample\cleanup.bat
@call .\Sample\setup.bat

@popd
@echo �Z�b�g�A�b�v�I��

@pause