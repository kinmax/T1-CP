
// REcursive solution to the Salesman problem 
// Look for an iterative solution to improve paralelism


/*--------------------------- INCLUDES ---------------------------*/

#include "math.h"
#include "time.h"
#include "stdio.h"
#include <omp.h>

/*---------------------------- DEFINES ---------------------------*/

#define NUM_CITIES 12
#define NUM_THREADS 2

/*-------------------------- STRUCTURES --------------------------*/

typedef struct {
                int x;
                int y;
               } COORD;

typedef struct {
                char city[15];
                COORD coord;
                } CITY;
                
typedef int SOLUTION[NUM_CITIES];                 
                
/*--------------------------- VARIABLES --------------------------*/

CITY cities[NUM_CITIES] = {
                           {"Karlsruhe", { 0,  0}},
                           {"Koeln"    , {-2,  5}},
                           {"Frankfurt", { 0,  3}},
		                       {"Muenchen" , { 4, -4}},
                           {"Freiburg" , {-2, -4}},
                           {"Dresden"  , { 8,  5}},
                           {"Berlin"   , { 8,  8}},
                           {"Hannover" , { 1,  8}},
                           {"Hamburg"  , { 2, 10}},
                           {"Stuttgart", { 1, -1}},
                           {"Ulm",       { 2, -2}},
                           {"Nurenberg", { 4,  2}}
                          };          
                          
SOLUTION final_solution[NUM_THREADS];    /* Final Order of the Cities  */ 
double best_result = 1000L; /* Final cost (Global Minima) */                    
                         
/*--------------------------- PROTOTYPES -------------------------*/

double calc_dist( COORD point1, COORD point2 );
double calc_total_dist( SOLUTION solution ); 
unsigned int fat ( int number ); 
void rotate_left ( int *solution, int position );

void ger_solution ( SOLUTION solution, int n, int th_id );
void heu_mais_perto ( void );
                             
/*----------------------------------------------------------------*
|                           calc_dist                             |
*-----------------------------------------------------------------*/

double calc_dist( COORD point1, COORD point2 )
{
  double cata, catb, result;
  
  cata = point1.x - point2.x;
  if ( cata < 0 )
     cata *= -1;

  catb = point1.y - point2.y;
  if ( catb < 0 )
     catb *= -1;

  result = sqrt ( pow(cata,2) + pow(catb,2) );
  
  return( result );
  
} /* calc_dist */

/*----------------------------------------------------------------*
|                        calc_total_dist                          |
*-----------------------------------------------------------------*/

double calc_total_dist( SOLUTION solution )
{
 int i;
 double total = 0L;
 
 for ( i=0; i<NUM_CITIES-1; i++ )
     total += calc_dist( cities[solution[i]].coord, cities[solution[i+1]].coord );
 
 total += calc_dist( cities[solution[i]].coord, cities[0].coord );
 
 return( total );

} /* calc_total_dist */

/*----------------------------------------------------------------*
|                              fat                                |
*-----------------------------------------------------------------*/

unsigned int fat ( int number )
{
  unsigned int soma;
  int i;

  soma = number;
  for ( i = number-1 ; i > 1 ; i-- )
      soma *= i;

  return ( soma );
}  /* fat */


/*----------------------------------------------------------------*
|                         rotate_left                             |
*-----------------------------------------------------------------*/

void rotate_left ( int *solution, int position )
{
  /* faco um shift left circular nos inteiros do vetor */
  /* so shift a partir de posicao, ate o final         */ 
  /* posicoes: |0|1|2|3|4| ... ate NUM_CITIES-1        */
  int sobra=0, i=0;
  
  /* verifico position */
  if ( (position < 0) || (position > NUM_CITIES-2) )
     {
     printf("\nERRO - Operacao de rotate left: posicao invalida\n");
     return;
     }
  
  /* guardo ponta esquerda do shift */
  sobra = *(solution+position);    
   
  /* efetuo shift */  
  for ( i= position ; i < NUM_CITIES-1 ; i++ )
      *(solution+i) = *(solution+i+1);   
  
  /* recoloco ponta esquerda na ponta direita */  
  *(solution+NUM_CITIES-1) = sobra;
  
} /* rotate_left */

/*----------------------------------------------------------------*
|                           ger_solution                          |
*-----------------------------------------------------------------*/

void ger_solution ( SOLUTION solution, int n, int th_id)
{
  /* rotina recursiva para o calculo da distancia de TODAS as solucoes */
  /* posiveis para o problema e escolha da menor (minimo global)       */
  
  int i, j;
  double custo;
  
  for ( i = 0 ; i < n ; i++ )
      {
      rotate_left(&solution[0],NUM_CITIES-n);
      if ( n != 2 )
         ger_solution(solution, n-1, th_id);
      else
         {
         /* calculo e comparo */
         
         custo = calc_total_dist(solution);
         
         if ( custo < best_result )
            {
            best_result = custo;
            
            for ( j=0 ; j < NUM_CITIES ; j++ )
                final_solution[th_id][j] = solution[j];
            }
         }      
      }

} /* ger_solution */


void swap(int arr[], int i, int j)
{
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
  return;
}

/*----------------------------------------------------------------*
|                               main                              |
*-----------------------------------------------------------------*/
                                
void main ( void )
{
  SOLUTION initial_solution[NUM_THREADS];
  double test_result = 0L;
  int i, j;
  unsigned int number_of_solutions = 0;
  double t1, t2;
  
  /* disparo relogio */
  
  t1 = omp_get_wtime();
     
  number_of_solutions = fat(NUM_CITIES-1);
  
  printf("\nNumero de solucoes = %u\n", number_of_solutions);

  for(i = 0; i < NUM_THREADS; i++)
  {
    for(j = 0; j < NUM_CITIES; j++)
    {
      initial_solution[i][j] = j;
    }
  }
  
  /* gero todas as solu��es, calculo e comparo */
  omp_set_num_threads(NUM_THREADS);
  #pragma omp parallel for default(none) schedule(static) shared(initial_solution) reduction(min:best_result)
  for(i = 0; i < NUM_CITIES; i++)
  {
    swap(initial_solution[omp_get_thread_num()], 0, i);
    ger_solution(initial_solution[omp_get_thread_num()], NUM_CITIES-1, omp_get_thread_num());
  }
  
  //heu_mais_perto();
  
  /* mostro resultado */
          		  
  printf("\nMinimo Global %f\n", best_result );
  
  printf("\nOrdem das cidades a visitar:\n\n");
  for(j = 0; j < NUM_THREADS; j++) 
  {
    for ( i = 0 ; i < NUM_CITIES ; i++ )
      printf("%s -> ",cities[final_solution[j][i]].city);
    printf("%s\n\n",cities[final_solution[j][0]].city);
  }
  
  
  /* paro relogio */
  
  t2 = omp_get_wtime();
  
  /* mostro tempo que passou */
  
  printf("\n\nSe passaram %lf segundos apos o inicio do programa.\n",
           t2-t1);
}                                
                                
                        