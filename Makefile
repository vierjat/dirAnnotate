CPP = g++
CC = gcc
CFLAGS = -Wall
LIBS = 
GLIBS = 
GLIBS += 
OBJECTS = dirAnnotate.o 
HEADERS = 

ALL : dirAnnotate.exe
	echo "Listo!"

dirAnnotate.exe : $(OBJECTS)
	$(CPP) $(OBJECTS) -o dirAnnotate.exe $(LIBS) $(GLIBS) $(CFLAGS)

dirAnnotate.o : dirAnnotate.cc $(HEADERS)
	$(CPP) -c dirAnnotate.cc -o dirAnnotate.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
