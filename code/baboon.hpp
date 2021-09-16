#ifndef BABOON
    #define BABOON

    enum class direction{to_left, to_right};
    enum class status{waiting, in_progress, done};
    
    class baboon {
        public:
            static int baboons_number;
            static const int ROPE_SIZE = 5;
            pthread_t my_thread;
            direction my_direction;
            int position;
            int id;

            baboon(){
                id = baboons_number;
                baboons_number++;
            }
            void set_to_rigth(){
                position = 0;
                my_direction = direction::to_right;
            }
            void set_to_left(){
                position = ROPE_SIZE;
                my_direction = direction::to_left;
            }
            bool is_completed(){
                if ((my_direction == direction::to_right) and (position == ROPE_SIZE))
                    return true;
                if ((my_direction == direction::to_left) and (position == 0))
                    return true;
                return false;
            }
    };

    int baboon::baboons_number = 0;

#endif