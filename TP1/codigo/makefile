# makefile para el trabajo practico "1" de sistemas operativos 2 
# usar tabulador (no espacios) en make

servidor : servidor.c
	gcc -Wall -Werror -pedantic -o eServidor servidor.c
# -Wall nos mostrará todos los avisos que produzca el compilador, no solamente los errores. Los avisos nos indican dónde y/o porqué podría surgir algún error en nuestro programa.
# -pedantic nos aporta más información sobre los errores y los avisos mostrados por GCC.
# -Werror tratara a cualquier warning como si un error se tratase. 

cliente : cliente.c
	gcc -Wall -Werror -pedantic -o eCliente cliente.c
# -Wall nos mostrará todos los avisos que produzca el compilador, no solamente los errores. Los avisos nos indican dónde y/o porqué podría surgir algún error en nuestro programa.
# -pedantic nos aporta más información sobre los errores y los avisos mostrados por GCC.
# -Werror tratara a cualquier warning como si un error se tratase. 

clean:
	rm eServidor eCliente \
