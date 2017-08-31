/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Oscar Azofeifa
 *
 * Created on 23 de agosto de 2017, 12:12
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


/*
 * 
 */
int main(int argc, char** argv) {
    
    int n, m;           //n: Longitud del vector    m: Limite maximo de los numeros
    
    //Definicion de metodos.
    int* mergeSortAux(int* list, int inicial, int final);
    int* mergeSort(int *list, int n);
    void Genera_vector(int v[], int m);
    int* inicializaVector(int numero);
    int p, my_rank;     //variables para el comm_size y comm_rank
    
    
    printf("Bienvenido! Digite la dimension del vector:\n");
    scanf("%d",n);
    printf("Se generarán %d numeros aleatorios entre un rango entre 0 y m. Defina m:\n",n);
    scanf("%d",m);
    
    int* mainVector = inicializaVector(n);          //Se inicializa el vector
    Genera_vector(mainVector, m);                   //Se le asignan valores aleatorios
    
    MPI_Init(&arc, &argv);                          /** Inicializacion del ambiente MPI
                                                     */
    
    MPI_Comm_size(MPI_COMM_WORLD, &p);              /*Se le pide al comunicador MPI_COMM_WORLD que
                                                     * almacene en p el numero de procesos de ese comunicador.
                                                     * O sea para este caso, p indicara el num total de procesos que corresponde.
                                                     */
    
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);        /* Se el pide al comunicador MPI_COMM_WORLD que devuelva en la variable
                                                     * my_rank la identificacion del proceso "llamador",
                                                     * la identificacion es un numero de 0 a p-1.
                                                     */
    
    int tamano = n/p;          //Se divide el array en tamaños iguales
    
    //se manda cada subarreglo a cada proceso
    int *sub_arreglo = malloc(tamano * sizeof(int));
    MPI_Scatter(mainVector, tamano, MPI_INT, sub_arreglo, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    
    //se ejecuta el mergesort en cada proceso
    mergeSort(sub_arreglo, sizeof(n)/sizeof(int));
    
    //se juntan todos los subarreglos en uno
    int *ordenados = NULL;
    if (my_rank == 0){
        ordenados = malloc(n * sizeof(int));
    }
    MPI_Gather(sub_arreglo, tamano, MPI_INT, ordenados, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    //REQUERIMIENTOS NO COMPLETADOS
    
    //1.    Repartir el vector entre los procesos p.
    //2.    Cada proceso ordena su parte. Si la parte es de tamaño impar, esta es asignada al ultimo proceso.
    //3.    Cada 2i procesos realizara un merge de su parte con la parte de los procesos 2i+1
    //3.1   Cuando el numero de procesos p sea impar o no sea potencia de 2, habran hojas que tendran que esperar para pasar su resultado.
    //4.    Se itera hasta que solo hayan dos procesos.
    //5.    Se mezclan los dos, listo.
    
    mergeSort(n, sizeof(n)/sizeof(int));            //Se aplica algoritmo de ordenamiento MergeSort
    
    
    //MONITOR
    for (int i = 0; i < sizeof(n)/sizeof(int); i++){
        printf("%d ", n[i]);
    }
    
    MPI_Finalize();   /* Se termina el ambiente MPI */
    return (EXIT_SUCCESS);
}

void Genera_vector(
         int    v[]        /* out */,
         int    m          /* in  */)
{
      int    i;
      for (i = 0; i < m; i++)
        v[i]= (int)rand()/(int)(((unsigned)RAND_MAX + 1) / 10);
                
 }  /* Genera_vector */

int* inicializaVector(int numero){
	int * lista = (int *) malloc(sizeof(int) * numero);
	
	if (lista == NULL){
		printf("Error al asignar memoria.\n");
		return NULL;
	}
}

int* mergeSortAux(int* list, int inicial, int final){//O(n log n)
    int n = final - inicial;                                // Tamaño el vector
    int mitad = (inicial + final)/2;                        // Punto de corte
    
    // CASO TRIVIAL DE RECURSION
    if (n > 2){                                              
        mergeSortAux(list, inicial, mitad);
        mergeSortAux(list, mitad, final);        
    }

    // CASO BASE DE RECURSION: Merge
    int listaParcial[n];                                    //Contenedor parcial
    int i = inicial, j = mitad;                             //Apuntadores
    while (i + j - mitad < final){
        if (j == final || (i < mitad && list[i] < list[j])){//Agrega a la lista parcial los valores menores entre los apuntadores.
            listaParcial[i + j - mitad - inicial] = list[i];//Se le quita inicial y mitad para eliminar bias en listaParcial.
            i++;
        }
        else {
            listaParcial[i + j - mitad - inicial] = list[j];//Se le quita inicial y mitad para eliminar bias en listaParcial.
            j++;
        }
    }
    
    // Reutilizacion del vector de entrada list
    for (int k = 0; k < n; k++)
        list[inicial + k] = listaParcial[k];
    return list;
}

int* mergeSort(int *list, int n){                           //Patron de funcion auxiliar
    return mergeSortAux(list, 0, n);
}