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
// Serão criadas N threads, onde cada babuíno será representado por uma thread
// Cada thread realiza o deslocamento do determinado babuíno:
//   - Os bauínos na esquerda (posição 0) vão para a direita (posição X), logo, quando posição = X que dizer que o babuíno atravessou
//   - Os bauínos na direita (posição X) vão para a esquerda (posição 0), logo, quando posição = 0 que dizer que o babuíno atravessou
// Existe uma variável de controle para o "direction" (definida por enum) indicando o sentido de movimento do babuíno:
//      * none      -> (só para inicialização) indica que ainda não houve nenhuma travessia
//      * to_left   -> indica que o babuíno em questão está indo da direita para a esquerda
//      * to_right  -> indica que o babuíno em questão está indo da esquerda para a direita
// Existe uma variável de controle para o "status" (definida por enum) indicando onde o babuíno está em relação à travessia:
//      * waiting       -> indica que o babuíno em questão está esperando pra usar a corda
//      * in_progress   -> indica que o babuíno em questão está atravessando pela corda
//      * done          -> indica que o babuíno em questão já atravessou a corda
// Deste modo, cada thread (babuíno) deverá possuir:
//      * direction (none | to_left | to_right)     : sentido de deslocamento na corda
//      * position  (0-X)                           : qual a posição do babuíno na corda
//      * status    (waiting | in_progress | done)  : relação do babuíno em relação à sua travessia


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

////////////////////////////////////////////////////////////////////////////
//  VARIÁVEIS GLOBAIS PRINCIPAIS
////////////////////////////////////////////////////////////////////////////
baboon baboons[BABOONS_AMOUNT];  
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t available_rope;

////////////////////////////////////////////////////////////////////////////
//  FUNÇÕES AUXILIARES GENÉRICAS
////////////////////////////////////////////////////////////////////////////
// A lógica dessa função foi baseada no seguinte site: https://www.bitdegree.org/learn/random-number-generator-cpp
int Get_Random_Value(){
    // Obtém um número randômico entre min e max
    return (rand() % MAX_RANDOM_VALUE) + MIN_RANDOM_VALUE;
}

// A lógica dessa função foi baseada no seguinte forúm: https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
int Get_Digits_Number(int number){
    return ((number > 0) ? (int) log10 ((double) number) + 1 : 1);
}

// Retorna o caractere printavel dependendo do status recebido
string get_status(status _status){
    if (_status == status::waiting)
        return "W";
    if (_status == status::in_progress)
        return "P";
    return "D";
}

// Retorna o caractere printavel dependendo da direção recebida
string get_direction(direction _direction){
    if (_direction == direction::to_left)
        return "L";
    return "R";
}

////////////////////////////////////////////////////////////////////////////
// FUNÇÕES DE EXIBIR INFORMAÇÕES NA TELA
////////////////////////////////////////////////////////////////////////////
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

// Mostra na tela somente o primeiro movimento do babuíno (para não poluir a análise)
void Show_Unrepeated_Baboons(int _id, int _baboon_on_rope, direction _current_rope_direction){
    static int last_id = -1;
    //|0:R,100,D|1:R,070,P|2:L,100,W|3:L,100,W|4:L,100,W|5:L,100,W|6:L,100,W|7:L,100,W|8:L,100,W|9:L,100,W|
    if (_id != last_id){
        if (_current_rope_direction == direction::to_left)
            cout << "--" << flush;
        else
            cout << "++" << flush;

        cout << "ID=" << setfill('0') << setw(Get_Digits_Number(BABOONS_AMOUNT)) << _id << "|C=" << _baboon_on_rope << " |" << flush;
        Show_Baboons_Informations();
    }
    last_id = _id;
}

////////////////////////////////////////////////////////////////////////////
// FUNÇÕES AUXILIARES DE GERENCIAMENTO DE ACESSO À CORDA
////////////////////////////////////////////////////////////////////////////
// Verifica se o babuíno em questão pode ou não acessar a corda. Retornando false ele pode
bool Wait_To_Use_Rope(int id){
    bool condA = baboons[id].my_status == status::in_progress;
    bool condB = current_rope_direction == baboons[id].my_direction;
    bool condC = baboon_on_rope == 0;
    bool condD = baboon_on_rope == ROPE_BABOON_LIMIT;
    
    if (((not condA) and (not condD) and (condB or condC)) or (condA and condB and (not condC)))
        return false;

    if (baboons[id].my_direction == direction::to_right)
        cout << "++" << flush;
    else
        cout << "--" << flush;
    cout << "ID[" << id << "] Esperando" << endl;
    pthread_cond_wait(&available_rope, &lock);
    return true;
}

// Atualiza informações de status do babuíno e quantidade de babuinos e direção do movimento na corda
void Update_Informations(int id){
    if (current_rope_direction != baboons[id].my_direction){
        current_rope_direction = baboons[id].my_direction;
        baboon_on_rope = 0;
        if (baboons[id].my_direction == direction::to_right)
            cout << "++ID[" << id << "] Novo conjunto de babuinos na corda indo para a direita" << endl;
        else
            cout << "--ID[" << id << "] Novo conjunto de babuinos na corda indo para a esquerda" << endl;
    }
    if (baboons[id].my_status == status::waiting){
        baboon_on_rope++;
        baboons[id].my_status = status::in_progress;
    }
}

// Movimenta o babuino na corda. Se ele estiver indo para a direta, soma, sendo para a esquerda, subtrai
void Move_Baboon(int id){
    if (baboons[id].my_direction == direction::to_right)
        baboons[id].position++;
    else
        baboons[id].position--;
}

// Caso seja a última posicao do babuíno na corda, envia um signal para indicar que há vaga disponível
void Send_Rope_Signal_Available(int id){
    int position = (baboons[id].my_direction == direction::to_right) ? baboons[id].position+1 : baboons[id].position-1;

    if (position == baboons[id].end_position){
        baboon_on_rope--;
        baboons[id].my_status = status::done;
        if (baboons[id].my_direction == direction::to_right)
            cout << "++" << flush;
        else
            cout << "--" << flush;
        cout << "ID[" << id << "] completou a travessia; Total de babuinos na corda = "<< baboon_on_rope << "\n" << endl;
        pthread_cond_signal(&available_rope);
    }
}

////////////////////////////////////////////////////////////////////////////
// FUNÇÃO DE GERENCIAMENTO DE ACESSO À CORDA
////////////////////////////////////////////////////////////////////////////
// Realiza a travessia dos babuínos que estão na esqueda para a direita
void *Manage_Rope_Crossing(void *arg) {
    int id = *((int *) arg); 

    for (int i = 0; i < ROPE_SIZE; i++) {
        pthread_mutex_lock(&lock);
        
        // Espera até ser possível que o babuíno possa usar a corda para atravessar
        while (Wait_To_Use_Rope(id));

        // Atualiza informações de status do babuíno e quantidade de babuinos e direção do movimento na corda
        Update_Informations(id);

        // Movimenta o babuíno na corda
        Move_Baboon(id);

        // Mostra na tela somente o primeiro movimento do babuíno (para não poluir a análise)
        Show_Unrepeated_Baboons(id, baboon_on_rope, current_rope_direction);
    
        // Caso seja a última posicao do babuíno na corda, envia um signal para indicar que há vaga disponível
        Send_Rope_Signal_Available(id);

        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}
              

////////////////////////////////////////////////////////////////////////////
//  FUNÇÃO PRINCIPAL
////////////////////////////////////////////////////////////////////////////                                                               
int main(void) {          
    // Inicializa a semente aleatória
    srand((unsigned) time(NULL));
    //srand(123);

    pthread_cond_init(&available_rope, NULL);

    for (int i = 0; i < BABOONS_AMOUNT; i++){
        // Se o valor aleatório for par, o babuíno encontra-se inicialmente na esquerda (querendo ir para a direita)
        if (Get_Random_Value()%2==0)
            baboons[i].set_to_rigth();
        // Para o valor ímpar, o babuíno encontra-se inicialmente na direita (querendo ir para a esquerda)
        else
            baboons[i].set_to_left();
        
        // Cria os babuínos de um a um
        pthread_create(&(baboons[i].my_thread), NULL, Manage_Rope_Crossing,(void *)(&(baboons[i].id)));
    }

    // Manda todos os babuínos aguardarem
    for (int i = 0; i < BABOONS_AMOUNT; i++)
        pthread_join(baboons[i].my_thread, NULL);

    // Exibe o resultado final e algumas informações extras
    cout << endl << endl << endl;
    cout << "==================================================================================================================" << endl;
    cout << "CONCLUSAO DA TRAVESSIA DOS BABUINOS (STATUS FINAL ABAIXO)" << endl;
    cout << "==================================================================================================================" << endl;
    Show_Baboons_Informations();
    Show_Subtitle();

    return 0;
}