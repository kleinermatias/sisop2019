#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>
#include <omp.h>
#include <time.h>

/* Lectura de una matriz de 21696 x 21696 */

#define WX 3
#define WY 3

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

    if ((retval = nc_open("/home/matias/Documentos/sisop2019/TP2/pepe.nc", NC_NOWRITE, &ncid)))
        ERR(retval);

    if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
        ERR(retval);

    //for (j = 1; j < NX - 1; j++)
    //{

    //}
    if ((retval = nc_get_vara_float(ncid, varid, start, conteo, data_in)))
        ERR(retval);

    if ((retval = nc_close(ncid)))
        ERR(retval);

    nc_close(ncid);

    /**
    printf("Matriz inicial: \n ");
    for (int i = 0; i < NX; i++)
    {
        for (int j = 0; j < NX; j++)
        {
            printf("%lf ", data_in[i][j]);
        }
        printf("%lf",(float)(0.0 / 0.0));
    }
    return (0);
    **/

    //printf("%lf",(float)(0.0 / 0.0)*10);
    //return (0);
   
    convolve(data_in, data_out, matriz_w);
    free(data_in);
    

    for (int j = 1; j < NX - 1; j++)
    {
        for (int i = 1; i < NX - 1; i++)
        {
            if (data_out[(i - 1) + ((j - 1) * NX)] == (float)(0.0/0.0))
            {
                printf("Matriz inicial: \n ");
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

    escribir_nc(data_out, 0, 0);
    free(data_out);

    return 0;
}

void convolve(float *data_in, float *data_out, float c[][WY])
{
    clock_t ini,fin;

    double segundos;

    ini=clock();

    int j, i;
    #pragma omp parallel for collapse(2) num_threads(4) 
    
    for (j = 1; j < NX - 1; j++)
    {
        for (i = 1; i < NX - 1; i++)
        {
            

            data_out[(i) + (j * NX)] = (data_in[(i - 1) + ((j - 1) * NX)] * c[0][0]) + (data_in[(i) + ((j - 1) * NX)] * c[0][1]) + (data_in[(i + 1) + ((j - 1) * NX)] * c[0][2]);
            data_out[(i) + (j * NX)] = data_out[(i) + (j * NX)] + (data_in[(i - 1) + ((j)*NX)] * c[1][0]) + (data_in[(i) + ((j)*NX)] * c[1][1]) + (data_in[(i + 1) + ((j)*NX)] * c[1][2]);
            data_out[(i) + (j * NX)] = data_out[(i) + (j * NX)] + (data_in[(i - 1) + ((j + 1) * NX)] * c[2][0]) + (data_in[(i) + ((j + 1) * NX)] * c[2][1]) + (data_in[(i + 1) + ((j + 1) * NX)] * c[2][2]);
            data_out[(i) + (j * NX)] = data_out[(i) + (j * NX)] *  (float)(0.00031746);
        }
    }

    fin=clock();
    segundos=(double)(fin-ini)/CLOCKS_PER_SEC;
    printf("%.16g \n", segundos);
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

        if ((retval = nc_create("/home/matias/Documentos/sisop2019/TP2/pepe2.nc", NC_CLOBBER, &ncid)))
            ERR(retval);
        /* Define the dimensions. NetCDF will hand back an ID for each. */
        if ((retval = nc_def_dim(ncid, "x", NX, &x_dimid)))
            ERR(retval);
        if ((retval = nc_def_dim(ncid, "y", NX, &y_dimid)))
            ERR(retval);

        /* The dimids array is used to pass the IDs of the dimensions of
        * the variable. */
        dimids[0] = x_dimid;
        dimids[1] = y_dimid;

        /* Define the variable. The type of the variable in this case is
        * NC_FLOAT (4-byte integer). */
        if ((retval = nc_def_var(ncid, "CMI", NC_FLOAT, NDIMS,
                                 dimids, &varid)))
            ERR(retval);

        /* End define mode. This tells netCDF we are done defining
        * metadata. */
        if ((retval = nc_enddef(ncid)))
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