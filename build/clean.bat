@echo off
REM ===========================================================================
REM �ꎞ�t�@�C�����ꎞ�f�B���N�g���폜�o�b�`
REM ===========================================================================

echo.�ꎞ�t�@�C�����폜���܂��B���~����Ƃ��́ACtrl+C�ŏI�����Ă��������B
pause

REM ���̃o�b�`�t�@�C���̂���f�B���N�g���ֈړ�
cd /d %~dp0
cd ..

REM �r���h���ɐ��������ꎞ�f�B���N�g��������
rmdir /Q /S Debug
rmdir /Q /S Release
rmdir /Q /S Check
rmdir /Q /S ipch

REM VC++IDE��doxygen�Ȃǂ����ꎞ�t�@�C��������
del /Q /F *.log
del /Q /F *.tmp
del /Q /F *.ncb
del /Q /F *.sdf
del /Q /F *.user
del /Q /F /A:H *.suo

REM doxygen����������ꎞ�f�B���N�g��������
cd doc
rmdir /Q /S html
del /Q /F *.log

pause
