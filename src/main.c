/* Contador de numeros primos multiprocesso
 *
 * Victor Ferrao Santolim
 * RA 187888
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define N_FILHOS 3

void filhoCalcPrimo(int filhoParaPai[2] , int paiParaFilho[2] , int numFilho);

int main() {

	pid_t filhos[N_FILHOS]; //Vetor de processos filhos
	unsigned long int numLido; //Leitura atual

	//Cria as pipes de comunicacao interprocesso, em ambos os sentidos
	int paiParaFilho[2];
    int filhoParaPai[2];
    pipe(paiParaFilho);
    pipe(filhoParaPai);

    int filhosAteEntao = 0;
    int resultFork;
    char resPrim;
    int numPrimos = 0;

	do
	{
       scanf("%lu", &numLido); //Pega um numero da entrada

       //Coloca o numero lido no pipe para os filhos lerem
       write(paiParaFilho[1], &numLido, 8);

       //Cria novos filhos caso ainda existam menos que N_FILHOS
       if(filhosAteEntao < N_FILHOS)
       {
       		filhos[filhosAteEntao] = fork();

       		//Erro
	    	if(filhos[filhosAteEntao] < 0)
	    	{
	    		printf("Erro no fork do filho %d\n", filhosAteEntao);
	    		return -1;
	    	}

	    	//Processo filho
	    	else if(filhos[filhosAteEntao] == 0)
	    	{
	    		
	    		filhoCalcPrimo(filhoParaPai , paiParaFilho , filhosAteEntao + 1);
	    	}
	    	//Processo pai
	    	else
	    	{
	    		filhosAteEntao++;
	    		printf("Filhos criados ate entao: %d\n", filhosAteEntao);
	    		
	    		if(filhosAteEntao == 1)
	    		{
	    			close(paiParaFilho[0]);
   					close(filhoParaPai[1]);
	    		}
	    		
	    	}

       }

	}while(getchar() != 10);

	close(paiParaFilho[1]);


  	for(int i = 0; i < N_FILHOS; i++) 
    	waitpid(filhos[i], NULL, 0);

    while(read(filhoParaPai[0], &resPrim, 1) > 0) 
    {
      
      if(resPrim == 0) numPrimos++;
      
    }

    close(filhoParaPai[0]);

    printf("%d\n", numPrimos);

    return 0;

}

void filhoCalcPrimo(int filhoParaPai[2] , int paiParaFilho[2] , int numFilho)
{
	unsigned long int n;
	char naoPrimo = 0;

	close(filhoParaPai[0]);
	close(paiParaFilho[1]);
	//Tenta realizar a leitura de novos numeros no pipe
	//Se houver, verifica se eh primo

	printf("Estou aqui 1\n");
	while(read(paiParaFilho[0] , &n , 8) > 0)
	{
		naoPrimo = 0;

		if(n == 0 || n == 1) naoPrimo = 1;

		for(unsigned long int i=2 ; i <= n/2 ; i++)
		{
	        if(n%i==0)
	        {
	            naoPrimo=1;
	            break;
	        }
		}

		//Escreve no pipe de saida, enviando o resultado para o pai
		// 1 = Nao eh primo
		// 0 = Eh primo

		write(filhoParaPai[1] , &naoPrimo, 1);
	}

	printf("Estou aqui 2\n");

	exit(1);
}