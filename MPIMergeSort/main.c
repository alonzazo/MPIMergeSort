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
#include "math.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    int n, m, p, my_rank;           //n: Longitud del vector    m: Limite maximo de los numeros //variables para el comm_size y comm_rank
    int * mainVector;
    int * sub_arreglo;              //Se inicializa el vector
    MPI_Status status;
    //Definicion de metodos.
    int* mergeSortAux(int* list, int inicial, int final);
    int* mergeSort(int *list, int n);
    void Genera_vector(int v[], int m);
    int* inicializaVector(int numero);
    int  esPotencia(int n);
    
    MPI_Init(&argc, &argv);                          /** Inicializacion del ambiente MPI
                                                     */
    
    MPI_Comm_size(MPI_COMM_WORLD, &p);              /*Se le pide al comunicador MPI_COMM_WORLD que
                                                     * almacene en p el numero de procesos de ese comunicador.
                                                     * O sea para este caso, p indicara el num total de procesos que corresponde.
                                                     */
    
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);        /* Se el pide al comunicador MPI_COMM_WORLD que devuelva en la variable
                                                     * my_rank la identificacion del proceso "llamador",
                                                     * la identificacion es un numero de 0 a p-1.
                                                     */
    
    if (my_rank == 0){
        //Suponiendo que el numero sea potencia de 2
        int flagPotencia = 1;
        printf("Bienvenido! Digite la dimension del vector (potencia de 2):\n");
        do {
            scanf("%d",&n);
            if (esPotencia(n)) flagPotencia = 0;
            else printf("Por favor, digite una cantidad potencia de 2.\n");
        } while (flagPotencia);
    
        printf("Se generarán %d numeros aleatorios entre un rango entre 0 y m. Defina m:\n",n);
        scanf("%d",m);
    
        mainVector = inicializaVector(n);               //Se define el vector
        Genera_vector(mainVector, m);                   //Se le asignan valores aleatorios
        
        //MONITOR
        printf("Vector por ordenar: ");
        for (int i = 0; i < n; i++){
            printf("%d ", mainVector[i]);
        }
        printf("\n");
    }
    
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); /* Se hace un BROADCAST desde proceso 0 (raiz) 
                                                      a todos los procesos - incluyendo a el mismo- 
                                                      del valor de n 
                                                      Note que el MPI_Bcast hace que cada uno de los procesos 
                                                      espere hasta recibir lo que se les  envia*/
    
    int tamano = n/p;                               //Se calcula el tamano de los vectores particulares
    
    //se manda cada subarreglo a cada proceso
    sub_arreglo = inicializaVector(tamano);
    
    MPI_Scatter(mainVector, tamano, MPI_INT, sub_arreglo, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    
    printf("Proceso #%d Arreglo sin ordenado: ", my_rank);
    for (int i = 0; i < tamano; i++){
        printf("%d ", sub_arreglo[i]);
    }
    printf("\n");
    
    //se ejecuta el mergesort en cada proceso
    //mergeSort(sub_arreglo, sizeof(n)/sizeof(int));
    mergeSort(sub_arreglo, tamano);
    
    //MONITOR
    printf("Proceso #%d Arreglo ordenado: ", my_rank);
    for (int i = 0; i < tamano; i++){
        printf("%d ", sub_arreglo[i]);
    }
    printf("\n");
    
    /*0:        0
     *1:    0       4
     *2:  0   2   4   6
     *3: 0 1 2 3 4 5 6 7    
     * Notese que el logaritmo base 2 indica la cantidad de niveles del arbol binario.
     */
    int nivelesDelArbol = (int)log2((double)p);
    int diferencia = 1;
    for (int i = 0; i < nivelesDelArbol; i++){                  //Recorre el arbol verticalmente
        int switcher = 0;                                       //flag que alterna quien envia y quien recibe
        for (int j = 0; j < p; j += diferencia){                //Recorre el arbol horizontalmente en saltos de 'diferencia'
            if (my_rank == j){                                  //Selecciona el proceso que corresponde
                if (switcher){
                    MPI_Send(&sub_arreglo, tamano, MPI_INT, j - diferencia, 25, MPI_COMM_WORLD);//Envia el subarreglo
                } else {
                    int * temporal = inicializaVector(tamano); //Crea un vector temporal para almacenar el arreglo que se recibe
                    MPI_Recv(&temporal, tamano, MPI_INT, j + diferencia, 25, MPI_COMM_WORLD, &status); // Se recibe el arreglo
                    sub_arreglo = merge(sub_arreglo, temporal, tamano, tamano); //se mezclan ordenadamente los arreglos
                    tamano += tamano;//nuevo tamano del arreglo
                }
                switcher = !switcher; //se alterna
            }
        }
        diferencia += diferencia;//Aumenta la diferencia entre cada nivel del arbol
    }
   /* //se juntan todos los subarreglos en uno
    int *ordenados = NULL;
    if (my_rank == 0){
        ordenados = malloc(n * sizeof(int));
    }
    MPI_Gather(sub_arreglo, tamano, MPI_INT, ordenados, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    */
    
    //REQUERIMIENTOS NO COMPLETADOS
    
    //1.    Repartir el vector entre los procesos p.
    //2.    Cada proceso ordena su parte. Si la parte es de tamaño impar, esta es asignada al ultimo proceso.
    //3.    Cada 2i procesos realizara un merge de su parte con la parte de los procesos 2i+1
    //3.1   Cuando el numero de procesos p sea impar o no sea potencia de 2, habran hojas que tendran que esperar para pasar su resultado.
    //4.    Se itera hasta que solo hayan dos procesos.
    //5.    Se mezclan los dos, listo.
    
    /*mergeSort(n, sizeof(n)/sizeof(int));            //Se aplica algoritmo de ordenamiento MergeSort
    */
    
    if(my_rank == 0){
    printf("Proceso #%d ARREGLO FINAL: ", my_rank);
        for (int i = 0; i < tamano; i++){
            printf("%d ", sub_arreglo[i]);
        }
        printf("\n");
    }
    
    MPI_Finalize();   /* Se termina el ambiente MPI */
    return (EXIT_SUCCESS);
}

int  esPotencia(int n){
    
    int result = 0;
    for (int i = 0; i < sizeof(int) * 8; i++){
        if (n & 1)
            result++;
        n = n >> 1;
    }
    if (result == 1) 
        return 1;
    else 
        return 0;
        
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

int * merge(int * list1, int * list2, int n1, int n2){
    int * resultList = inicializaVector(n1 + n2);           //Contenedor parcial                             
    int i = 0, j = 0;                                       //Apuntadores
    while (i + j < n1 + n2){
        if (j == n2 || (i < n1 && list1[i] < list2[j])){//Agrega a la lista parcial los valores menores entre los apuntadores.
            resultList[i + j] = list1[i];//Se le quita inicial y mitad para eliminar bias en listaParcial.
            i++;
        }
        else {
            resultList[i + j] = list2[j];//Se le quita inicial y mitad para eliminar bias en listaParcial.
            j++;
        }
    }
    return resultList;
}