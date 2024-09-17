CPPFLAGS = $(shell llvm-config-12 --cxxflags) -fexceptions -std=c++17
LLFLAGS = -ltbb -lgc -lpugixml $(shell llvm-config-12 --cxxflags --ldflags --libs all)
INCLUDES = -I/opt/intel/oneapi/vtune/latest/include -I/opt/intel/oneapi/tbb/latest/include

src_files := $(shell find ./ -type f -name '*.cpp')
objects := $(patsubst %.cpp,%.o,$(src_files))
debugobjects := $(patsubst %.cpp,%.debug.o,$(src_files))



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
	-find . -name '*.o' -delete
	-rm nom
	-find . -name '*.d' -delete

%.o : %.cpp
	clang++-12 $(CPPFLAGS) $(INCLUDES) -O3 -c $< -o $@

%.debug.o : %.cpp
	clang++-12 $(CPPFLAGS) $(INCLUDES) -g -O0 -c $< -o $@


sources = $(*.cpp)

deps = $(patsubst %.cpp,%.d,$(src_files))

headers: $(deps)

%.d: %.cpp
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $(INCLUDES) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 sed 's,\($*\)\.debug.o[ :]*,\1.debug.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$


-include $(deps)

