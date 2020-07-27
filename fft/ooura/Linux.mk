CXXFLAGS := -fPIC -O3 -Wall -Werror -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
CXX := g++
MAJOR := 0
MINOR := 1
NAME := ooura
VERSION := $(MAJOR).$(MINOR)

lib: lib$(NAME).so.$(VERSION)

lib$(NAME).so.$(VERSION): $(NAME).o
	$(CXX) -shared $^ -o $@

clean:
	$(RM) *.o *.so*

