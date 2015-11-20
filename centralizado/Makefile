CC = clang++ -std=c++11

pageRank: pageRank.o
	$(CC) -o pageRank pageRank.o

pageRank.o: pageRank.cc
	$(CC) -c pageRank.cc

clean:
	rm -f pageRank.o pageRank
