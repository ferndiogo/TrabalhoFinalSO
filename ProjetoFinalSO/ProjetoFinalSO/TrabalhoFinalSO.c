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

//variável com a posição a ser processada do bfIN
int posBfIN = 0;
int posBfOUT = 0;

int verRead = 0;//Quantas posiçoes ocupadas no buffer circular

pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexProd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCons = PTHREAD_MUTEX_INITIALIZER;

int countP1 = 0;
int countP2 = 0;
int countC = 0;


void* produtora(void* cont) {
	while (posBfIN < tamIN - 3) {
		while (verRead >= tamCir - 1)
			pthread_cond_wait(&empty, &mutexProd);
		pthread_mutex_lock(&mutexProd);
		//variável que guarda a média de três posições consecutivas
		bfCir[posBfIN % tamCir] = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
		*((int*)cont) += 1;
		posBfIN++;
		verRead++;
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mutexProd);

		/*printf("Buffer CIRC : ");
		for (int i = 0; i < tamCir; i++) {
			printf("%d |", bfCir[i]);
		}
		printf("\n");*/
	}

}


void* consumidora(void* cont) {
	while (posBfOUT < tamOUT) {
		while (verRead <= 0)
			pthread_cond_wait(&full, &mutexCons);
		pthread_mutex_lock(&mutexCons);
		bfOUT[posBfOUT] = bfCir[posBfOUT % tamCir];
		bfCir[posBfOUT % tamCir] = 0;
		*((int*)cont) += 1;
		posBfOUT++;
		verRead--;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutexCons);
		/*printf("Buffer OUT : ");
		for (int i = 0; i < tamOUT; i++) {
			printf("%d |", bfOUT[i]);
		}*/
	}
}

int main() {

	//declaração das variáveis thread
	pthread_t PM_T1, PM_T2, CM_T;

	//ciclo for para encher o bfIN com números
	for (int i = 0, j = 1; i < tamIN; i++, j++) {
		bfIN[i] = j;
	}

	/*printf("Buffer IN : ");
	for (int i = 0; i < tamIN; i++) {
		printf("%d |", bfIN[i]);
	}*/

	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, &countP1);
	pthread_create(&PM_T2, NULL, produtora, &countP2);
	pthread_create(&CM_T, NULL, consumidora, &countC);

	//executa as threads produtoras e consumidora até terminarem
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

	printf("\n Produtora 1: %d ", countP1);
	printf("\n Produtora 2:  %d ", countP2);
	printf("\n Consumidora :  %d \n", countC);


}