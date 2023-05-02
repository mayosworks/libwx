@echo off
REM ===========================================================================
REM 一時ファイル＆一時ディレクトリ削除バッチ
REM ===========================================================================

echo.一時ファイルを削除します。中止するときは、Ctrl+Cで終了してください。
pause

REM このバッチファイルのあるディレクトリへ移動
cd /d %~dp0
cd ..

REM ビルド時に生成される一時ディレクトリを消す
rmdir /Q /S Debug
rmdir /Q /S Release
rmdir /Q /S Check
rmdir /Q /S ipch

REM VC++IDEやdoxygenなどが作る一時ファイルを消す
del /Q /F *.log
del /Q /F *.tmp
del /Q /F *.ncb
del /Q /F *.sdf
del /Q /F *.user
del /Q /F /A:H *.suo

REM doxygenが生成する一時ディレクトリを消す
cd doc
rmdir /Q /S html
del /Q /F *.log

pause
