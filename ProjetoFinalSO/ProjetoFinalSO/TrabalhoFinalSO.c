#include <stdio.h>
#include <pthread.h>

#define tamIN 20000
#define tamCir 50
#define tamOUT 19998

int bfIN[tamIN];
int bfCir[tamCir];
int bfOUT[tamOUT];

//vari�vel com a posi��o a ser processada do bfIN
int posBfIN=0;

void produtora() {
	pthreah_mutex_lock();
	//vari�vel que guarda a m�dia de tr�s posi��es consecutivas
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

	//declara��o das vari�veis thread
	pthread_t PM_T1, PM_T2, CM_T;

	//vari�vel que assume os valores do bfIN de 1 a 20000
	int j = 1;
	//ciclo for para encher o bfIN com n�meros
	for (int i = 0; i < tamIN; i++) {
		bfIN[i] = j;
		j++;
	}

	//cria as threads produtoras e consumidora
	pthread_create(&PM_T1, NULL, produtora, NULL);
	pthread_create(&PM_T2, NULL, produtora, NULL);
	pthread_create(&CM_T, NULL, consumidora, NULL);

	//executa as threads produtoras e consumidora at� terminarem
	pthread_join(PM_T1, NULL);
	pthread_join(PM_T2, NULL);
	pthread_join(CM_T, NULL);

}
