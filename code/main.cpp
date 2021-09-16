////////////////////////////////////////////////////////////////////////////
//  Universidade Federal do Rio de Janeiro
//  Disciplina: Sistemas Operacionais - EEL770
//  Professor: Rodrigo Souza Couto
//  Desenvolvedor: Chritian Marques de Oliveira Silva
//  DRE: 117.214.742
//  Trabalho 2: Locks e Variáveis de Condição - Problema dos babuínos
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  O Sistema Operacional utilizado foi o Ubuntu 18.04 (WSL)
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//  RESUMO DO PROBLEMA
////////////////////////////////////////////////////////////////////////////
// Uma corda para transporte dos babuínos
// Nenhum babuíno poderá encontrar com outro vindo na direção oposta (os dois caem)
// Máximo de 5 babuínos da corda em uma direção
// Um fluxo contínuo de babuínos cruzando em uma direção não deve impedir que os babuínos sigam indefinidamente

////////////////////////////////////////////////////////////////////////////
//  RESUMO DA IMPLEMENTAÇÃO
////////////////////////////////////////////////////////////////////////////
// Serão criadas 10 threads, onde cada babuíno será representado por uma thread
// Cada thread realiza a soma de deslocamento do determinado babuíno
// Supondo que a corda tem 100 metros, quando chegar nesse valor o babuíno atravessou
// Existe uma variável de controle para o "direction" (definida por enum) indicando o sentido do babuíno:
//      * to_left  -> indica que o babuíno em questão está indo da direita para a esquerda
//      * to_right -> indica que o babuíno em questão está indo da esquerda para a direita
// Deste modo, cada thread (babuíno) deverá possuir:
//      * direction (to_left | to_right)    : sentido de deslocamento na corda
//      * position   (0-100)                : posição do babuíno na corda

//[B0:TL,100,W];[B1:TR,100,W];[B2:TL,100,W];[B3:TR,100,W];[B4:TR,100,W];[B5:TL,100,W];[B6:TR,100,W];[B7:TL,100,W];[B8:TR,100,W];[B9:TL,100,W]

////////////////////////////////////////////////////////////////////////////
//  IMPORTAÇÃO DE BIBLIOTECAS E CONFIGURAÇÕES DE AMBIENTE
////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <string>
#include "baboon.hpp"
using namespace std; 

////////////////////////////////////////////////////////////////////////////
//  VARIAVEIS/DEFINIÇÕES GLOBAIS
////////////////////////////////////////////////////////////////////////////
const int BABOONS_AMOUNT = 10;
baboon baboons[BABOONS_AMOUNT];  
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
const unsigned char MIN_RANDOM_VALUE = 1;
const unsigned char MAX_RANDOM_VALUE = 100;

// A lógica dessa função foi baseada no site: https://www.bitdegree.org/learn/random-number-generator-cpp
int Get_Random_Value(){
    // Obtém um número randômico entre min e max
    return (rand() % MAX_RANDOM_VALUE) + MIN_RANDOM_VALUE;
}

void *Increase_Movement(void *arg) {
    int id = *((int *) arg); 
    // pthread_mutex_lock(&lock);
    // cout << "Increase_Movement: " << id << endl;
    // pthread_mutex_unlock(&lock);
    for (int i = 0; i < baboons[id].ROPE_SIZE; i++) {
        pthread_mutex_lock(&lock);
        baboons[id].position++;
        cout << "++ ID[" << id << "]; Pos = " << baboons[id].position << endl;
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

void *Decrease_Movement(void *arg) {
    int id = *((int *) arg); 
    // pthread_mutex_lock(&lock);
    // cout << "Decrease_Movement: " << id << endl;
    // pthread_mutex_unlock(&lock);
    for (int i = 0; i < baboons[id].ROPE_SIZE; i++) {
        pthread_mutex_lock(&lock);
        baboons[id].position--;
        cout << "-- ID[" << id << "]; Pos = " << baboons[id].position << endl;
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}
                                                                             
int main(void) {          
    // Inicializa a semente aleatória
    //srand((unsigned) time(NULL));
    srand(123);

    for (int i = 0; i < BABOONS_AMOUNT; i++){
        if (Get_Random_Value()%2==0){
            baboons[i].set_to_rigth();
            pthread_create(&(baboons[i].my_thread), NULL, Increase_Movement,(void *)(&(baboons[i].id)));
        }
        else{
            baboons[i].set_to_left();
            pthread_create(&(baboons[i].my_thread), NULL, Decrease_Movement,(void *)(&(baboons[i].id)));
        }
    }

    for (int i = 0; i < BABOONS_AMOUNT; i++)
        pthread_join(baboons[i].my_thread, NULL);

    for (int i = 0; i < BABOONS_AMOUNT; i++)
        cout << "Contador" << i << " = "<< baboons[i].position << " --> "<< baboons[i].is_completed() << endl;

    return 0;
}