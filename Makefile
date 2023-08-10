CC = g++ -g -std=c++11 

travelMonitorClient: main.o travelRequest.o helpers.o person.o skip-list.o linkedList.o hashTable.o bloom-filter.o date.o

	$(CC)   -o travelMonitorClient helpers.o travelRequest.o main.o person.o skip-list.o linkedList.o hashTable.o bloom-filter.o date.o

main.o: main.cpp helpers.cpp

	$(CC) -g -c main.cpp helpers.cpp 

cyclicBuffer.o: cyclicBuffer.cpp helpers.cpp

	$(CC) -g -c cyclicBuffer.cpp helpers.cpp

helpers.o: helpers.cpp skip-list.cpp linkedList.cpp person.cpp hashTable.cpp bloom-filter.cpp date.cpp person.cpp 

	$(CC)   -c helpers.cpp skip-list.cpp person.cpp linkedList.cpp hashTable.cpp bloom-filter.cpp date.cpp person.cpp

skip-list.o: skip-list.cpp linkedList.cpp person.cpp bloom-filter.cpp date.cpp 

	$(CC)   -c skip-list.cpp linkedList.cpp person.cpp bloom-filter.cpp date.cpp

linkedList.o: linkedList.cpp person.cpp bloom-filter.cpp

	$(CC)   -c linkedList.cpp person.cpp bloom-filter.cpp

hashTable.o: hashTable.cpp person.cpp linkedList.cpp

	$(CC)   -c hashTable.cpp person.cpp bloom-filter.cpp

person.o: person.cpp date.cpp

	$(CC)   -c person.cpp date.cpp

bloom-filter.o: bloom-filter.cpp

	$(CC)   -c bloom-filter.cpp

date.o: date.cpp

	$(CC)   -c date.cpp

travelRequest.o: travelRequest.cpp date.cpp

	$(CC)	-c travelRequest.cpp date.cpp

monitorServer.o: monitorServer.cpp hashTable.cpp person.cpp linkedList.cpp bloom-filter.cpp skip-list.cpp date.cpp

	$(CC) -c monitorServer.cpp hashTable.cpp person.cpp linkedList.cpp bloom-filter.cpp skip-list.cpp date.cpp

clean: 

	rm -f *.o

run:
	./travelMonitorClient -m 5 -b 20 -c 9 -s 100000 -i Directory -t 5
	

monitorServer: monitorServer.o hashTable.o person.o cyclicBuffer.o linkedList.o bloom-filter.o skip-list.o date.o helpers.o

	$(CC) -o monitorServer monitorServer.o hashTable.o person.o cyclicBuffer.o linkedList.o bloom-filter.o skip-list.o date.o helpers.o -lpthread

bash:
	: > inputFile.txt
	./testFile.sh countriesFile.txt virusesFile.txt 500 NO

create_infile:
	./create_infile.sh inputFile.txt Directory 5
	

