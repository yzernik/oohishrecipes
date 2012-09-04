all: server clientadd clientquery pyclient

server : servermain.c searchserver.c cthpool/thpool.c mysuffixtree/mygensuffixtree.c mysuffixtree/genoperations.c
	gcc servermain.c searchserver.c cthpool/thpool.c mysuffixtree/mygensuffixtree.c mysuffixtree/genoperations.c -pthread -o server

clientadd : clientaddmain.c client.c clientadd.c
	gcc clientaddmain.c client.c clientadd.c -o clientadd

clientquery : clientquerymain.c client.c clientquery.c
	gcc clientquerymain.c client.c clientquery.c -o clientquery

pyclient : recipesearchmodule.c clientadd.c clientquery.c client.c
	gcc -shared -I/usr/include/python2.7/ -lpython2.7 -o recipesearchmodule.so recipesearchmodule.c clientadd.c clientquery.c client.c

clean:
	rm -f server clientadd clientquery connect_socket recipesearchmodule.so
