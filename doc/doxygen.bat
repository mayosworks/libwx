@echo off
REM ===========================================================================
REM doxygen�}�j���A���h�L�������g�쐬�R�}���h���s�o�b�`
REM ===========================================================================

REM ���̃o�b�`�t�@�C���̂���f�B���N�g���ֈړ�
cd /d %~dp0

echo.�}�j���A�����X�V���܂��B���~����Ƃ��́ACtrl+C�ŏI�����Ă��������B
pause

doxygen.exe doxygen.cfg

REM �w���v�t�@�C�������̃t�H���_�փR�s�[
move /Y .\html\libwx.chm .\libwx.chm

echo.�h�L�������g���X�V���܂����B
pause
