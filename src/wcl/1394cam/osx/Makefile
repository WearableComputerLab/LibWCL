CXX = g++ -Wall -g -O2

SOURCES = video.cpp

OFILES = $(SOURCES:.cpp=.o)

all: $(OFILES)

#all: ARVideoSettingsController.o

#ARVideoSettingsController.o :
#	g++ -c -fpascal-strings ARVideoSettingsController.m

# rule that calulates the prerequisites for any .cpp file
%.d: %.cpp
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, \1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SOURCES:.cpp=.d)

clean:
	rm -rf *.o
