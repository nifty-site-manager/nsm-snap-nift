#basic makefile for nsm-snap-nift
objects=nsm.o ConsoleColor.o DateTimeInfo.o Directory.o Expr.o ExprtkFns.o Filename.o FileSystem.o Getline.o GitInfo.o HashTk.o Lolcat.o LuaFns.o Lua.o NumFns.o Pagination.o Parser.o Path.o ProjectInfo.o Quoted.o StrFns.o SystemInfo.o Title.o TrackedInfo.o Variables.o WatchList.o
cppfiles=nsm.cpp ConsoleColor.cpp DateTimeInfo.cpp Directory.cpp Expr.cpp ExprtkFns.cpp Filename.cpp FileSystem.cpp Getline.cpp GitInfo.cpp hashtk/HashTk.cpp Lolcat.cpp LuaFns.cpp Lua.cpp NumFns.cpp Pagination.cpp Parser.cpp Path.cpp ProjectInfo.cpp Quoted.cpp StrFns.cpp SystemInfo.cpp Title.cpp TrackedInfo.cpp Variables.cpp WatchList.cpp

CXX?=g++
CXXFLAGS=-D__BUNDLED__ -std=c++11 -Wall -Wextra -pedantic -O3 -s
LINK=-pthread ./LuaJIT/src/libluajit.a -ldl

###

all: make-luajit nsm

###

make-luajit:
	cd LuaJIT && make

###

HashTk.o: hashtk/HashTk.cpp hashtk/HashTk.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

###

nsm: $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o nsm $(LINK)
	cp nsm nift

nsm.o: nsm.cpp GitInfo.o ProjectInfo.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ProjectInfo.o: ProjectInfo.cpp ProjectInfo.h GitInfo.o Parser.o WatchList.o Timer.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

GitInfo.o: GitInfo.cpp GitInfo.h ConsoleColor.o FileSystem.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Parser.o: Parser.cpp Parser.h DateTimeInfo.o Expr.o ExprtkFns.o Getline.o HashTk.o LuaFns.o Lua.o Pagination.o SystemInfo.o TrackedInfo.o Variables.o 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

WatchList.o: WatchList.cpp WatchList.h FileSystem.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Getline.o: Getline.cpp Getline.h ConsoleColor.o FileSystem.o Lolcat.o StrFns.o Consts.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Lolcat.o: Lolcat.cpp Lolcat.h FileSystem.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

LuaFns.o: LuaFns.cpp LuaFns.h Lua.o ConsoleColor.o ExprtkFns.o FileSystem.o Path.o Quoted.o Variables.o Consts.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Lua.o: Lua.cpp Lua.h StrFns.o LuaJIT/src/lua.hpp Lua-5.3/src/lua.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ExprtkFns.o: ExprtkFns.cpp ExprtkFns.h Expr.o FileSystem.o Quoted.o Variables.o Consts.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Expr.o: Expr.cpp Expr.h exprtk/exprtk.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

FileSystem.o: FileSystem.cpp FileSystem.h Path.o SystemInfo.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Pagination.o: Pagination.cpp Pagination.h Path.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

DateTimeInfo.o: DateTimeInfo.cpp DateTimeInfo.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TrackedInfo.o: TrackedInfo.cpp TrackedInfo.h Path.o Title.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Variables.o: Variables.cpp Variables.h NumFns.o Path.o StrFns.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

NumFns.o: NumFns.cpp NumFns.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Path.o: Path.cpp Path.h ConsoleColor.o Directory.o Filename.o SystemInfo.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

StrFns.o: StrFns.cpp StrFns.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Directory.o: Directory.cpp Directory.h Quoted.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Filename.o: Filename.cpp Filename.h Quoted.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

SystemInfo.o: SystemInfo.cpp SystemInfo.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Title.o: Title.cpp Title.h ConsoleColor.o Quoted.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ConsoleColor.o: ConsoleColor.cpp ConsoleColor.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Quoted.o: Quoted.cpp Quoted.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

###

install:
	chmod 755 nsm
	chmod 755 nift
	mv nift $(DESTDIR)
	mv nsm $(DESTDIR)

	mv LuaJIT/src/lauxlib.h $(DESTDIR)
	mv LuaJIT/src/lua.h $(DESTDIR)
	mv LuaJIT/src/lua.hpp $(DESTDIR)
	mv LuaJIT/src/luaconf.h $(DESTDIR)
	mv LuaJIT/src/lualib.h $(DESTDIR)

	mv LuaJIT/src/luajit $(DESTDIR)

uninstall:
	rm $(DESTDIR)/nift
	rm $(DESTDIR)/nsm

	rm $(DESTDIR)/lauxlib.h
	rm $(DESTDIR)/lua.h
	rm $(DESTDIR)/lua.hpp
	rm $(DESTDIR)/luaconf.h
	rm $(DESTDIR)/lualib.h

	rm $(DESTDIR)/luajit

clean:
	rm -f $(objects) exprtk.o
	cd LuaJIT && make clean

clean-all:
	rm -f $(objects) exprtk.o nift nsm
	cd LuaJIT && make clean
