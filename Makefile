# Compiler and flags
CXX=g++
CXXFLAGS=-std=c++17
DEPFLAGS=-MM

# Directories
SRCDIR=src
INCDIR=include
BUILDDIR=build
DEPDIR=.deps

TARGET=drcc

SOURCES=$(shell find $(SRCDIR) -name '*.cpp')
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
DEPFILES=$(SOURCES:$(SRCDIR)/%.cpp=$(DEPDIR)/%.d)


INCLUDES:=-I$(INCDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(DEPDIR)/%.d
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# $(DEPFILES): 
# 	mkdir -p $@

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	$(CXX) $(DEPFLAGS) $(INCLUDES) $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# include $(wildcard $(DEPFILES))

clean:
	rm -rf $(BUILDDIR) $(DEPDIR) $(TARGET)

.PHONY: all clean