SHELL         = bash
BASEDIR       = $(shell pwd)
H4DQMSRCDIR   = ../H4DQM/src
SRCDIR        = src
EXEDIR        = test
LIBDIR        = lib
BINDIR        = bin
LIBRARY       = libFastTimeTestBeamAnalysis

SRCFILES=$(wildcard $(SRCDIR)/*.cc)
OBJFILES=$(patsubst %.cc,%.o,$(SRCFILES)) 
SRCFILES += $(H4DQMSRCDIR)/Waveform.cpp $(H4DQMSRCDIR)/WaveformFit.cpp $(H4DQMSRCDIR)/WaveformUtils.cpp
OBJFILES += $(H4DQMSRCDIR)/Waveform.o   $(H4DQMSRCDIR)/WaveformFit.o   $(H4DQMSRCDIR)/WaveformUtils.o

USERINCLUDES += -I$(BASEDIR) -I../H4DQM
EXTERNALLIBS = `root-config --glibs` -lMathCore -lMathMore -lz -lm 
CXX          = g++
CXXFLAGS     = -c -g -Wall `root-config --cflags` -fPIC -O2 -ggdb -std=gnu++0x 
CXXFLAGS    += $(USERINCLUDES)

all: info clean prepare $(OBJFILES) 
	$(CXX) -shared -fPIC -o $(LIBDIR)/$(LIBRARY).so $(SRCDIR)/*.o $(H4DQMSRCDIR)/*.o $(EXTERNALLIBS)

info:
	@echo "--------------------------"
	@echo "Compiling @ $(BASEDIR)"
	@echo "Source are: $(SRCFILES)"
	@echo "--------------------------"

clean:
	$(shell	rm $(SRCDIR)/*.o)
	$(shell	rm $(H4DQMSRCDIR)/*.o)
	$(shell	rm $(LIBDIR)/*.o)
	$(shell	rm $(BINDIR)/*)

prepare:
	$(shell mkdir -p $(LIBDIR))
	$(shell mkdir -p $(BINDIR))

$(OBJFILES): | obj

obj:
	@mkdir -p $@

obj/%.o : %.cc
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@


#gcc -o libfoo.so module1.o module2.o -shared
#$(LIBOBJS): $(LIBCPP)
#	$(CXX) $(CXXFLAGS) -c $(LIBCPP) -o $(LIBOBJS)

#$(LIBRARY): $(LIBOBJS)
#	ar -cr $(LIBRARY) $(LIBOBJS)



