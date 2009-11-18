REM init variables
set BF_NOW=%DATE:~9,4%-%DATE:~6,2%-%DATE:~3,2%_%TIME:~0,2%-%TIME:~3,2%
set BF_DEST=..\wxPTB_generated_release
set BF_SRC=..\..\trunk

REM create directories
mkdir %BF_DEST%\wxPTB
mkdir %BF_DEST%\wxPTB\graphic
mkdir %BF_DEST%\wxPTB_src

REM copy files for release package
cd %BF_DEST%\wxPTB
copy %BF_SRC%\BUILD .
copy %BF_SRC%\README .
copy %BF_SRC%\INSTALL .
copy %BF_SRC%\LICENSE .
copy %BF_SRC%\HISTORY .
copy %BF_SRC%\CREDITS .
copy %BF_SRC%\CHANGELOG .
copy %BF_SRC%\HELP .
copy %BF_SRC%\wxPTB_win32_rs.exe wxPTB.exe

copy %BF_SRC%\graphic\logo.ico .\graphic

REM copy files for source package
cd ..\wxPTB_src
xcopy %BF_SRC%\* . /E /V

rmdir .out /S /Q
rmdir .svn /S /Q
del wxPTB.log /Q
del wxPTB.conf /Q
del wxPTB.layout /Q
del wxPTB.depend /Q
del wxPTB.exe /Q
del ptb.sig /Q
del test.bat /Q

cd ..

REM === pack ===


REM source package with 7z
7z a -t7z %1_src.7z -mx=9 wxPTB_src

REM source package with zip
7z a -tzip %1_src.zip -mx=9 wxPTB_src

REM win32-binary package with 7z
7z a -t7z %1_win32.7z -mx=9 wxPTB

REM win32-binary package with zip
7z a -tzip %1_win32.zip -mx=9 wxPTB
