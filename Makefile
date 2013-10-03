# Copyright Stephan T. Lavavej, http://nuwen.net .
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# http://boost.org/LICENSE_1_0.txt .

ifeq (,$(MSVC)) # GCC

WARNINGS = -Wall -Wextra -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align \
	-Wwrite-strings -Wconversion -fno-nonansi-builtins -Wold-style-cast -Woverloaded-virtual

OPTIMIZATIONS = -s -O3 -fomit-frame-pointer -ffast-math -ftracer -maccumulate-outgoing-args

BZIP2 = -lbz2
JPEG = -ljpeg
REGEX = -lboost_regex
ZLIB = -lz

ifeq (,$(findstring Windows,$(OS))) # GNU/Linux GCC

# My system is weird.
HACK = -isystem /home/stl/gcc-4.1.1/include -L/home/stl/gcc-4.1.1/lib

MEMORY = $(REGEX)
MWINDOWS =
WINSOCK =

else # MinGW GCC

MEMORY = -lpsapi
MWINDOWS = -mwindows
WINSOCK = -lws2_32

endif

else # MSVC

BATCH_FILE = "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
LIB_DIR = C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Lib

ifeq (,$(DEBUG))
OPTIMIZATIONS = /MT /GL /O2
BZIP2 = "$(LIB_DIR)\bzip2.lib"
JPEG = "$(LIB_DIR)\jpeg.lib"
ZLIB = "$(LIB_DIR)\zlib.lib"
else
OPTIMIZATIONS = /MTd /Fd$(@:.exe=.pdb) /Zi
BZIP2 = "$(LIB_DIR)\bzip2d.lib"
JPEG = "$(LIB_DIR)\jpegd.lib"
ZLIB = "$(LIB_DIR)\zlibd.lib"
endif

WARNINGS = /Wall /wd4127 /wd4512 /wd4514 /wd4571 /wd4625 /wd4626 /wd4640 /wd4710 /wd4711 /wd4820 /wd4996

# 4127 - while (true) is a constant conditional expression.
# 4512 - Can't implicitly define copy assignment operator due to 12.8/12.
# 4514 - Inlining.
# 4571 - catch (...) does the right thing.
# 4625 - boost::noncopyable does the right thing.
# 4626 - boost::noncopyable does the right thing.
# 4640 - Local static objects aren't friendly to multithreading.
# 4710 - Inlining.
# 4711 - Inlining.
# 4820 - Padding.
# 4996 - __declspec(deprecated) is useless.

MEMORY = "$(LIB_DIR)\psapi.lib"
MWINDOWS = /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
REGEX =
WINSOCK = "$(LIB_DIR)\ws2_32.lib"

endif


ifeq (,$(findstring Windows,$(OS))) # Unix

AND_ALWAYS_SUCCEED = ; true
AND_ALWAYS_FAIL = ; false
IF_EXIST_NUKE = rm -f FILE

else # Windows

AND_ALWAYS_SUCCEED = & echo x > NUL
AND_ALWAYS_FAIL = & grep x NUL
IF_EXIST_NUKE = if exist FILE del FILE

endif


.PRECIOUS: %_test.exe

.PHONY: all clean poison

all: $(subst .cc,.exe,$(wildcard *.cc))

clean:
	@$(IF_EXIST_NUKE:FILE=*.exe)
	@$(IF_EXIST_NUKE:FILE=*.obj)
	@$(IF_EXIST_NUKE:FILE=*.pdb)
	@$(IF_EXIST_NUKE:FILE=*.suo)
	@$(IF_EXIST_NUKE:FILE=*.tmp)

poison:
	@grep -v POISON_OK *.cc *.hh | grep -E "\<(signed|unsigned|short|long)\>|string::value_type|FIXME" $(AND_ALWAYS_SUCCEED)


%: %_test.exe ;

bwt_test.exe: INCANTATIONS += $(MEMORY)
bzip2_test.exe: INCANTATIONS += $(BZIP2)
cgi_test.exe: INCANTATIONS += $(REGEX)
daemon_test.exe: FINAL_INCANTATIONS += $(MWINDOWS)
jpeg_test.exe: INCANTATIONS += $(JPEG)
memory_test.exe: INCANTATIONS += $(MEMORY)
sha256_test.exe: INCANTATIONS += $(REGEX)
socket_client_test.exe: INCANTATIONS += $(WINSOCK)
socket_server_test.exe: INCANTATIONS += $(WINSOCK)
string_test.exe: INCANTATIONS += $(REGEX)
zlib_test.exe: INCANTATIONS += $(ZLIB)

$(subst .cc,.exe,$(wildcard *.cc)): %.exe: %.cc $(wildcard *.hh)
ifeq (,$(MSVC)) # GCC
	@$(CXX) $(HACK) $(WARNINGS) $(OPTIMIZATIONS) $< -o $@ $(INCANTATIONS) $(FINAL_INCANTATIONS)
else # MSVC
	@$(BATCH_FILE) > NUL && cl /EHsc /nologo $(OPTIMIZATIONS) $(WARNINGS) \
		/Tp$< $(INCANTATIONS) /link /OPT:REF,NOICF,NOWIN98 $(FINAL_INCANTATIONS) \
		> $(@:.exe=.tmp) || copy $(@:.exe=.tmp) $(@:.exe=_failed.tmp) > NUL
	@grep -vxE --color=never "$<|Generating code|Finished generating code" $(@:.exe=.tmp) $(AND_ALWAYS_SUCCEED)
	@$(IF_EXIST_NUKE:FILE=$(@:.exe=.obj))
	@$(IF_EXIST_NUKE:FILE=$(@:.exe=.tmp))
	@$(IF_EXIST_NUKE:FILE=$(@:.exe=_failed.tmp)) $(AND_ALWAYS_FAIL)
endif
