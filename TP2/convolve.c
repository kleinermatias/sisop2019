#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <time.h>
#include <omp.h>


/* Lectura de una matriz de 21696 x 21696 */

#define WX 3
#define WY 3
#define num_thr 16
#define NX 21696
#define NY 21696

#define NDIMS 2
#define ERRCODE 2
#define ERR(e)                                 \
    {                                          \
        printf("Error: %s\n", nc_strerror(e)); \
        exit(ERRCODE);                         \
    }

void convolve(float *data_in, float *data_out, float c[][WY]);
int escribir_nc(float *data_out, int punterox, int punteroy);

int main()
{
    float *data_out = (float*)malloc(NX * NY * sizeof(float));
    float *data_in = (float*)malloc(NX * NY * sizeof(float));
    
    float matriz_w[WX][WY] =
        {
            {-1, -1, -1}, /*  initializers for row indexed by 0 */
            {-1, 8, -1},  /*  initializers for row indexed by 1 */
            {-1, -1, -1}  /*  initializers for row indexed by 2 */
        };

    int ncid, varid, retval;

    size_t start[2] = {0};
    size_t conteo[2] = {0};

    conteo[0] = NX;
    conteo[1] = NY;

    /** Aca desplazo los pedacitos de matriz **/
    start[0] = 0;
    start[1] = 0;

    if ((retval = nc_open("./pepe.nc", NC_NOWRITE, &ncid)))
        ERR(retval);

    if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
        ERR(retval);

    
    if ((retval = nc_get_vara_float(ncid, varid, start, conteo, data_in)))
        ERR(retval);

    if ((retval = nc_close(ncid)))
        ERR(retval);

    nc_close(ncid);
#pragma omp parallel for num_threads(num_thr)
    for (int j = 1; j < NX - 1; j++)
    {
        for (int i = 1; i < NX - 1; i++)
        {
            if (data_out[(i - 1) + ((j - 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i - 1) + ((j - 1) * NX)] = (0);
            }

            if (data_out[(i) + ((j - 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i) + ((j - 1) * NX)] = (0);
            }

            if (data_out[(i + 1) + ((j - 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i + 1) + ((j - 1) * NX)] = (0);
            }

            if (data_out[(i - 1) + ((j)*NX)] == (float)(0.0/0.0))
            {
                data_out[(i - 1) + ((j)*NX)] = (0);
            }

            if (data_out[(i) + ((j)*NX)] == (float)(0.0/0.0))
            {
                data_out[(i) + ((j)*NX)] = (0);
            }

            if (data_out[(i + 1) + ((j)*NX)] == (float)(0.0/0.0))
            {
                data_out[(i + 1) + ((j)*NX)] = (0);
            }

            if (data_out[(i - 1) + ((j + 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i - 1) + ((j + 1) * NX)] = (0);
            }

            if (data_out[(i) + ((j + 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i) + ((j + 1) * NX)] = (0);
            }

            if (data_out[(i + 1) + ((j + 1) * NX)] == (float)(0.0/0.0))
            {
                data_out[(i + 1) + ((j + 1) * NX)] = (0);
            }
        }
    }
   
    convolve(data_in, data_out, matriz_w);
    free(data_in);
    

    escribir_nc(data_out, 0, 0);
    free(data_out);

    return 0;
}

void convolve(float *data_in, float *data_out, float c[][WY])
{

    int j, i;
    struct timespec start, end;
    
    if( clock_gettime( CLOCK_MONOTONIC_RAW, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    
    #pragma omp parallel for num_threads(num_thr) 
    for (j = 1; j < NX - 1; j++)
    {
        for (i = 1; i < NX - 1; i++)
        {
            data_out[(i) + (j * NX)] = ((data_in[(i - 1) + ((j - 1) * NX)] * c[0][0]) + (data_in[(i) + ((j - 1) * NX)] * c[0][1]) + (data_in[(i + 1) + ((j - 1) * NX)] * c[0][2]) + (data_in[(i - 1) + ((j)*NX)] * c[1][0]) + (data_in[(i) + ((j)*NX)] * c[1][1]) + (data_in[(i + 1) + ((j)*NX)] * c[1][2]) + (data_in[(i - 1) + ((j + 1) * NX)] * c[2][0]) + (data_in[(i) + ((j + 1) * NX)] * c[2][1]) + (data_in[(i + 1) + ((j + 1) * NX)] * c[2][2]))*(float)(0.00031746);
        }
    }

    
    if( clock_gettime( CLOCK_MONOTONIC_RAW, &end) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

   u_int64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
   //u_int64_t total_time_s = delta_us/1000000;
   //u_int64_t total_time_ms = (delta_us%1000000)/1000;
   //u_int64_t total_total = delta_us;
   //printf("TERMINO DESCARGA EN %lds %ldms %ldus\n",total_time_s, total_time_ms, delta_us%1000);
   //printf("%ld",delta_us);
   FILE *fPtr;
fPtr = fopen("./statics", "a");
char buff[500];
// copy to buffer
sprintf(buff, "%ld\n", delta_us);
fputs(buff, fPtr);
}

int escribir_nc(float *data_out, int punterox, int punteroy)
{

    /* When we create netCDF variables and dimensions, we get back an
    * ID for each one. */
    int ncid, x_dimid, y_dimid, varid;
    int dimids[NDIMS];
    size_t start[2] = {0};
    start[0] = punterox * NX;
    start[1] = punteroy * NY;
    size_t conteo[2] = {0};
    conteo[0] = NX;
    conteo[1] = NX;

    /* Loop indexes, and error handling. */
    int x, y, retval;

    /* Create the file. The NC_CLOBBER parameter tells netCDF to
    * overwrite this file, if it already exists.*/

    if ((retval = nc_open("./pepe2.nc", NC_WRITE | NC_SHARE, &ncid)))
        ERR(retval);
    if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
        //ERR(retval);
        ERR(retval);

    /* Write the pretend data to the file. Although netCDF supports
    * reading and writing subsets of data, in this case we write all
    * the data in one operation. */
    if ((retval = nc_put_vara_float(ncid, varid, start, conteo, data_out)))
        ERR(retval);

    /* Close the file. This frees up any internal netCDF resources
    * associated with the file, and flushes any buffers. */
    if ((retval = nc_close(ncid)))
        ERR(retval);

    return 0;
}
