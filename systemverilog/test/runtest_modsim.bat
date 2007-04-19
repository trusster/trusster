set PLATFORM=win32pe

set testname=%1

rem Visual C/C++ compilation
rem cl -c -I"%MTI_HOME%\include" -I.. -DSIM=mti -DWIN32=1 -Dvpi_2_0 -EHsc ../dictionary.cpp ../vrandom.cpp ../vreg.cpp ../reg.cpp ../synch.cpp ../memory.cpp ../vout.cpp ../TEST/%testname%.cpp 
cl -c -I"%MTI_HOME%\include" -I.. -MTd -Gm  -D_DEBUG -D_USRDLL  -RTC1 -DWIN32 -EHsc ../synch.cpp ../dictionary.cpp ../vrandom.cpp ../vreg.cpp ../reg.cpp  ../memory.cpp ../vout.cpp ../TEST/%testname%.cpp 


link -dll -export:veriusertfs dictionary.obj vrandom.obj vreg.obj reg.obj vout.obj synch.obj memory.obj %testname%.obj i:\pthreads-2004-11-22\pre-built\lib\pthreadVC1.lib "%MTI_HOME%\win32\mtipli.lib" -out:%testname%.dll
rem link -dll -DEBUG -export:vlog_startup_routines dictionary.obj trandom.obj vreg.obj reg.obj vout.obj synch.obj memory.obj %testname%.obj i:\pthreads-2004-11-22\pre-built\lib\pthreadVC1.lib "%MTI_HOME%\win32\mtipli.lib" -out:%testname%.dll

rem del /f /q /s work*
"%MTI_HOME%\%PLATFORM%\vlib" work
"%MTI_HOME%\%PLATFORM%\vlog" +acc=r+top. ../test/%testname%.v

"%MTI_HOME%\%PLATFORM%\vsim" -c -trace_foreign 1 -do ..\test\vsim.do top -pli %testname%.dll +out_file+%testname%_results.txt


