#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define tamIN 20000
#define tamCirc 50
#define tamOUT 19998

int bfIN[tamIN];
int bfCirc[tamCirc];
int bfOUT[tamOUT];
pthread_t thread1;
pthread_t thread2;

struct {
    pthread_mutex_t lock; // threads que vão estar bloqueadas
    pthread_cond_t cond; // condição para verificar se pode continuar a produzir ou não
    int iIndex; //indice do buffer_in ao qual vai ser feita a media= (buffer_in[put.iIndex]+buffer_in[put.iIndex+1]+buffer_in[put.iIndex+2])/3
    int circIndex; //indice do buffer_circ onde vai ser colocado o valor da media

} put = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };

struct {
    pthread_mutex_t lock; // thread  que vai estar bloqueada
    pthread_cond_t cond; // condição que verifica se a thread consumidora ja consumio valor
    int indexOut; //indice do buffer_out onde vai ser colocado o elemento do buffer_circ
    int circIndex; //indice do elemento do buffer_circ que vai ser colocado no buffer_out
    int ready; // variavel que vai funcionar para dizer que a thread está pronta a consumir ou não
} get = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };


void* producer(void*), * consumer(void*);



int main() {

    int i;
    for (i = 0; i < tamIN; i++) {
        bfIN[i] = i + 1; //inicializaçao do buffer_in com interos do 1 a 30000
    }

    pthread_t PM_T1, PM_T2, CM_T;

    pthread_create(&PM_T1, NULL, producer, NULL); //inicializaçao de thread produtora
    pthread_create(&PM_T2, NULL, producer, NULL); //inicializaçao de thread produtora
    pthread_create(&CM_T, NULL, consumer, NULL); //inicializaçao de thread consumidora

    pthread_join(PM_T1, NULL);
    pthread_join(PM_T2, NULL);
    pthread_join(CM_T, NULL);

    printf("Buffer OUT : ");
    for (int i = 0; i < tamOUT; i++) {
        printf("%d |", bfOUT[i]);
    }
    printf("\n\n");

    printf("Buffer CIRC : ");
    for (int i = 0; i < tamCirc; i++) {
        printf("%d |", bfCirc[i]);
    }
    printf("\n\n");

    return 0;
}


void* producer(void* arg) {

    while(1) {

        pthread_mutex_lock(&put.lock);
        if (put.iIndex >= tamIN - 2) {
            pthread_mutex_unlock(&put.lock);
            return (NULL);
        }
        while (bfCirc[put.circIndex] != -1 && bfCirc[put.circIndex] != 0) {
            pthread_cond_wait(&put.cond, &put.lock); //colocar em espera threads produtoras enquanto o buffer_circ tiver cheio
        }
        bfCirc[put.circIndex] = (bfIN[put.iIndex] + bfIN[put.iIndex + 1] + bfIN[put.iIndex + 2]) / 3; //colocar no buffer_circ a media de buffer_in[put.iIndex],buffer_in[put.iIndex+1],buffer_in[put.iIndex+2]
        put.iIndex++; //incrementar indice do buffer_in
        put.circIndex++; //incrementar indice do buffer_circ
        if (put.circIndex == tamCirc) {
            put.circIndex = 0; //colocar indice do buffer_circ a 0 caso exceda o tamanho maximo do buffer_circ
        }

        pthread_mutex_unlock(&put.lock);


        pthread_mutex_lock(&get.lock);

        get.ready++;
        pthread_cond_signal(&get.cond); //acordar threads consumidoras quando existir elementos no buffer_circ

        pthread_mutex_unlock(&get.lock);

        //  *((int *) arg) += 1;


    }


}

void* consumer(void* arg) {

    while(1) {

        pthread_mutex_lock(&get.lock);
        if (get.indexOut == tamOUT) {
            pthread_mutex_unlock(&get.lock);
            return (NULL);  //quando chegar ao fim do buffer_out acaba
        }

        while (get.ready == 0) {
            pthread_cond_wait(&get.cond, &get.lock); //colocar em espera thread consumidora enquanto ainda não houver elementos no buffer_circ
        }

        bfOUT[get.indexOut] = bfCirc[get.circIndex]; //colocar elemento do buffer_circ no buffer_out
        bfCirc[get.circIndex] = -1; //colocar elemento colocado no buffer_out a -1
        get.circIndex++; //incrementar indice do buffer_circ
        get.indexOut++; //incrementar indice do buffer_out
        get.ready--;
        if (get.circIndex == tamCirc) {
            get.circIndex = 0;
        }
        pthread_mutex_unlock(&get.lock);

        pthread_mutex_lock(&put.lock);
        if (bfCirc[put.circIndex] == -1) {
            pthread_cond_signal(&put.cond); //acordar threads produtoras quando poderem colocar mais elementos no buffer_circ
        }
        pthread_mutex_unlock(&put.lock);


        // *((int *) arg) += 1;



    }

}
