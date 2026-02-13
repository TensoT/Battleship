CC=g++



CFLAGS=$(shell pkg-config --cflags opencv)

LIBS=$(shell pkg-config --libs opencv)



OBJS= main.o   TASK3.o SHA256.o SIMPLESOCKET.o

DEMOTARGET=main server client client_random client_pattern client_hunt




client.o:   client.C

	$(CC) -c $<  -std=c++11



client_random.o:    client_random.C

	$(CC) -c $<  -std=c++11



client_pattern.o:   client_pattern.c

	$(CC) -c $<  -std=c++11



client_hunt.o:  client_hunt.c

	$(CC) -c $<  -std=c++11



server.o:   server.C

	$(CC) -c $<  -std=c++11



SIMPLESOCKET.o: SIMPLESOCKET.C

	$(CC) -c $<  -std=c++11



SHA256.o:   SHA256.C

	$(CC) -c $<  -std=c++11



TASK3.o:    TASK3.C

	$(CC) -c $<  -std=c++11



main.o: main.C

	$(CC) -c $<  -std=c++11



main:   $(OBJS)

	$(CC) -o $@ $^ -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11 -lpthread $(LIBS)



server: server.o TASK3.o

	$(CC) -o server server.o TASK3.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11



client: client.o

	$(CC) -o client client.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11



client_random:  client_random.o

	$(CC) -o client_random client_random.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11



client_pattern: client_pattern.o

	$(CC) -o client_pattern client_pattern.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11



client_hunt:    client_hunt.o

	$(CC) -o client_hunt client_hunt.o SIMPLESOCKET.o -L/usr/lib/x86_64-linux-gnu -ldl -lstdc++  -std=c++11



clean:

	-rm -r -f   $(DEMOTARGET) *.o DOXYGENDOC  *.txt



doc:

	doxygen Doxyfile



all:	main server client client_random client_pattern client_hunt



run:    main

	./main
