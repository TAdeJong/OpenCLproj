LINK=link /nologo
CPP=cl /nologo
CPPFLAGS=$(MYINCLUDE) /EHsc $(MYLIBS) /W3 /O2

MYLIBS="C:\Program Files (x86)\Intel\OpenCL SDK\3.0\lib\x86\OpenCL.lib"
MYINCLUDE=/I"C:\Program Files (x86)\Intel\OpenCL SDK\3.0\include"

all: helloworld.exe vectoradd.exe es.exe

es.exe: es.cpp es_common.h

clean:
	del *.obj *.exe
