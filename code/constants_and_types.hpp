////////////////////////////////////////////////////////////////////////////
//  Universidade Federal do Rio de Janeiro
//  Disciplina: Sistemas Operacionais - EEL770
//  Professor: Rodrigo Souza Couto
//  Desenvolvedor: Chritian Marques de Oliveira Silva
//  DRE: 117.214.742
//  Trabalho 2: Locks e Variáveis de Condição - Problema dos babuínos
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANTS_AND_TYPES
    #define CONSTANTS_AND_TYPES

    ////////////////////////////////////////////////////////////////////////////
    //  TIPOS
    ////////////////////////////////////////////////////////////////////////////
    enum class direction{none, to_left, to_right};
    enum class status{waiting, in_progress, done};

    ////////////////////////////////////////////////////////////////////////////
    //  VARIAVEIS/DEFINIÇÕES GLOBAIS
    ////////////////////////////////////////////////////////////////////////////
    // ------------------------
    // Ajuste da simulação: quantidade de babuínos, o limite de babuínos na corda por vez e o tamanho da corda, respectivamente
    // ------------------------
    const int BABOONS_AMOUNT = 13;
    const int ROPE_BABOON_LIMIT = 5;
    const int ROPE_SIZE = 300;
    // ------------------------

    int baboon_on_rope = 0;
    direction current_rope_direction = direction::none;
    const unsigned char MIN_RANDOM_VALUE = 1;
    const unsigned char MAX_RANDOM_VALUE = 100;

#endif