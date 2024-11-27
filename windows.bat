@echo off

set dir=%~dp0

cd %dir% && gcc ./src/lexer/lexer.c ./src/parser/parser.c ./src/main.c -o main.exe

if %errorlevel% equ 0 (
    cls && start cmd /k main.exe --file ./tests/T007.pas
    exit 0
) else (
    echo Erro ao compilar o programa.
    pause
    exit 1
)