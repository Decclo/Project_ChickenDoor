# define the C compiler to use
CC = abr-gcc

# define any compile-time flags
CFLAGS= -Os -DF_CPU=16000000UL -mmcu=atmega328p

# define any directories containing header files other than /usr/include
INCLUDES = -I/usr/local/include/

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
#LFLAGS = -L/home/newhall/lib  -L../lib
LFLAGS = -L/usr/local/lib/

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
#LIBS = -lmylib -lm
LIBS =

# define the C source files
SRCS = ./main.cpp

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
OBJS = $(SRCS:.c=.o)

# define port on which the microprocessor is located:
PORT= /dev/ttyUSB0

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all: $(MAIN)
	@echo  == Compilation Finished ==
	

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
#%.c: %.o
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
%.o: %.c
	${CC} ${CFLAGS} -c $<

clean:
	$(RM) ./src/*.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
