CXX := g++

# Base flags (always present)
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 \
           -MMD -MP \
           -Iinclude

# try pkg-config first (common on Linux)
PKG_CFLAGS := $(shell pkg-config --cflags openssl 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs openssl 2>/dev/null)

# fallback for macOS Homebrew openssl@3
BREW_OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null)
BREW_CFLAGS := $(if $(BREW_OPENSSL_PREFIX), -I$(BREW_OPENSSL_PREFIX)/include)
BREW_LIBS   := $(if $(BREW_OPENSSL_PREFIX), -L$(BREW_OPENSSL_PREFIX)/lib -lcrypto)

# Append any discovered flags (do NOT overwrite CXXFLAGS)
CXXFLAGS += $(PKG_CFLAGS) $(BREW_CFLAGS)
LDFLAGS  := $(PKG_LIBS) $(BREW_LIBS) -lcrypto -lz

# Target name
TARGET := GitWork

# Sources
SRCS := $(wildcard src/*.cpp) main.cpp

# Objects / deps
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include dependency files; ignore missing ones
-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
