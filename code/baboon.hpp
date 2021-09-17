////////////////////////////////////////////////////////////////////////////
//  Universidade Federal do Rio de Janeiro
//  Disciplina: Sistemas Operacionais - EEL770
//  Professor: Rodrigo Souza Couto
//  Desenvolvedor: Chritian Marques de Oliveira Silva
//  DRE: 117.214.742
//  Trabalho 2: Locks e Variáveis de Condição - Problema dos babuínos
////////////////////////////////////////////////////////////////////////////

#ifndef BABOON
    #define BABOON

    #include <string>
    #include "constants_and_types.hpp"
    using namespace std;
    
    class baboon {
        public:
            static int baboons_number;
            pthread_t my_thread;
            direction my_direction;
            status my_status = status::waiting;
            int start_position;
            int position;
            int id;

            baboon(){
                id = baboons_number;
                baboons_number++;
            }
            void set_to_rigth(){
                position = 0;
                start_position = position;
                my_direction = direction::to_right;
            }
            void set_to_left(){
                position = ROPE_SIZE;
                start_position = position;
                my_direction = direction::to_left;
            }
    };

    int baboon::baboons_number = 0;

#endif