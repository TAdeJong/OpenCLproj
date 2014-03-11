LINK=link /nologo
CPP=cl /nologo
CPPFLAGS=$(MYINCLUDE) /EHsc $(MYLIBS)

MYLIBS="C:\Program Files (x86)\Intel\OpenCL SDK\3.0\lib\x86\OpenCL.lib"
MYINCLUDE=/I"C:\Program Files (x86)\Intel\OpenCL SDK\3.0\include"

all: helloworld.exe vectoradd.exe

clean:
	del *.obj *.exe