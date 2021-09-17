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

#ifndef ROPE
    #define ROPE

    #include "constants_and_types.hpp"

    class rope{
        public:

    };
#endif