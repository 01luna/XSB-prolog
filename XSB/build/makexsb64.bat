@echo off
REM   makexsb.bat
REM   Script for compiling XSB under Windows using VC++

set XSBCONFIGdir=..\config\x64-pc-windows

IF NOT EXIST %XSBCONFIGdir%\saved.o MKDIR %XSBCONFIGdir%\saved.o
IF NOT EXIST %XSBCONFIGdir%\bin mkdir %XSBCONFIGdir%\bin
IF NOT EXIST %XSBCONFIGdir%\lib mkdir %XSBCONFIGdir%\lib

@copy odbc\* %XSBCONFIGdir%
@copy windows64\banner.msg %XSBCONFIGdir%
@copy windows64\xsb_configuration.P %XSBCONFIGdir%\lib
@copy windows64\xsb_config.h %XSBCONFIGdir%
@copy windows64\xsb_config_aux.h %XSBCONFIGdir%
@copy windows64\xsb_debug.h %XSBCONFIGdir%

@cd ..\emu

REM Concatenate MSVC_mkfile.mak & MSVC.dep into emu\MSVC_mkfile.mak
@copy ..\build\windows64\MSVC_mkfile.mak+..\build\MSVC.dep MSVC_mkfile.mak


@nmake /nologo /f "MSVC_mkfile.mak" %1 %2 %3 %4 %5 %6 %7

@del MSVC_mkfile.mak

@cd ..\gpp
@nmake /nologo /s /f "MSVC_mkfile64.mak" %1 %2 %3 %4 %5 %6 %7

@cd ..\packages

@cd dbdrivers
@nmake /nologo /s /f NMakefile64.mak %1 %2 %3 %4 %5 %6 %7
@cd ..

REM build curl before sgml and xpath
REM @cd curl
REM @nmake /nologo /f NMakefile64.mak %1 %2 %3 %4 %5 %6 %7
REM @cd ..

REM @cd sgml\cc
REM @nmake /nologo /f NMakefile64.mak %1 %2 %3 %4 %5 %6 %7
REM @cd ..\..

REM @cd xpath\cc
REM @nmake /nologo /f NMakefile64.mak %1 %2 %3 %4 %5 %6 %7
REM @cd ..\..

REM We don't have win64 DLLs for PCRE yet
REM @cd pcre
REM @nmake /nologo /f NMakefile64.mak %1 %2 %3 %4 %5 %6 %7
REM @cd ..

@cd ..\build
