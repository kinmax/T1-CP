
// REcursive solution to the Salesman problem 
// Look for an iterative solution to improve paralelism


/*--------------------------- INCLUDES ---------------------------*/

#include "math.h"
#include "time.h"
#include "stdio.h"

/*---------------------------- DEFINES ---------------------------*/

#define NUM_CITIES 4

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
                           /*{"Karlsruhe", { 0,  0}},
                           {"Koeln"    , {-2,  5}},
                           {"Frankfurt", { 0,  3}},
		                       {"Muenchen" , { 4, -4}},
                           {"Freiburg" , {-2, -4}},
                           {"Dresden"  , { 8,  5}},
                           {"Berlin"   , { 8,  8}},
                           {"Hannover" , { 1,  8}},*/
                           {"Hamburg"  , { 2, 10}},
                           {"Stuttgart", { 1, -1}},
                           {"Ulm",       { 2, -2}},
                           {"Nurenberg", { 4,  2}}
                          };          
                          
SOLUTION final_solution;    /* Final Order of the Cities  */ 
double best_result = 1000L; /* Final cost (Global Minima) */                    
                         
/*--------------------------- PROTOTYPES -------------------------*/

double calc_dist( COORD point1, COORD point2 );
double calc_total_dist( SOLUTION solution ); 
unsigned int fat ( int number ); 
void rotate_left ( int *solution, int position );

void ger_solution ( SOLUTION solution, int n );
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

void ger_solution ( SOLUTION solution, int n )
{
  /* rotina recursiva para o calculo da distancia de TODAS as solucoes */
  /* posiveis para o problema e escolha da menor (minimo global)       */
  
  int i, j;
  double custo;
  
  for ( i = 0 ; i < n ; i++ )
      {
      rotate_left(&solution[0],NUM_CITIES-n);
      if ( n != 2 )
         ger_solution(solution, n-1);
      else
         {
         /* calculo e comparo */
         
         custo = calc_total_dist(solution);
         
         if ( custo < best_result )
            {
            best_result = custo;
            
            for ( j=0 ; j < NUM_CITIES ; j++ )
                final_solution[j] = solution[j];
            }
         }      
      }

} /* ger_solution */


/*----------------------------------------------------------------*
|                          heu_mais_perto                         |
*-----------------------------------------------------------------*/

void heu_mais_perto (void)
{
  /* rotina heuristica para a solucao aproximada do problema   */
  /* a partir da cidade inicial vou visitando sempre a cidade  */
  /* mais proxima ate esgotar as possibilidades; entao retorno */
  
  int    proxima_cidade, pos_solucao, j;
  double distancia, menor_distancia, custo_total;
  COORD  atual;
  SOLUTION visitadas;
  
  /* inicializo */
  
  pos_solucao = 1;  /* 0 ja eh Karlsruhe */
  final_solution[0] = 0;
  
  visitadas[0] = 1;
  for ( j=1 ; j < NUM_CITIES ; j++ )
      visitadas[j] = 0;
      
  /* posicao atual eh Karlsruhe */
  
  atual.x = cities[0].coord.x;
  atual.y = cities[0].coord.y;
  
  /* acho a cidade mais perto para cada posicao do vetor solucao */
  
  for ( pos_solucao = 1 ; pos_solucao < NUM_CITIES ; pos_solucao++ )
      {
      /* procuro cidade mais perto do ponto atual,   */
      /* que ainda nao tenha sido visitada           */
      
      menor_distancia = 1000;
      
      for ( j = 1 ; j < NUM_CITIES ; j++ )
          {
          /* se cidade nao foi visitada ainda */
          
          if ( visitadas[j] == 0 )
             {
             /* calculo distancia para posicao atual */
             
             distancia = calc_dist( atual, cities[j].coord);
          
             /* se distancia menor, marco */
             
             if ( distancia < menor_distancia )
                {
                menor_distancia = distancia;
                proxima_cidade = j;
                }
             }
          }  
      /* atualizo atual, vetor solucao e vetor visitadas */
      
      atual.x = cities[proxima_cidade].coord.x;
      atual.y = cities[proxima_cidade].coord.y;
      final_solution[pos_solucao] = proxima_cidade; 
      visitadas[proxima_cidade] = 1;
      }
      
  /* calculo custo da solucao encontrada */
  
  custo_total = calc_total_dist(final_solution);
  
  /* atualizo variavel global de resultado (custo) */
  
  best_result = custo_total;    
            
} /* heu_mais_perto */

/*----------------------------------------------------------------*
|                               main                              |
*-----------------------------------------------------------------*/
                                
void main ( void )
{
  SOLUTION initial_solution = {0, 1, 2, 3};
  double test_result = 0L;
  int i;
  unsigned int number_of_solutions = 0;
  time_t t1, t2;
  
  /* disparo relogio */
  
  time(&t1);
     
  number_of_solutions = fat(NUM_CITIES-1);
  
  printf("\nNumero de solucoes = %u\n", number_of_solutions);
  
  /* gero todas as solu��es, calculo e comparo */
  
  ger_solution(initial_solution, NUM_CITIES-1);
  
  //heu_mais_perto();
  
  /* mostro resultado */
          		  
  printf("\nMinimo Global %f\n", best_result );
  
  printf("\nOrdem das cidades a visitar:\n\n");
  for ( i = 0 ; i < NUM_CITIES ; i++ )
      printf("%s -> ",cities[final_solution[i]].city);
  printf("%s",cities[0].city);
  
  /* paro relogio */
  
  time(&t2);
  
  /* mostro tempo que passou */
  
  printf("\n\nSe passaram %.0lf segundos apos o inicio do programa.\n",
           difftime(t2, t1));
}                                
                                
                        