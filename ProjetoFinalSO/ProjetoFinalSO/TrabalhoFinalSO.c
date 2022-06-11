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

void *produtora(void* arg) {
	int media;
	while(posBfIN+2 < tamIN){
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		//variável que guarda a média de três posições consecutivas
		media = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
		posBfIN++;
		pthread_mutex_unlock(&mutex);

		for (int i = 0; i < tamCir; i++){
			if (bfCir[i] == 0) {
				bfCir[i] = media;
				sem_post(&full);
				break;
			}
		}
		printf("Buffer CIRC : ");
		for (int i = 0; i < tamCir; i++) {
			printf("%d |",bfCir[i]);
		}
	}


}

void *consumidora(void* arg) {
	sem_wait(&full);
	for (int i = 0; i < tamCir; i++) {
		if (bfCir[i] != 0) {
			bfOUT[posBfOUT] = bfCir[i];
			posBfOUT++;
			sem_post(&empty);
		}
	}
	printf("Buffer OUT : ");
	for (int i = 0; i < tamOUT; i++) {
		printf("%d |", bfOUT[i]);
	}
	
}

int main() {

	//declaração das variáveis thread
	pthread_t PM_T1, PM_T2, CM_T;

	//variável que assume os valores do bfIN de 1 a 20000
	int j = 1;
	//ciclo for para encher o bfIN com números
	for (int i = 0; i < tamIN; i++) {
		bfIN[i] = j;
		j++;
	}

	//ciclo for para encher o bfIN com zeros
	for (int i = 0; i < tamCir; i++) {
		bfIN[i] = 0;
	}

	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, tamCir);
	sem_init(&full, 0, 0);

	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, NULL);
	pthread_create(&PM_T2, NULL, produtora, NULL);
	pthread_create(&CM_T, NULL, consumidora, NULL);

	//executa as threads produtoras e consumidora até terminarem
	pthread_join(PM_T1, NULL);
	pthread_join(PM_T2, NULL);
	pthread_join(CM_T, NULL);

}
