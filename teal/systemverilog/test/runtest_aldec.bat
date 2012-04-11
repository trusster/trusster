
call "%VSCOMNTOOLS%vsvars32.bat"
set PLATFORM=win32

set testname=%1

rem Visual C/C++ compilation
rem for vpi (2.0) compilation...
rem cl -c -I"%aldec_home%\pli\interface" -I.. -DALDEC -DWIN32=1 -MTd -Dvpi_2_0 -EHsc ../dictionary.cpp ../trandom.cpp ../vreg.cpp ../reg.cpp ../synch.cpp ../memory.cpp ../vout.cpp ../TEST/%testname%.cpp 

rem for pli (1.0) comiliation
cl -c -I"%aldec_home%\pli\interface" -I.. -DALDEC -MTd   -D_DEBUG -D_USRDLL -Z7 -RTC1 -DWIN32 -EHsc ../synch.cpp ../dictionary.cpp ../trandom.cpp ../vreg.cpp ../reg.cpp  ../memory.cpp ../vout.cpp ../TEST/%testname%.cpp 


 link -dll -DEBUG -export:veriusertfs dictionary.obj trandom.obj vreg.obj reg.obj vout.obj synch.obj memory.obj %testname%.obj i:\pthreads-2004-11-22\pre-built\lib\pthreadVC1.lib "%aldec_home%\pli\lib\aldecpli.lib" -out:%testname%.dll
rem link -dll -export:vlog_startup_routines dictionary.obj trandom.obj vreg.obj reg.obj vout.obj synch.obj memory.obj %testname%.obj i:\pthreads-2004-11-22\pre-built\lib\pthreadVC1.lib "%aldec_home%\pli\lib\aldecpli.lib" -out:%testname%.dll

rem del /f /q /s work*
call "%aldec_home%\etc\setenv.bat"
vlib work
vlog -work work +accr+top. ../test/%testname%.v

rem echo do aldec.do %testname% > meta_aldec.do
del meta_aldec.do
echo set user_pli %testname%.dll >> meta_aldec.do
echo set worklib work  >> meta_aldec.do
echo vsim top +out_file+%testname%_results.txt >> meta_aldec.do
echo run -all >> meta_aldec.do
echo quit >> meta_aldec.do


rem vsim  top -pli %testname%.dll +out_file+%testname%_results.txt
vsimsa meta_aldec.do
