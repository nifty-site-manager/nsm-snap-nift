#basic makefile for nsm-snap
objects=nsm.o DateTimeInfo.o Directory.o Filename.o FileSystem.o PageBuilder.o PageInfo.o Path.o Quoted.o SiteInfo.o Title.o
cppfiles=nsm.cpp DateTimeInfo.cpp Directory.cpp Filename.cpp FileSystem.cpp PageBuilder.cpp PageInfo.cpp Path.cpp Quoted.cpp SiteInfo.cpp Title.cpp
CC=g++
LINK=-pthread
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -O3

nsm: $(objects)
	$(CC) $(CXXFLAGS) $(cppfiles) -o nsm $(LINK)
	$(CC) $(CXXFLAGS) $(cppfiles) -o nift $(LINK)

nsm.o: nsm.cpp SiteInfo.o Timer.h
	$(CC) $(CXXFLAGS) -c -o $@ $< $(LINK)

SiteInfo.o: SiteInfo.cpp SiteInfo.h FileSystem.o PageBuilder.o
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LINK)

FileSystem.o: FileSystem.cpp FileSystem.h Path.o
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LINK)

PageBuilder.o: PageBuilder.cpp PageBuilder.h DateTimeInfo.o PageInfo.o
	$(CC) $(CXXFLAGS) -c -o $@ $<

DateTimeInfo.o: DateTimeInfo.cpp DateTimeInfo.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

PageInfo.o: PageInfo.cpp PageInfo.h Path.o Title.o
	$(CC) $(CXXFLAGS) -c -o $@ $<

Path.o: Path.cpp Path.h Directory.o Filename.o
	$(CC) $(CXXFLAGS) -c -o $@ $<

Directory.o: Directory.cpp Directory.h Quoted.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

Filename.o: Filename.cpp Filename.h Quoted.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

Title.o: Title.cpp Title.h Quoted.o
	$(CC) $(CXXFLAGS) -c -o $@ $<

Quoted.o: Quoted.cpp Quoted.h
	$(CC) $(CXXFLAGS) -c -o $@ $<

install:
	chmod 755 nsm
	mv nift $(DESTDIR)
	mv nsm $(DESTDIR)

uninstall:
	rm $(DESTDIR)/nift
	rm $(DESTDIR)/nsm

clean:
	rm -f $(objects)

clean-all:
	rm -f $(objects) nift nsm

