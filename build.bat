@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo  Membangun Luai (Lua Dialek Indonesia) dengan LuaJIT
echo ========================================================

if not exist luajit\src\libluajit.a (
    echo [1/2] Mengompilasi LuaJIT 2.1 static library...
    pushd luajit\src
    if exist "%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin\mingw32-make.exe" (
        "%LOCALAPPDATA%\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin\mingw32-make.exe" TARGET_SYS=Windows BUILDMODE=static XCFLAGS=-DLUAJIT_ENABLE_LUA52COMPAT libluajit.a
    ) else (
        mingw32-make TARGET_SYS=Windows BUILDMODE=static XCFLAGS=-DLUAJIT_ENABLE_LUA52COMPAT libluajit.a
    )
    popd
) else (
    echo [1/2] Library statis LuaJIT sudah tersedia di luajit\src\libluajit.a.
)

echo [2/2] Mengompilasi Interpreter Luai (luai.exe)...
g++ -static -O2 -std=c++17 -Iluajit\src src\lexer.cpp src\runtime.cpp src\repl.cpp src\json_module.cpp src\http_module.cpp src\main.cpp luajit\src\libluajit.a -lwinhttp -o luai.exe

if %errorlevel% equ 0 (
    echo [Sukses] luai.exe berhasil dibangun!
    if exist luai-vscode\bin (
        copy /y luai.exe luai-vscode\bin\luai.exe >nul
    )
) else (
    echo [Gagal] Terjadi kesalahan saat kompilasi luai.exe.
    exit /b %errorlevel%
)
