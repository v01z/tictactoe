#include "header.hpp"


int main(int argc, char ** argv)
{

    if (argc == 2) 
        {
            
            usage(argv);

            game_process(atoi(argv[1]), atoi(argv[1]));

            return EXIT_SUCCESS;
        }
    
    else if (argc >= 3) //Let the User is able to add more arguments than 2 if he wants xD
        {
            usage(argv);

            if (atoi(argv[2]) > atoi (argv[1]))
                {
                   std::cout << KRED;
                   std::cout << "Length of win line shouldn't be more ";
                   std::cout << "than length of a table size." << std::endl;

                   return EXIT_FAILURE;
                }

            game_process(atoi(argv[1]), atoi(argv[2]));

        }

	game_process(3,3); //default behavior of game start

    return EXIT_SUCCESS;
}
