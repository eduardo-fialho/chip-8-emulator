@echo off
setlocal
pushd "%~dp0" || exit /b 1

where cmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake nao foi encontrado no PATH.
    echo Instale o CMake e deixe o comando cmake disponivel no terminal.
    set "RC=1"
    goto :finish
)

rem Escolhe explicitamente um gerador para nao herdar NMake de CMAKE_GENERATOR.
set "GENERATOR="
set "CXX_COMPILER="
set "COMPILER_FAMILY="
set "TOOLCHAIN_PREFIX="

rem Prioridade: MSYS2 UCRT64, que e comum neste projeto.
if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set "CXX_COMPILER=C:\msys64\ucrt64\bin\g++.exe"
    set "COMPILER_FAMILY=GNU"
    set "TOOLCHAIN_PREFIX=C:\msys64\ucrt64"
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
    if exist "C:\msys64\ucrt64\bin\ninja.exe" set "GENERATOR=Ninja"
    if not defined GENERATOR if exist "C:\msys64\ucrt64\bin\mingw32-make.exe" set "GENERATOR=MinGW Makefiles"
)

rem Caso UCRT64 nao esteja no caminho padrao, procura compiladores no PATH.
if not defined CXX_COMPILER (
    for /f "delims=" %%I in ('where g++.exe 2^>nul') do if not defined CXX_COMPILER set "CXX_COMPILER=%%I"
    if defined CXX_COMPILER set "COMPILER_FAMILY=GNU"
)

if not defined CXX_COMPILER (
    for /f "delims=" %%I in ('where clang++.exe 2^>nul') do if not defined CXX_COMPILER set "CXX_COMPILER=%%I"
    if defined CXX_COMPILER set "COMPILER_FAMILY=CLANG"
)

if not defined CXX_COMPILER (
    for /f "delims=" %%I in ('where cl.exe 2^>nul') do if not defined CXX_COMPILER set "CXX_COMPILER=%%I"
    if defined CXX_COMPILER set "COMPILER_FAMILY=MSVC"
)

rem Ninja pode trabalhar com GCC, Clang ou MSVC (no ambiente correto).
if not defined GENERATOR if defined CXX_COMPILER (
    where ninja.exe >nul 2>&1
    if not errorlevel 1 set "GENERATOR=Ninja"
)

rem Se nao houver Ninja, use MinGW Makefiles com GCC/Clang e mingw32-make.
if not defined GENERATOR if /I "%COMPILER_FAMILY%"=="GNU" (
    where mingw32-make.exe >nul 2>&1
    if not errorlevel 1 set "GENERATOR=MinGW Makefiles"
)
if not defined GENERATOR if /I "%COMPILER_FAMILY%"=="CLANG" (
    where mingw32-make.exe >nul 2>&1
    if not errorlevel 1 set "GENERATOR=MinGW Makefiles"
)

rem Em um Developer Command Prompt, NMake pode ser usado com MSVC.
if not defined GENERATOR if /I "%COMPILER_FAMILY%"=="MSVC" (
    where nmake.exe >nul 2>&1
    if not errorlevel 1 set "GENERATOR=NMake Makefiles"
)

rem Visual Studio so e alternativa quando o compilador MSVC foi detectado ou nao ha outro compilador.
rem Evita misturar um compilador MSYS2/GCC com bibliotecas SDL2 feitas para MSVC.
if not defined GENERATOR if /I "%COMPILER_FAMILY%"=="MSVC" call :detectVisualStudio
if not defined GENERATOR if not defined CXX_COMPILER call :detectVisualStudio

if not defined GENERATOR (
    echo ERROR: Nao encontrei uma combinacao de compilador e gerador suportada.
    echo.
    echo Opcoes reconhecidas pelo script:
    echo   - MSYS2 UCRT64 com g++ e Ninja ou mingw32-make
    echo   - g++ ou clang++ no PATH com Ninja ou mingw32-make
    echo   - MSVC no Developer Command Prompt com Ninja ou nmake
    echo   - Visual Studio com as ferramentas C++ instaladas
    echo.
    echo Instale as ferramentas que faltam ou adicione-as ao PATH e tente novamente.
    set "RC=1"
    goto :finish
)

echo Generator: %GENERATOR%
if defined CXX_COMPILER echo Compiler:  %CXX_COMPILER%
if defined TOOLCHAIN_PREFIX echo SDL2/toolchain prefix: %TOOLCHAIN_PREFIX%

rem Se uma tentativa anterior usou outro gerador, o cache nao pode ser reutilizado.
set "CACHED_GENERATOR="
if exist "build\CMakeCache.txt" (
    for /f "tokens=1,* delims==" %%A in ('findstr /B /C:"CMAKE_GENERATOR:INTERNAL=" "build\CMakeCache.txt" 2^>nul') do set "CACHED_GENERATOR=%%B"
)
if defined CACHED_GENERATOR if /I not "%CACHED_GENERATOR%"=="%GENERATOR%" (
    echo Existing build cache uses "%CACHED_GENERATOR%"; recreating build directory for "%GENERATOR%"...
    rmdir /s /q build
)

rem Configura com o gerador e, quando detectado, o compilador escolhidos acima.
if defined CXX_COMPILER (
    if defined TOOLCHAIN_PREFIX (
        cmake -S . -B build -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_CXX_COMPILER=%CXX_COMPILER%" "-DCMAKE_PREFIX_PATH=%TOOLCHAIN_PREFIX%"
    ) else (
        cmake -S . -B build -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_CXX_COMPILER=%CXX_COMPILER%"
    )
) else (
    cmake -S . -B build -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=Release
)
if errorlevel 1 (
    echo ERROR: Configuracao do CMake falhou.
    set "RC=1"
    goto :finish
)

cmake --build build --config Release
if errorlevel 1 (
    echo ERROR: Compilacao falhou.
    set "RC=1"
    goto :finish
)

set "EXE=build\bin\Release\chip8.exe"
if not exist "%EXE%" (
    echo ERROR: A compilacao terminou, mas o executavel nao foi encontrado: %EXE%
    set "RC=1"
    goto :finish
)

if "%~1"=="" goto :built
"%EXE%" "%~1"
set "RC=%ERRORLEVEL%"
goto :finish

:built
echo.
echo Build successful: %EXE%
echo To run a ROM, use: run.bat "roms\your-rom.ch8"
set "RC=0"

:finish
popd
exit /b %RC%


:detectVisualStudio
set "VS_MAJOR="
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "tokens=1 delims=." %%V in ('"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion') do set "VS_MAJOR=%%V"
)
if "%VS_MAJOR%"=="18" (
    cmake --help | findstr /C:"Visual Studio 18 2026" >nul 2>&1
    if not errorlevel 1 set "GENERATOR=Visual Studio 18 2026"
)
if "%VS_MAJOR%"=="17" set "GENERATOR=Visual Studio 17 2022"
if "%VS_MAJOR%"=="16" set "GENERATOR=Visual Studio 16 2019"
if "%VS_MAJOR%"=="15" set "GENERATOR=Visual Studio 15 2017"
if defined GENERATOR set "CXX_COMPILER="
exit /b 0
