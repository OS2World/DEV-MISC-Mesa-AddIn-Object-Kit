# makefile
# Created by IBM WorkFrame/2 MakeMake at 17:06:24 on 15 Oct 1995
#
# The actions included in this make file are:
#  Compile::Resource Compiler
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind

.SUFFIXES: .RC .cpp .obj .res 

.all: \
    f:\cprogs\msolv3\mesasolv.dll

.RC.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /DM2Z /Gm /Ge- /G5 /Fo"%|dpfF.obj" /C %s

f:\cprogs\msolv3\mesasolv.dll: \
    f:\cprogs\msolv3\range.obj \
    f:\cprogs\msolv3\solver.obj \
    f:\cprogs\msolv3\AddIn.obj \
    f:\cprogs\msolv3\MESASOLV.obj \
    f:\cprogs\msolv3\model.obj \
    f:\cprogs\msolv3\MESASOLV.res \
    {$(LIB)}mesasolv.def
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" "
     /Fef:\cprogs\msolv3\mesasolv.dll 
     mesasolv.def
     f:\cprogs\msolv3\range.obj
     f:\cprogs\msolv3\solver.obj
     f:\cprogs\msolv3\AddIn.obj
     f:\cprogs\msolv3\MESASOLV.obj
     f:\cprogs\msolv3\model.obj
<<
    rc.exe f:\cprogs\msolv3\MESASOLV.res f:\cprogs\msolv3\mesasolv.dll
    copy *.dll f:\apps\mesa2

f:\cprogs\msolv3\MESASOLV.res: \
    f:\cprogs\msolv3\MESASOLV.RC \
    {$(INCLUDE)}mesasolv.h

f:\cprogs\msolv3\range.obj: \
    f:\cprogs\msolv3\range.cpp \
    {f:\cprogs\msolv3;$(INCLUDE);}range.h

f:\cprogs\msolv3\model.obj: \
    f:\cprogs\msolv3\model.cpp \
    {f:\cprogs\msolv3;$(INCLUDE);}range.h \
    {f:\cprogs\msolv3;$(INCLUDE);}model.h \
    {f:\cprogs\msolv3;$(INCLUDE);}extaddin.h 

f:\cprogs\msolv3\MESASOLV.obj: \
    f:\cprogs\msolv3\MESASOLV.CPP \
    {f:\cprogs\msolv3;$(INCLUDE);}range.h \
    {f:\cprogs\msolv3;$(INCLUDE);}model.h \
    {f:\cprogs\msolv3;$(INCLUDE);}solver.h \
    {f:\cprogs\msolv3;$(INCLUDE);}addin.h \
    {f:\cprogs\msolv3;$(INCLUDE);}extaddin.h \
    {f:\cprogs\msolv3;$(INCLUDE);}mesasolv.h 

f:\cprogs\msolv3\AddIn.obj: \
    f:\cprogs\msolv3\AddIn.cpp \
    {f:\cprogs\msolv3;$(INCLUDE);}addin.h \
    {f:\cprogs\msolv3;$(INCLUDE);}extaddin.h 

f:\cprogs\msolv3\solver.obj: \
    f:\cprogs\msolv3\solver.cpp \
    {f:\cprogs\msolv3;$(INCLUDE);}range.h \
    {f:\cprogs\msolv3;$(INCLUDE);}model.h \
    {f:\cprogs\msolv3;$(INCLUDE);}solver.h
