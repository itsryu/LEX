@echo off

set dir=%~dp0

cd %dir% && gcc ./lexer.c ./parser.c ./main.c -o main.exe

if %errorlevel% equ 0 (
    cls && start cmd /k main.exe --file ./tests/T003.pas
    exit 0
) else (
    echo Erro ao compilar o programa.
    pause
    exit 1
)