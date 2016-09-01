BINDIR   := bin
OBJDIR   := obj
SRCDIR   := src
SRCS     := $(wildcard $(SRCDIR)/*.cc)
OBJS     := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))
CXXFLAGS := -std=c++14 -Wall -Werror 
LDFLAGS  := -lGL -lpng

all: $(BINDIR)/main

$(BINDIR)/main: $(OBJS)
	@echo "[LD]  $@"
	@mkdir -p $(BINDIR)
	@$(CXX) $(LDFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@echo "[CXX] $<"
	@mkdir -p $(OBJDIR)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -vf bin/*

.PHONY: main clean
