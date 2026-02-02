CXX = clang++
CXXFLAGS = -std=c++17 -O2 -Wall
LDFLAGS = -lgit2

SRC = Prompt/main.cpp \
      Prompt/getOptions.cpp \
      Prompt/segmentGit.cpp \
      Prompt/segmentCwd.cpp \
      Prompt/segmentUser.cpp \
      Prompt/segmentMachine.cpp \
      Prompt/segmentError.cpp \
      Prompt/segmentShortError.cpp

TARGET = prompt

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
