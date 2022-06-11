#include <stdio.h>
#include <pthread.h>

#define tamIN 20000
#define tamCir 50
#define tamOUT 19998

int bfIN[tamIN];
int bfCir[tamCir];
int bfOUT[tamOUT];

//variável com a posição a ser processada do bfIN
int posBfIN=0;

void produtora() {
	pthreah_mutex_lock();
	//variável que guarda a média de três posições consecutivas
	int media = (bfIN[posBfIN] + bfIN[posBfIN + 1] + bfIN[posBfIN + 2]) / 3;
	posBfIN++;

	for (int i = 0; i < tamCir; i++){
		if (bfCir[i] == 0) {
			bfCir[i] = media;
			break;
		}
	}


}

void consumidora() {

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

	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, NULL);
	pthread_create(&PM_T2, NULL, produtora, NULL);
	pthread_create(&CM_T, NULL, consumidora, NULL);

	//executa as threads produtoras e consumidora até terminarem
	pthread_join(PM_T1, NULL);
	pthread_join(PM_T2, NULL);
	pthread_join(CM_T, NULL);

}
