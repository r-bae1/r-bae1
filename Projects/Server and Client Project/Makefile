all: dbclient dbserver
clean:
	rm dbclient
	rm dbserver
	
dbclient: dbclient.c msg.h
	gcc dbclient.c -o dbclient -Wall -Werror -std=gnu99
dbserver: dbserver.c msg.h common_threads.h
	gcc dbserver.c -o dbserver -Wall -Werror -std=gnu99 -pthread
