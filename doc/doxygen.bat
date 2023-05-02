@echo off
REM ===========================================================================
REM doxygenマニュアルドキュメント作成コマンド実行バッチ
REM ===========================================================================

REM このバッチファイルのあるディレクトリへ移動
cd /d %~dp0

echo.マニュアルを更新します。中止するときは、Ctrl+Cで終了してください。
pause

doxygen.exe doxygen.cfg

REM ヘルプファイルをこのフォルダへコピー
move /Y .\html\libwx.chm .\libwx.chm

echo.ドキュメントを更新しました。
pause
