OBJDIR = build

TESTTOP = unittest1.exe unittest2.exe

TEST = unittest1.o unittest2.o
DUT  = main.o dispatcher.o spi_command.o adc_data_buffer.o
MOCK = rdyb_wrapper.o serial_wrapper.o spi_wrapper.o timer_wrapper.o 
OBJ  = testtop.o gtest-all.o

CC = gcc
RM = rm -f
AR = ar
LD = g++

INCLUDE = -Igoogletest -Igoogletest/include -I../src -Imock
CFLAGS = -O0 -g -DUNIT_TEST -DGTEST_OS_WINDOWS -D_WIN32_WINNT=0x0500
LDFLAGS = -fprofile-arcs -lstdc++

test : $(addprefix $(OBJDIR)/, $(TESTTOP))

clean :
	$(RM) $(addprefix $(OBJDIR)/, $(TESTTOP) $(TEST) $(DUT) $(MOCK) $(OBJ) *.gcno *.gcda *.gcov)

$(OBJDIR)/%.exe : $(addprefix $(OBJDIR)/, %.o $(DUT) $(MOCK) $(OBJ))
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(LD) $(LDFLAGS) $^ -o $@
	$@

$(OBJDIR)/%.o : googletest/src/%.cc
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o : %.cpp
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o : ../src/%.cpp
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o : mock/%.cpp
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

