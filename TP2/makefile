# makefile para el trabajo practico "1" de sistemas operativos 2 
# usar tabulador (no espacios) en make
all: convolve

convolve : convolve.c
	icc -xCORE_AVX2 -std=gnu99 -L/home/Estudiante53/Development/SO2/hpc/libs/netcdf/lib/ -lnetcdf -O3 -o prueba convolve.c -fopenmp
	gcc -pg -std=gnu99 -L/home/Estudiante53/Development/SO2/hpc/libs/netcdf/lib/ -lnetcdf -O3 -o pruebag convolve.c -fopenmp
# -Wall nos mostrará todos los avisos que produzca el compilador, no solamente los errores. Los avisos nos indican dónde y/o porqué podría surgir algún error en nuestro programa.
# -pedantic nos aporta más información sobre los errores y los avisos mostrados por GCC.
# -Werror tratara a cualquier warning como si un error se tratase. 

clean:
	rm eConvolve \
