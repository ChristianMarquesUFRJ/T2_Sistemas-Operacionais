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



////////////////////////////////////////////////////////////////////////////
//  IMPORTAÇÃO DE BIBLIOTECAS E CONFIGURAÇÕES DE AMBIENTE
////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <cmath>
#include <string>
#include <iomanip>
using namespace std; 
#include "baboon.hpp"
#include "constants_and_types.hpp"

baboon baboons[BABOONS_AMOUNT];  
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t available_rope;

////////////////////////////////////////////////////////////////////////////
//  FUNÇÕES AUXILIARES
////////////////////////////////////////////////////////////////////////////

// A lógica dessa função foi baseada no seguinte site: https://www.bitdegree.org/learn/random-number-generator-cpp
int Get_Random_Value(){
    // Obtém um número randômico entre min e max
    return (rand() % MAX_RANDOM_VALUE) + MIN_RANDOM_VALUE;
}

double Calculate_Percent_Complete(int initial_position, int current_position){
    if (initial_position == 0)
        return ((current_position)/ROPE_SIZE)*100.0;
    
    return ((ROPE_SIZE-current_position)/ROPE_SIZE)*100.0;
}

// A lógica dessa função foi baseada no seguinte forúm: https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
int Get_Digits_Number(int number){
    return ((number > 0) ? (int) log10 ((double) number) + 1 : 1);
}

int Get_In_Moving_Baboons(){
    int number = 0;
    for (int i = 0; i < BABOONS_AMOUNT; i++) 
        if (baboons[i].my_status == status::in_progress)
            number++;
    return number;
}

string get_status(status _status){
    if (_status == status::waiting)
        return "W";
    if (_status == status::in_progress)
        return "P";
    return "D";
}

string get_direction(direction _direction){
    if (_direction == direction::to_left)
        return "L";
    return "R";
}

// Mostra a legenda das informacoes
void Show_Subtitle(){
    cout << "==================================================================================================================" << endl;
    cout << "As informacoes acima estao impressas seguindo o padrao abaixo:" << endl;
    cout << "   |0:R,100,D| => |<I> : <II> , <III> , <IV>|" << endl;
    cout << "               I) Identificador numerico do babuino em questao" << endl;
    cout << "               II) Sentido de deslocamento do babuino (R = p/ direita; L = p/ esquerda)" << endl;
    cout << "               III) Posicao do babuino na corda: o da esquerda comeca com 0 e o da direita com o tamanho da corda" << endl;
    cout << "               IV) Status de travessia do babuino (W = esperando; P = atravessando/em progresso; D = feita)" << endl;
    cout << "==================================================================================================================" << endl;
    cout << "   Observacoes:" << endl;
    cout << "       1) ++ ou --: Indicam se eh um movimento para direita ou esquerda, respectivamente" << endl;
    cout << "       2) C: o numero de babuinos atualmente na corda" << endl;
    cout << "       3) ID: Identificador numerico do babuino em questao" << endl;
    cout << "==================================================================================================================" << endl;
}

// Exibe a seguinte sequência de exemplo:
//      |0:R,100,D|1:R,070,P|2:L,100,W|3:L,100,W|4:L,100,W|5:L,100,W|6:L,100,W|7:L,100,W|8:L,100,W|9:L,100,W|
void Show_Baboons_Informations(){
    for (int i = 0; i < BABOONS_AMOUNT; i++) {
        cout << setfill('0') << setw(Get_Digits_Number(BABOONS_AMOUNT)) << i << ":" << get_direction(baboons[i].my_direction) << "," << flush;  
        cout << setfill('0') << setw(Get_Digits_Number(ROPE_SIZE)) << baboons[i].position << "," << get_status(baboons[i].my_status) << "|" << flush;
    }
    cout << endl;
}

void Show_Unrepeated_Baboons(int _id, int _baboon_on_rope, direction _current_rope_direction){
    static int last_id = -1;
    //|0:R,100,D|1:R,070,P|2:L,100,W|3:L,100,W|4:L,100,W|5:L,100,W|6:L,100,W|7:L,100,W|8:L,100,W|9:L,100,W|
    if (_id != last_id){
        if (_current_rope_direction == direction::to_left)
            cout << "--" << flush;
        else
            cout << "++" << flush;

        cout << "ID=" << _id << "|C=" << _baboon_on_rope << " |" << flush;
        Show_Baboons_Informations();
    }
    last_id = _id;
}

// Realiza a travessia dos babuínos que estão na esqueda para a direita
void *Move_To_Right(void *arg) {
    int id = *((int *) arg); 

    for (int i = 0; i < ROPE_SIZE; i++) {
        pthread_mutex_lock(&lock);
        while (1){
            bool condA = baboons[id].my_status == status::in_progress;
            bool condB = current_rope_direction == direction::to_right;
            bool condC = baboon_on_rope == 0;
            bool condD = baboon_on_rope == ROPE_BABOON_LIMIT;
            
            if (((not condA) and (not condD) and (condB or condC)) or (condA and condB and (not condC)))
                break;
            else{
                cout << "++ID[" << id << "] Esperando" << endl;
                pthread_cond_wait(&available_rope, &lock);
            }
        }
        if (current_rope_direction != direction::to_right){
            current_rope_direction = direction::to_right;
            baboon_on_rope = 0;
            cout << "++ID[" << id << "] Inicio de nova corda" << endl;
        }
        if (baboons[id].my_status == status::waiting){
            baboon_on_rope++;
            baboons[id].my_status = status::in_progress;
        }
        baboons[id].position++;
        Show_Unrepeated_Baboons(id, baboon_on_rope, current_rope_direction);
    
        if (i+1 == ROPE_SIZE){
            baboon_on_rope--;
            baboons[id].my_status = status::done;
            cout << "++ID[" << id << "] completo; C="<< baboon_on_rope << "\n" << endl;
            pthread_cond_signal(&available_rope);
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

// Realiza a travessia dos babuínos que estão na direita para a esquerda
void *Move_To_Left(void *arg) {
    int id = *((int *) arg); 

    for (int i = 0; i < ROPE_SIZE; i++) {
        pthread_mutex_lock(&lock);
        while (1){
            bool condA = baboons[id].my_status == status::in_progress;
            bool condB = current_rope_direction == direction::to_left;
            bool condC = baboon_on_rope == 0;
            bool condD = baboon_on_rope == ROPE_BABOON_LIMIT;
            if (((not condA) and (not condD) and (condB or condC)) or (condA and condB and (not condC)))
                break;
            else{
                cout << "--ID[" << id << "] Esperando" << endl;
                pthread_cond_wait(&available_rope, &lock);
            }
        }
        if (current_rope_direction != direction::to_left){
            current_rope_direction = direction::to_left;
            baboon_on_rope = 0;
            cout << "--ID[" << id << "] Inicio de nova corda" << endl;
        }
        if (baboons[id].my_status == status::waiting){
            baboon_on_rope++;
            baboons[id].my_status = status::in_progress;
        }
        Show_Unrepeated_Baboons(id, baboon_on_rope, current_rope_direction);
        baboons[id].position--;
       
        if (i+1 == ROPE_SIZE){
            baboon_on_rope--;
            baboons[id].my_status = status::done;
            cout << "--ID[" << id << "] completo; C="<< baboon_on_rope << "\n" << endl;
            pthread_cond_signal(&available_rope);
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}
              

////////////////////////////////////////////////////////////////////////////
//  FUNÇÃO PRINCIPAL
////////////////////////////////////////////////////////////////////////////                                                               
int main(void) {          
    // Inicializa a semente aleatória
    //srand((unsigned) time(NULL));
    srand(123);

    pthread_cond_init(&available_rope, NULL);

    for (int i = 0; i < BABOONS_AMOUNT; i++){
        // Se o valor aleatório for par, o babuíno encontra-se inicialmente na esquerda (querendo ir para a direita)
        if (Get_Random_Value()%2==0){
            baboons[i].set_to_rigth();
            pthread_create(&(baboons[i].my_thread), NULL, Move_To_Right,(void *)(&(baboons[i].id)));
        }
        // Para o valor ímpar, o babuíno encontra-se inicialmente na direita (querendo ir para a esquerda)
        else{
            baboons[i].set_to_left();
            pthread_create(&(baboons[i].my_thread), NULL, Move_To_Left,(void *)(&(baboons[i].id)));
        }
    }

    for (int i = 0; i < BABOONS_AMOUNT; i++)
        pthread_join(baboons[i].my_thread, NULL);

    cout << endl << endl << endl;
    cout << "==================================================================================================================" << endl;
    cout << "CONCLUSAO DA TRAVESSIA DOS BABUINOS (STATUS FINAL ABAIXO)" << endl;
    cout << "==================================================================================================================" << endl;
    Show_Baboons_Informations();
    Show_Subtitle();

    return 0;
}