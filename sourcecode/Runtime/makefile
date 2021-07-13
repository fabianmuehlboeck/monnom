CPPFLAGS = $(shell llvm-config-12 --cxxflags) -fexceptions -std=c++17
LLFLAGS = -ltbb -lgc -lpugixml $(shell llvm-config-12 --cxxflags --ldflags --libs all)
INCLUDES = -I/opt/intel/oneapi/vtune/latest/include -I/opt/intel/oneapi/tbb/latest/include

VPATH = .:./instructions

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst instructions/%.cpp,%.o,$(wildcard instructions/*.cpp))
debugobjects := $(patsubst %.cpp,%.debug.o,$(wildcard *.cpp)) $(patsubst instructions/%.cpp,%.debug.o,$(wildcard instructions/*.cpp))


.DEFAULT: nombuild
.PHONY: nombuild
nombuild: headers nom

.PHONY: rebuild
rebuild: clean nombuild

nom: $(objects)
	clang++-12 -Xlinker --export-dynamic -O3 -o nom $(CPPFLAGS) $(LLFLAGS) $^ /opt/intel/oneapi/vtune/latest/lib64/libjitprofiling.a

nomdebug: $(debugobjects)
	clang++-12 -Xlinker --export-dynamic -g -O0 -o nom $(CPPFLAGS) $(LLFLAGS) $^ /opt/intel/oneapi/vtune/latest/lib64/libjitprofiling.a


.PHONY: clean
clean:
	-rm *.o
	-rm nom
	-rm *.d

%.o : %.cpp
	clang++-12 $(CPPFLAGS) $(INCLUDES) -O3 -c $< -o $@

%.debug.o : %.cpp
	clang++-12 $(CPPFLAGS) $(INCLUDES) -g -O0 -c $< -o $@


sources = $(*.cpp)

deps = $(patsubst %.cpp,%.d,$(wildcard *.cpp)) $(patsubst instructions/%.cpp,%.d,$(wildcard instructions/*.cpp))

headers: $(deps)

%.d: %.cpp
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $(INCLUDES) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 sed 's,\($*\)\.debug.o[ :]*,\1.debug.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$


-include $(deps)

