set QTDIR=E:\thuy_c++\Qt\5.12.0\msvc2017
set PATH=%QTDIR%\bin;%PATH%
set Qt5_DIR=%QTDIR%\lib\cmake\Qt5
mkdir build
cd build
rem D:\thuy_data\thuy_c++\cmake\bin\cmake [-G generator] ../
cmake [-G generator] ../
pause