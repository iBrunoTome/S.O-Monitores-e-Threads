/*
Sistemas Operacionais
Objetivo
Exercitar o conteúdo visto em sala de aula sobre o uso das threads e monitores.
Integrantes
Bruno Tomé - 0011254 - ibrunotome@gmail.com
Ronan Nunes - 0011219 - ronannc12@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* Biblioteca para usar o sleep (segundo o -Wall) */
#include <pthread.h>

/* estrutura de dados que os personagens usam */

typedef struct {
    int id;
    int numero_iteracoes;
} ArgumentoT;

/* declaração de variaveis globais */
int forno = 0; /* 0 = ninguém usando, 1 = alguém usando */
pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t ativas = PTHREAD_MUTEX_INITIALIZER; /*monitor usado para contar quantas threads estao ativas*/
pthread_cond_t cond[6];
int vetor_fila[6] = {0, 0, 0, 0, 0, 0}; /* contem os personagens que estao aguardando o forno */
int usou_forno[6] = {0, 0, 0, 0, 0, 0}; /* conta quantas vezes a pessoa usou o forno */
int fila = 0; /* contador de pessoas presentes na fila */
int acabou = 0; /* contador que indica quando acaba as interaçoes das threads */
int flag = 0; /* usada para indicar com nao a como devolver um personagem(Deadlock) */
int per = 3; /* veriavel usada pra comparar prioridades */

/* A função nome retorna o nome do personagem, os nomes foram posicionados de forma que
   a namorada do personagem sempre fique 3 posições a frente dele, para que com um simples mod
   seja possível definir a prioridade dos casais de forma circular. Como não implementamos a
   prioridade utilizando mod, o posicionamento não fez diferença alguma. */

char *nome (int id) {

    char *nome;

    switch (id) {
        case 0 :
                nome = "Sheldon";
                break;
        case 1 :
                nome = "Howard";
                break;
        case 2 :
                nome = "Leonard";
                break;
        case 3 :
                nome = "Amy";
                break;
        case 4 :
                nome = "Bernadette";
                break;
        case 5 :
                nome = "Penny";
                break;
        default :
                break;
    }

    return nome;
}

/* procedimento prioridade, recebe o argumento do tipo personagem, retorna qual personagem na fila deve usar o forno*/

void prioridade(ArgumentoT *personagem){

	if (( vetor_fila[0] == 1 || vetor_fila[3] == 1 ) && ( vetor_fila[1] == 1 || vetor_fila[4] == 1 ) && ( vetor_fila[2] == 1 || vetor_fila[5] == 1 ) ){
		flag = 1;
	} 

	else {
		flag = 0;
		if ((*personagem).id  == 0 ){
			if (vetor_fila[1] == 1){
				per = 1;
			}
			else if (vetor_fila[4] == 1){
				per = 4;
			}
			else if (vetor_fila[2] == 1){
				per = 2;
			}
			else if (vetor_fila[5] == 1){
				per = 5;
			}
			else if (vetor_fila[3] == 1){
				per = 3;
			}  			
		}
		else if ((*personagem).id  == 1 ){	
			if (vetor_fila[2] == 1){
				per = 2;
			}
			else if (vetor_fila[5] == 1){
				per = 5;
			
			}
			else if (vetor_fila[0] == 1){
				per = 0;			
			}
			else if (vetor_fila[3] == 1){		
				per = 3;
			}
			else if (vetor_fila[4] == 1){
				per = 4;
			}
		}
		else if ((*personagem).id  == 2){
			if (vetor_fila[0] == 1){
				per = 0;
			}
			else if (vetor_fila[3] == 1){
				per = 3;
			}
			else if (vetor_fila[1] == 1){
				per = 1;
			}
			else if (vetor_fila[4] == 1){
				per = 4;
			}
			else if (vetor_fila[5] == 1){
				per = 5;
			}
		}
		else if ((*personagem).id  == 3){
			if (vetor_fila[1] == 1){
				per = 1;
			}
			else if (vetor_fila[4] == 1){
				per = 4;
			}
			else if (vetor_fila[2] == 1){
				per = 2;
			}
			else if (vetor_fila[5] == 1){
				per = 5;
			}
			else if (vetor_fila[0] == 1){
				per = 0;
			}
		}
		else if ((*personagem).id == 4){
			if (vetor_fila[2] == 1){
				per = 2;
			}
			else if (vetor_fila[5] == 1){
				per = 5;
			}
			else if (vetor_fila[0] == 1){
				per = 0;
			}
			else if (vetor_fila[3] == 1){
				per = 3;
			}
			else if (vetor_fila[1] == 1){
				per = 1;
			}
		}
		else if ((*personagem).id  == 5){
			if (vetor_fila[0] == 1){
				per = 0;
			}
			else if (vetor_fila[3] == 1){
				per = 3;
			}
			else if (vetor_fila[1] == 1){
				per = 1;
			}
			else if (vetor_fila[4] == 1){
				per = 4;
			}
			else if (vetor_fila[2] == 1){
				per = 2;
			}
		}		
	}
}

/* Função que apenas printa na tela que tem personagem esta usando o forno e o mesmo fica a utiliza-lo por 3 segundos */

void usando_forno(int id) {			
	usou_forno[id]++; /*incrementa no numero de vezes que tal personagem usou o forno*/    		 
    printf("%s está usando o forno\n", nome(id));
    sleep(1);
}

/* Função que escolhe aleatoriamente um tempo para o personagens ficar comendo */

void tempo_comendo() {
    sleep(3 + rand() % 4); /* Cumprindo o requisito de os personagens passarem de 3 a 6 segundos fazendo outras coisas ( ͡° ͜ʖ ͡°) */
}

/* Função onde o personagem vai requisitar o forno */

void quer_usar_forno(ArgumentoT *personagem) { 
    pthread_mutex_lock(&monitor);
    	printf("%s quer usar o forno\n", nome((*personagem).id)); 

        if ((forno == 1) || (fila > 0)){ /* se o forno está fechado ou tem alguem na fila */
			vetor_fila[(*personagem).id] = 1; 
           	fila++; /* contador de pessoas na fila */      
           	pthread_cond_wait(&cond[(*personagem).id], &monitor); /* da um wait no id do personagem */            
        }

       	forno = 1; /* forno em uso */
    pthread_mutex_unlock(&monitor);
}

/* função onde o personagem que estava utilizando o forno o libera e escolhe um personagem da fila se for possível, olhando sua prioridade */

void foi_comer(ArgumentoT *personagem){
    pthread_mutex_lock(&monitor);
        printf("%s foi comer\n", nome((*personagem).id));
        
        forno = 0; /* abre o forno */ 

        prioridade(personagem);        

        if (flag == 0){       
        	pthread_cond_signal(&(cond[per]));
        	vetor_fila[per] = 0;  
        	fila--; /* decrementa o contador de pessoas na fila */
    	}

    pthread_mutex_unlock(&monitor);
}

/* personagens */

void *personagens(void *argumento) {

	ArgumentoT *personagem = (ArgumentoT *) argumento; /* (ArgumentoT *) é um cast para a variável genérica argumento */
 
	pthread_mutex_lock(&ativas);
		acabou++; /*incrementa o numero de threads ativas*/
	pthread_mutex_unlock(&ativas);	

    while ((*personagem).numero_iteracoes > 0) {
        quer_usar_forno(personagem); /* mutex para pegar o forno */
        usando_forno((*personagem).id); 
        foi_comer(personagem); /* mutex para liberar o forno */
        tempo_comendo(); /* função para os personagens passarem de 3 a 6 segundos fazendo outras coisas */
        (*personagem).numero_iteracoes--; /* decrementa o número de iterações que foi passado via parâmetro */      
    }

    pthread_mutex_lock(&ativas);
		acabou--;/*decrementa o numero de threads ativas*/
	pthread_mutex_unlock(&ativas);	

	pthread_exit(NULL);

}

void *raj(){

	int achei = 0;
	int r = 0;
	int pos = 0;

	while (acabou > 0){ /* enquanto as threads não terminaram (acabou == 0 indica que terminaram) */
		sleep (5); /* faz o raj passar de 5 em 5 segundos checando */
		achei = usou_forno[1];

		if ((forno == 0) && (fila > 0)) { /* se o forno estiver aberto e se estiver pessoas esperando na fila */
			puts ("Raj detectou um DeadLock");
			for (r = 0; r < 6;r++){ /*raj quando há deadlock libera o personagem que menos usou o forno, trata a inanição*/
				if (usou_forno[r] <= achei){
					achei = usou_forno[r];
					pos = r;
				}
			}

			printf ("Liberando %s \n", nome(pos));
			pthread_cond_signal(&(cond[pos]));
			vetor_fila[pos] = 0;
			fila--; /* decrementa o contador de pessoas da fila */
		}
	}

	pthread_exit(NULL);
}

/* Irá disparar as threads dos personagens */

void dispara_threads_personagens(pthread_t threads[], ArgumentoT argumentos_threads[]) {

    int j = 0;

    while (j < 6) {
        /* personagens é o ponteiro para execução de uma função, e argumentos_threads[j] é o argumento passado referente a thread */
        pthread_create(&(threads[j]), NULL, personagens, &argumentos_threads[j]);
        j++; 
    }
}

void dispara_thread_raj(pthread_t thread){
	pthread_create(&(thread), NULL, raj, NULL);
}

int main(int argc, char **argv){

    int num_iteracoes, j = 0;

    /* Criando as threads para os 6 personagens + uma separada para o Raj */

    pthread_t threads_personagens[6];
    pthread_t thread_raj = 0;

    ArgumentoT argumentos_threads[6];

    /* Pegando o argumento passado via linha de comando */

    if (argc == 2) {
        num_iteracoes = atoi(argv[1]);
    }
    
    else {
        printf("\n\nVocê não passou o número de iterações via parâmetro, ou passou mais parâmetros do que o esperado.\n\n");
        exit(1);
    }

    for (j = 0; j < 6; j++) {
        argumentos_threads[j].id = j;  /* Atribuindo um id as pessoas */  
        argumentos_threads[j].numero_iteracoes = num_iteracoes; /* Atribuindo o número de iterações as pessoas */    
    }

    /* Disparar as threads dos personagens */

    dispara_threads_personagens(threads_personagens, argumentos_threads);

    /* dispara_thread_raj */

    dispara_thread_raj(thread_raj);

    /* Esperando as threads terminarem */

    for (j = 0; j < 6; j++) {
        pthread_join(threads_personagens[j], NULL);
    }
   
  	pthread_join(thread_raj,NULL);
    
    return 0;
}
