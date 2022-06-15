#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define tamIN 20000
#define tamCir 50
#define tamOUT 19998

int bfIN[tamIN];
int bfCir[tamCir];
int bfOUT[tamOUT];

//variável com a posição a ser processada do bfIN
int posBfIN = 0;
int posBfOUT = 0;


//Contador para apresentar quantas vezes as threads produziram
//e consumiram elementos
int countP1 = 0;
int countP2 = 0;
int countC = 0;
pthread_mutex_t empty, full, mutex;

void* produtora(void* cont) {
	while (posBfIN < tamIN - 3) {
		//bloquear
		pthread_mutex_lock(&empty);
		pthread_mutex_lock(&mutex);

		//variável que guarda a média de três posições consecutivas
		bfCir[posBfIN % tamCir] = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
		*((int*)cont) += 1;
		posBfIN++;
		//desbloquear
		pthread_mutex_unlock(&full);
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
		//bloquear
		pthread_mutex_lock(&full);
		pthread_mutex_lock(&mutex);

		bfOUT[posBfOUT] = bfCir[posBfOUT % tamCir];
		bfCir[posBfOUT % tamCir] = 0;
		*((int*)cont) += 1;
		posBfOUT++;
		//desbloquear
		pthread_mutex_unlock(&empty);
		pthread_mutex_unlock(&mutex);

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

	//ciclo for para encher o bfIN com zeros
	for (int i = 0; i < tamCir; i++) {
		bfCir[i] = 0;
	}

	pthread_mutex_init(&empty, NULL);
	pthread_mutex_init(&full, NULL);
	pthread_mutex_init(&mutex, NULL);


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

	printf("\nProdutora 1: %d ", countP1);
	printf("\nProdutora 2:  %d ", countP2);
	printf("\nConsumidora: %d \n", countC);


}