@echo off
echo Searching for Visual Studio installation...
setlocal enableextensions enabledelayedexpansion

del k15_resize*.pdb

set PROJECT_NAME=win32_image_resize_sample
set C_FILE_NAME=win32_image_resize_sample.c

set DLL_NAME=k15_resize

set DLL_COMPILER_OPTIONS=/nologo /Od /TC /MTd /W3 /Gm- /Z7 /Fe!DLL_NAME!.dll /DRESIZE_DLL
set DLL_LINKER_OTPIONS=/PDB:!DLL_NAME!_%TIME:~0,2%_%TIME:~3,2%_%TIME:~6,2%.pdb /DLL /EXPORT:resizeFunction

set COMPILER_OPTIONS=/nologo /Od /TC /MTd /W3 /Gm- /Z7 /Fe!PROJECT_NAME!.exe
set LINKER_OPTIONS=/PDB:!PROJECT_NAME!.pdb

set CL_OPTIONS=!COMPILER_OPTIONS! /link !LINKER_OPTIONS!
set DLL_CL_OPTIONS=!DLL_COMPILER_OPTIONS! /link !DLL_LINKER_OTPIONS!

set FOUND_PATH=0
set VS_PATH=

::check whether this is 64bit windows or not
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=32BIT || set OS=64BIT

IF %OS%==64BIT set REG_FOLDER=HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\
IF %OS%==32BIT set REG_FOLDER=HKLM\SOFTWARE\Microsoft\VisualStudio\

::Go to end if nothing was found
IF %REG_FOLDER%=="" GOTO DECISION

::try to get get visual studio path from registry for different versions
FOR /l %%G IN (14, -1, 8) DO (
	set REG_PATH=%REG_FOLDER%%%G.0
	set REG_COMMAND=reg query !REG_PATH! /v InstallDir

	!REG_COMMAND! >nul 2>nul

	::if errorlevel is 0, we found a valid installDir
	if !errorlevel! == 0 (
		::issue reg command again but evaluate output
		FOR /F "skip=2 tokens=*" %%A IN ('!REG_COMMAND!') DO (
			set VS_PATH=%%A
			::truncate stuff we don't want from the output
			set VS_PATH=!VS_PATH:~24!
			set FOUND_PATH=1
			goto DECISION
		)
	)
)

:DECISION
::check if a path was found
IF !FOUND_PATH!==0 (
	echo Could not find valid Visual Studio installation.
) ELSE (
	echo Starting build process...
	set VCVARS_PATH="!VS_PATH!..\..\VC\vcvarsall.bat"
	set CL_PATH="!VS_PATH!..\..\VC\bin\cl.exe"
	set BUILD_COMMAND=!CL_PATH! !C_FILE_NAME! !CL_OPTIONS!
	set DLL_BUILD_COMMAND=!CL_PATH! !C_FILE_NAME! !DLL_CL_OPTIONS!
	call !VCVARS_PATH!
	call !BUILD_COMMAND!
	call !DLL_BUILD_COMMAND!
) 