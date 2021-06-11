spell.exe: spellcheckMain.o hash.o
	g++ -o spell.exe spellcheckMain.o hash.o

spellcheckMain.o: spellcheckMain.cpp hash.h
	g++ -c spellcheckMain.cpp

hash.o: hash.cpp hash.h
	g++ -c hash.cpp

debug:
	g++ -g -o spellDebug.exe spellcheckMain.cpp hash.cpp

clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.cpp backups
	cp *.h backups
