PACKAGE = simulator

CXX=g++
CXXFLAGS=-O3 -std=c++11 -Wall

SRCDIR := ./src
RSLDIR := ./result
OBJDIR := ./obj

BINNAME := $(PACKAGE)
TARGET	:= $(BINNAME)

SOURCES := $(wildcard $(SRCDIR)/*.cpp) 
HEADERS := $(wildcard $(SRCDIR)/*.hpp)

# OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o))) 
OBJECTS := $(SOURCES:%.cpp=%.o)

FILES     := $(HEADERS) $(SOURCES) $(MAKEFILE_LIST)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	@$(RM) $(OBJECTS) $(TARGET) *.bak *~ core* GTAGS GSYMS GRTAGS GPATH

distclean: clean
	-$(RM) $(RSLDIR)/* 

depend:
	makedepend -Y -f $(MAKEFILE_LIST) -- $(CXXFLAGS) -- $(SOURCES)

tags: GTAGS
GTAGS: $(SOURCES) $(HEADERS)
	gtags -v

.PHONY: clean distclean
.PHONY: depend
.PHONY: tags

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: router.hpp network.hpp 
network.o: network.hpp router.hpp 
router.o: router.hpp network.hpp trietree.hpp
trietree.o: trietree.hpp
