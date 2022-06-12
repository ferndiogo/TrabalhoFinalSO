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

sem_t full;
sem_t empty;

pthread_mutex_t mutex;

void* produtora(void* arg) {
	int media;
	while (posBfIN < tamIN - 3) {
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		//variável que guarda a média de três posições consecutivas
		media = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
		posBfIN++;

		for (int i = 0; i < tamCir; i++) {
			if (bfCir[i] == 0) {
				bfCir[i] = media;
				sem_post(&full);
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
		/*printf("Buffer CIRC : ");
		for (int i = 0; i < tamCir; i++) {
			printf("%d |", bfCir[i]);
		}
		printf("\n");*/
	}

}

void* consumidora(void* arg) {
	while (posBfOUT < tamOUT) {
		sem_wait(&full);
		for (int i = 0; i < tamCir; i++) {
			if (bfCir[i] != 0) {
				bfOUT[posBfOUT] = bfCir[i];
				bfCir[i] = 0;
				posBfOUT++;
				sem_post(&empty);
			}
		}
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
	sem_init(&empty, 0, tamCir);
	sem_init(&full, 0, 0);

	int countP1 = 0;
	int countP2 = 0;
	int countC1 = 0;
	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, countP1);
	pthread_create(&PM_T2, NULL, produtora, countP2);
	pthread_create(&CM_T, NULL, consumidora, countC1);

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

	printf("%d |", countP1);
	printf("%d |", countP2);
	printf("%d |", countC1);

}
