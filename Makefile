#Compiler and Linker
CC          := gcc

#Name of executable
TARGET      := deposit_slip

#Source, header, object, and target directories
SRCDIR      := .
INCDIR      := headers
OBJDIR      := obj
TARGETDIR   := bin
SRCEXT      := c
OBJEXT      := o
INSTALLDIR  := /usr/local/bin/
#Flags, Libraries and Includes
CFLAGS      := -g -Wall `pkg-config --cflags json-glib-1.0` `pkg-config --cflags glib-2.0` `pkg-config --cflags gtk+-3.0`
LFLAGS         := `pkg-config --libs glib-2.0` `pkg-config --libs json-glib-1.0` `pkg-config --libs gtk+-3.0` -lm
INC          := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
#Find all source files
SOURCES     := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
#Create object file names from the source file names; uses nested pattern substitution
OBJECTS     := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: directories $(TARGET)

# Make directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(OBJDIR)

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LFLAGS)


#Compile
$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

#Clean objects
clean:
	@$(RM) -rf $(OBJDIR)

# Install
install: $(TARGETDIR)/$(TARGET)
	install -p $(TARGETDIR)/$(TARGET) $(INSTALLDIR)


.PHONY: clean
