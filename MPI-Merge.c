#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

//Definicion de metodos.
int* mergeSortAux(int* list, int inicial, int final);
int* mergeSort(int *list, int n);
void Genera_vector(int v[], int m);
int* inicializaVector(int numero);

int main(int argc, char** argv){
	
	//Vector principal
    int* mainVector;
	
	//n: Longitud del vector    m: Limite maximo de los numeros
	int n, m;
	
	//definicion de variables
	clock_t comienzo, dif;
	
	// Inicilizacion del MPI
	int p, my_rank;
	
	/* Inicializacion del ambiente MPI */
	MPI_Init(&argc, &argv);
	
	/*Se le pide al comunicador MPI_COMM_WORLD que
     * almacene en p el numero de procesos de ese comunicador.
     * O sea para este caso, p indicara el num total de procesos que corresponde.
    */
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	/* Se el pide al comunicador MPI_COMM_WORLD que devuelva en la variable
     * my_rank la identificacion del proceso "llamador",
     * la identificacion es un numero de 0 a p-1.
     */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	if(my_rank == 0){
		printf("Bienvenido! Digite la dimension del vector:\n");
		scanf("%d",&n);
		printf("Se generarán %d numeros aleatorios entre un rango entre 0 y m. Defina m:\n",n);
		scanf("%d",&m);
	
		//Se inicializa el vector
		mainVector = inicializaVector(n); 	
	
		//se inicia el temporizador
		comienzo = clock();
	
		//Se le asignan valores aleatorios
		Genera_vector(mainVector, m);
	}
	
	//Se divide el array en tamaños iguales
	int tamano = n/p;
	
	//se manda cada subarreglo a cada proceso
	int* sub_arreglo = inicializaVector(tamano);
	MPI_Scatter(mainVector, tamano, MPI_INT, sub_arreglo, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    
	//se ejecuta el mergesort en cada proceso
    mergeSort(sub_arreglo, tamano);
	
	//se juntan todos los subarreglos en uno
    int *ordenados = NULL;
    if (my_rank == 0){
        ordenados = inicializaVector(n);
    }
    MPI_Gather(sub_arreglo, tamano, MPI_INT, ordenados, tamano, MPI_INT, 0, MPI_COMM_WORLD);
    
	//hacer el MergeSort final, solo lo hace el padre
	if (my_rank == 0){
		mergeSort(ordenados, n);
	}
	
	/* Se termina el ambiente MPI */
	MPI_Finalize();
	
	//termina el temporizador
	dif = clock() - comienzo;
	int msec = dif * 1000 / CLOCKS_PER_SEC;
	
	/* Despliega en pantalla el numero de veces que aparecia
	 * en la lista cada uno de los posibles valores
	 */
	int i = 0, j = 0;
	int conteo = 0;
	while(j <= m){
		if(ordenados[i] == j){
			conteo++;
		}
		else if (ordenados[i] > j){
			printf ("El valor %d se ha encontrado %d veces\n", j, conteo);
			j++;
			conteo = 0;
		}
		i++;
	}
	printf("\n");
	printf("\n");
	 
	 
	// La lista ordenada de los valores
	for(int k = 0; k < n; k++){
		printf("%d  ", ordenados[k]);
	}
	printf("\n");
	printf("\n");
	 
	// El tiempo que tardó el ordenamiento
	printf("El programa tardó %d milisegundos en hacer el ordenamiento", msec);
	printf("\n");
	printf("\n");
	 
	//los valores de p, n y m
	printf("El número de procesos fue de: %d\n", p);
	printf("La longitud del vector fue de: %d\n", n);
	printf("Se trabajó con un rango de 0 a %d\n", m);
	
	return(EXIT_SUCCESS);
}


void Genera_vector(
         int    v[]        /* out */,
         int    m          /* in  */)
{
      int    i;
      for (i = 0; i < m; i++)
        v[i]= rand() % m + 1; //genera un numero aleatorio entre 0 y m
                
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