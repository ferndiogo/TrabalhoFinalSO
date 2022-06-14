#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define tamIN 200
#define tamCir 5
#define tamOUT 198

int bfIN[tamIN];
int bfCir[tamCir];
int bfOUT[tamOUT];

//vari�vel com a posi��o a ser processada do bfIN
int posBfIN = 0;
int posBfOUT = 0;

//sem_t full;
//sem_t empty;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t empty, full;

//Contador para apresentar quantas vezes as threads produziram 
//e consumiram elementos
int countP1 = 0;
int countP2 = 0;
int countC = 0;

void* produtora(void* cont) {
	while (posBfIN < tamIN - 3) {
		pthread_mutex_lock(&mutex);	
		pthread_cond_wait(&empty, &mutex);
		//vari�vel que guarda a m�dia de tr�s posi��es consecutivas
		bfCir[posBfIN % tamCir] = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
		*((int*)cont) += 1;
		posBfIN++;
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutex);

		/*printf("Buffer CIRC : ");
		for (int i = 0; i < tamCir; i++) {
			printf("%d |", bfCir[i]);
		}
		printf("\n");*/
	}

}

void* consumidora(void* cont) {
	while (posBfOUT < tamOUT) {
		pthread_cond_wait(&full, &mutex);
		bfOUT[posBfOUT] = bfCir[posBfOUT % tamCir];
		bfCir[posBfOUT % tamCir] = 0;
		*((int*)cont) += 1;
		posBfOUT++;
		pthread_cond_signal(&empty);

		/*printf("Buffer OUT : ");
		for (int i = 0; i < tamOUT; i++) {
			printf("%d |", bfOUT[i]);
		}*/
	}
}

int main() {

	//declara��o das vari�veis thread
	pthread_t PM_T1, PM_T2, CM_T;

	//ciclo for para encher o bfIN com n�meros
	for (int i = 0, j = 1; i < tamIN; i++,j++) {
		bfIN[i] = j;
	}

	/*printf("Buffer IN : ");
	for (int i = 0; i < tamIN; i++) {
		printf("%d |", bfIN[i]);
	}*/

	//ciclo for para encher o bfIN com zeros
	for (int i = 0; i < tamCir; i++) {
		bfCir[i] = 0;
	}

	pthread_mutex_init(&mutex, NULL);
	//sem_init(&empty, 0, tamCir);
	//sem_init(&full, 0, 0);

	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, &countP1);
	pthread_create(&PM_T2, NULL, produtora, &countP2);
	pthread_create(&CM_T, NULL, consumidora, &countC);

	//executa as threads produtoras e consumidora at� terminarem
	pthread_join(PM_T1, NULL);
	pthread_join(PM_T2, NULL);
	pthread_join(CM_T, NULL);

 	printf("Buffer OUT : ");
	for (int i = 0; i < tamOUT; i++) {
		printf("%d |", bfOUT[i]);
	}

	printf("\n\n");

	printf("Buffer CIRC : ");
	for (int i = 0; i < tamCir; i++) {
		printf("%d |", bfCir[i]);
	}
	printf("\n");

	printf("\nProdutora 1: %d ", countP1);
	printf("\nProdutora 2:  %d ", countP2);
	printf("\nConsumidora: %d \n", countC);

}
