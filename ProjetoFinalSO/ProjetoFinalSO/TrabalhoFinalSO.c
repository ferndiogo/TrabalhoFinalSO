#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define tamIN 20000
#define tamCirc 50
#define tamOUT 19998

int bfIN[tamIN];
int bfCirc[tamCirc];
int bfOUT[tamOUT];

//Contador para apresentar quantas vezes as threads produziram
//e consumiram elementos
int countP1 = 0;
int countP2 = 0;
int countC = 0;

pthread_mutex_t mutexProd  = PTHREAD_MUTEX_INITIALIZER; // threads que vão estar bloqueadas
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // condição para verificar se pode continuar a produzir ou não
int posbfIN; //indice do buffer_in ao qual vai ser feita a media= (buffer_in[put.iIndex]+buffer_in[put.iIndex+1]+buffer_in[put.iIndex+2])/3
int posCircProd; //indice do buffer_circ onde vai ser colocado o valor da media

pthread_mutex_t mutexCons = PTHREAD_MUTEX_INITIALIZER; // thread  que vai estar bloqueada
pthread_cond_t full = PTHREAD_COND_INITIALIZER; // condição que verifica se a thread consumidora ja consumio valor
int posbfOut; //indice do buffer_out onde vai ser colocado o elemento do buffer_circ
int posCircCons; //indice do elemento do buffer_circ que vai ser colocado no buffer_out
int ready; // variavel que vai funcionar para dizer que a thread está pronta a consumir ou não


void* producer(void*), * consumer(void*);


int main() {

    for (int i = 0, j = 1; i < tamIN; i++, j++) {
        bfIN[i] = j; 
    }

    pthread_t PM_T1, PM_T2, CM_T;

    //inicializção da Threads
    pthread_create(&PM_T1, NULL, producer, &countP1); 
    pthread_create(&PM_T2, NULL, producer, &countP2); 
    pthread_create(&CM_T, NULL, consumer, &countC); 

    //Espera que as threads terminem de correr
    pthread_join(PM_T1, NULL);
    pthread_join(PM_T2, NULL);
    pthread_join(CM_T, NULL);

    //ciclo para imprimir na consola o buffer OUT
    printf("Buffer OUT : ");
    for (int i = 0; i < tamOUT; i++) {
        printf("%d |", bfOUT[i]);
    }
    printf("\n\n");

    //ciclo para imprimir na consola o buffer Circular
    printf("Buffer CIRC : ");
    for (int i = 0; i < tamCirc; i++) {
        printf("%d |", bfCirc[i]);
    }
    printf("\n\n");

    //imprimir na consola o que cada threads consumiu e produziu
    printf("\nProdutora 1: %d ", countP1);
    printf("\nProdutora 2:  %d ", countP2);
    printf("\nConsumidora: %d \n", countC);

    return 0;
}


void* producer(void* count) {

    while(1) {
        //bloqueia um recurso compartilhado
        pthread_mutex_lock(&mutexProd);
        //verifica a condição para terminar a Thread
        if (posbfIN >= tamIN - 3) {
            pthread_mutex_unlock(&mutexProd);
            break;
        }
        while (bfCirc[posCircProd] != -1 && bfCirc[posCircProd] != 0) {
            pthread_cond_wait(&empty, &mutexProd); //colocar em espera threads produtoras enquanto o buffer_circ tiver cheio
        }
        bfCirc[posCircProd] = (bfIN[posbfIN] + bfIN[posbfIN + 1] + bfIN[posbfIN + 2]) / 3; //colocar no buffer_circ a media de buffer_in[put.iIndex],buffer_in[put.iIndex+1],buffer_in[put.iIndex+2]
        posbfIN++; //incrementar indice do buffer_in
        posCircProd++; //incrementar indice do buffer_circ
        if (posCircProd == tamCirc) {
            posCircProd = 0; //colocar indice do buffer_circ a 0 caso exceda o tamanho maximo do buffer_circ
        }

        pthread_mutex_unlock(&mutexProd);


        pthread_mutex_lock(&mutexCons);

        ready++;
        pthread_cond_signal(&full); //acordar threads consumidoras quando existir elementos no buffer_circ

        pthread_mutex_unlock(&mutexCons);

        *((int *) count) += 1;


    }


}

void* consumer(void* count) {

    while(1) {

        pthread_mutex_lock(&mutexCons);
        if (posbfOut == tamOUT) {
            pthread_mutex_unlock(&mutexCons);
            return (NULL);  //quando chegar ao fim do buffer_out acaba
        }

        while (ready == 0) {
            pthread_cond_wait(&full, &mutexCons); //colocar em espera thread consumidora enquanto ainda não houver elementos no buffer_circ
        }

        bfOUT[posbfOut] = bfCirc[posCircCons]; //colocar elemento do buffer_circ no buffer_out
        bfCirc[posCircCons] = -1; //colocar elemento colocado no buffer_out a -1
        posCircCons++; //incrementar indice do buffer_circ
        posbfOut++; //incrementar indice do buffer_out
        ready--;
        if (posCircCons == tamCirc) {
            posCircCons = 0;
        }
        pthread_mutex_unlock(&mutexCons);

        pthread_mutex_lock(&mutexProd);
        if (bfCirc[posCircCons] == -1) {
            pthread_cond_signal(&empty); //acordar threads produtoras quando poderem colocar mais elementos no buffer_circ
        }
        pthread_mutex_unlock(&mutexProd);


        *((int *) count) += 1;



    }

}
