all: video.o ARVideoSettingsController.o

video.o :
	g++ -c video.c

ARVideoSettingsController.o :
	g++ -c -fpascal-strings ARVideoSettingsController.m

clean:
	rm -rf *.o
