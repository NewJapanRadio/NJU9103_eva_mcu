OBJDIR = build

TESTTOP = unittest.exe

TESTOBJ = unittest_dummy.o unittest1.o unittest_spi_command.o
DUT     = main.o dispatcher.o spi_command.o adc_data_buffer.o
MOCK    = timer_wrapper.o
OBJ     = gtest_main.o gtest-all.o gmock-all.o

CC = gcc
RM = rm -f
AR = ar
LD = g++

INCLUDE = -Igoogletest -Igoogletest/include -Igooglemock -Igooglemock/include -I../src -Imock
CFLAGS = -O0 -g -DUNIT_TEST
LDFLAGS = -fprofile-arcs -lstdc++

test : $(addprefix $(OBJDIR)/, $(TESTTOP))

clean :
	$(RM) $(addprefix $(OBJDIR)/, $(TESTTOP) $(TESTOBJ) $(DUT) $(MOCK) $(OBJ) *.gcno *.gcda *.gcov)

$(OBJDIR)/%.exe : $(addprefix $(OBJDIR)/, $(TESTOBJ) $(DUT) $(MOCK) $(OBJ))
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

$(OBJDIR)/%.o : googlemock/src/%.cc
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

