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

int* mergeSortAux(int* list, int inicial, int final){
    int n = final - inicial;
    int mitad = (inicial + final)/2;
    if (n > 2){
        mergeSortAux(list, inicial, mitad);
        mergeSortAux(list, mitad, final);        
    }
    int listaParcial[n];
    int i = inicial, j = mitad;
    while (i + j - mitad < final){
        if (j == final || (i < mitad && list[i] < list[j])){
            listaParcial[i + j - mitad - inicial] = list[i];
            i++;
        }
        else {
            listaParcial[i + j - mitad - inicial] = list[j];
            j++;
        }
    }
    for (int k = 0; k < n; k++)
        list[inicial + k] = listaParcial[k];
    return list;
}

int* mergeSort(int *list, int n){
    return mergeSortAux(list, 0, n);
}
/*
 * 
 */
int main(int argc, char** argv) {
    
    int n, m;           //n: Longitud del vector    m: Limite maximo de los numeros
    
    void Genera_vector(int v[], int m);
    int* inicializaVector(int numero);
    
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