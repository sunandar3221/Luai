@echo off
setlocal enabledelayedexpansion
pushd lua-5.4.7\src
for %%f in (*.c) do (
    if not "%%f"=="lua.c" if not "%%f"=="luac.c" (
        zig cc -O2 -c %%f
    )
)
set "OBJS="
for %%o in (*.obj) do (
    set "OBJS=!OBJS! %%o"
)
zig ar rcs liblua.a !OBJS!
del /q *.obj
popd
zig c++ -O2 -std=c++17 src\lexer.cpp src\runtime.cpp src\repl.cpp src\main.cpp lua-5.4.7\src\liblua.a -o luai.exe
