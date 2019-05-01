#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

/* Lectura de una matriz de 21696 x 21696 */


#define WX 3
#define WY 3

#define MAXMO 21696
#define NX (MAXMO/32)
#define NY (MAXMO/32)

#define NDIMS 2
#define ERRCODE 2
#define ERR(e)                                 \
    {                                          \
        printf("Error: %s\n", nc_strerror(e)); \
        exit(ERRCODE);                         \
    }

struct arr2d
{
    float **m;
};

float convolve(float **a, float b[][WY]);
int escribir_nc(float data_out[][NY], int punterox,int punteroy);

int main()
{

    float data_out[NX][NY];
    float data_in[NX][NY];
    float matriz_w[WX][WY] =
        {
            {-1, -1, -1}, /*  initializers for row indexed by 0 */
            {-1, 8, -1},  /*  initializers for row indexed by 1 */
            {-1, -1, -1}  /*  initializers for row indexed by 2 */
        };

    struct arr2d matriz_resultado_NX_NY;
    struct arr2d matriz_resultado_3_3;

    int ncid, varid, retval;

    size_t start[2] = {0};
    size_t conteo[2] = {0};

    conteo[0] = NX;
    conteo[1] = NX;

    /** malloc y inicializacion de matriz **/
    matriz_resultado_NX_NY.m = malloc(NX * sizeof(float *));
    for (int i = 0; i < NX; i++)
    {
        matriz_resultado_NX_NY.m[i] = malloc(NX * sizeof(float));
    }

    matriz_resultado_3_3.m = malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++)
    {
        matriz_resultado_3_3.m[i] = malloc(3 * sizeof(float));
    }




                for (int i = 0; i < NX; i++)
            {
                for (int j = 0; j < NX; j++)
                {
                    matriz_resultado_NX_NY.m[i][j] = 0;
                }
            }

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (i == j)
                    {
                        matriz_resultado_3_3.m[i][j] = 0;
                    }
                    else
                    {
                        matriz_resultado_3_3.m[i][j] = 0;
                    }
                }
            }



    /** Aca desplazo los pedacitos de matriz **/
    for (int desplazamiento_x = 0; desplazamiento_x < (MAXMO / NX); desplazamiento_x++)
    {
        for (int desplazamiento_y = 0; desplazamiento_y < (MAXMO / NY); desplazamiento_y++)
        {
            start[0] = desplazamiento_x * NX;
            start[1] = desplazamiento_y * NY;
            if ((retval = nc_open("/home/matias/Documentos/sisop2019/TP2/pepe.nc", NC_NOWRITE, &ncid)))
                ERR(retval);

            if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
                ERR(retval);

            if ((retval = nc_get_vara_float(ncid, varid, start, conteo, &data_in[0][0])))
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



            for (int pixel_x = 1; pixel_x < NX - 1; pixel_x++)
            {
                for (int pixel_y = 1; pixel_y < NY - 1; pixel_y++)
                {
                    int indice_x = 0;
                    for (int x = pixel_x - 1; x < pixel_x + 2; x++)
                    {

                        int indice_y = 0;
                        for (int y = pixel_y - 1; y < pixel_y + 2; y++)
                        {
                            if (data_in[x][y]==-1){
                                matriz_resultado_3_3.m[indice_x][indice_y]=(float)(0.0 / 0.0);
                            }
                            else {
                                matriz_resultado_3_3.m[indice_x][indice_y] = data_in[x][y];
                            }
                            indice_y++;
                        }
                        indice_x++;
                    }
                    matriz_resultado_NX_NY.m[pixel_x][pixel_y] = convolve(matriz_resultado_3_3.m, matriz_w);
                }
            }
            
            
            for (int i = 0; i < NX; i++)
            {
                for (int j = 0; j < NX; j++)
                {
                    data_out[i][j] = ((float)0.000317469)*matriz_resultado_NX_NY.m[i][j];
                }
            }

            escribir_nc(data_out, desplazamiento_x,desplazamiento_y);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        free(matriz_resultado_3_3.m[i]);
    }
    free(matriz_resultado_3_3.m);
    
    for (int i = 0; i < NX; i++)
    {
        free(matriz_resultado_NX_NY.m[i]);
    }
    free(matriz_resultado_NX_NY.m);
    return 0;
}

float convolve(float **a, float b[][WY])
{   
    float ave = 0;
    for (int i = 0; i < WX; i++)
    {
        for (int j = 0; j < WX; j++)
        {
            ave = ave + (a[i][j] * b[i][j]);
        }
    }
    return ave;
}

int escribir_nc(float data_out[][NY], int punterox, int punteroy)
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
    if ((punteroy == 0) && (punterox == 0))
    {
        if ((retval = nc_create("/home/matias/Documentos/sisop2019/TP2/pepe2.nc", NC_CLOBBER, &ncid)))
            ERR(retval);
        /* Define the dimensions. NetCDF will hand back an ID for each. */
        if ((retval = nc_def_dim(ncid, "x", MAXMO, &x_dimid)))
            ERR(retval);
        if ((retval = nc_def_dim(ncid, "y", MAXMO, &y_dimid)))
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
    }
    else
    {
        if ((retval = nc_open("/home/matias/Documentos/sisop2019/TP2/pepe2.nc", NC_WRITE | NC_SHARE, &ncid)))
            ERR(retval);
        if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
            ERR(retval);
    }

    /* Write the pretend data to the file. Although netCDF supports
    * reading and writing subsets of data, in this case we write all
    * the data in one operation. */
    if ((retval = nc_put_vara_float(ncid, varid, start, conteo, &data_out[0][0])))
        ERR(retval);
    
    /* Close the file. This frees up any internal netCDF resources
    * associated with the file, and flushes any buffers. */
    if ((retval = nc_close(ncid)))
        ERR(retval);
    
    return 0;
}