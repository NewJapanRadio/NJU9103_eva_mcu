OBJDIR = build

TESTTOP = unittest.exe

TESTOBJ = unittest_dummy.o unittest_spi_command.o unittest_main.o unittest_uart.o unittest_expected_error.o
DUT     = main.o dispatcher.o spi_command.o adc_data_buffer.o
MOCK    = timer_wrapper.o
OBJ     = gtest_main.o gtest-all.o gmock-all.o

CC = gcc
RM = rm -f
AR = ar
LD = g++

INCLUDE = -Igoogletest -Igoogletest/include -Igooglemock -Igooglemock/include -I../src -Imock
CFLAGS = -O0 -g -DUNIT_TEST
LDFLAGS = -fprofile-arcs

test : $(addprefix $(OBJDIR)/, $(TESTTOP))

clean :
	$(RM) $(addprefix $(OBJDIR)/, $(TESTTOP) $(TESTOBJ) $(DUT) $(MOCK) $(OBJ) *.gcno *.gcda *.gcov)

report :
	lcov -c -d build -o build/coverage.info --rc lcov_branch_coverage=1
	genhtml build/coverage.info -o build/report --branch-coverage

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
	$(CC) $(CFLAGS) -coverage $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o : mock/%.cpp
	@if [ ! -d $(OBJDIR) ]; \
		then echo "mkdir -p $(OBJDIR)"; mkdir -p $(OBJDIR); \
		fi
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

