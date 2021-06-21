#include <random>
#include <algorithm>
#include "header.hpp"



void initGame(Area &area, const size_t table_size, const size_t win_line_length) {

   area.victory = win_line_length;
   area.cell.dX = area.cell.dY = table_size;

   area.table = new (std::nothrow) STATE * [sizeof(STATE *)*area.cell.dX];
      if (!area.table)
         {
            std::cerr << KRED;
            std::cerr << "Cant allocate memory for \'area.table\'." << std::endl;
            exit (EXIT_FAILURE);
         }

   for (size_t y{}; y < area.cell.dX; ++y) 
      {

       *(area.table + y) = new (std::nothrow) STATE[sizeof(STATE)*area.cell.dY];
         if (!area.table[y])
            {
               std::cerr << KRED;
               std::cerr << "Cant allocate memory for \'area.table[" 
               << y << "].\'" << std::endl;

               for (size_t go_back{y - 1};go_back >= 0; go_back--)
                  {
                     delete []area.table[go_back];
                     area.table[go_back] = nullptr;
                  }

               delete []area.table;
               area.table = nullptr;

               exit(EXIT_FAILURE);
            }

       for (size_t x{}; x < area.cell.dY; ++x) 
          {
           setCellData(EMPTY_MARK, area.table, {{y}, {x}});
          }
      }
   
}

void freeAtEnd(Area &area) {
	
   for (size_t y{}; y < area.cell.dX; ++y)
		   {
		   delete [] area.table[y];
		   area.table[y] = nullptr;
		}

	delete [] area.table;
	area.table = nullptr;

}

void refreshScreen(Area &area) {

   char temp;
   int system_status;

   system_status = std::system(CLEAR_SCR);

   if (system_status < 0) //Most cases in all operation systems.
                          //Tbh "most cases" for 'system' is -1,
                          //but returned error value of a child
                          //process may be different.
      {
         std::cout << KRED;
         std::cout <<  its_pitty;
         freeAtEnd(area);
         exit (EXIT_FAILURE);
      }
   
#if defined (__unix__) //POSIX only

//Code 127: Cant create shell environment in a forked process.
//So if we cant create a shell, then we cant run 'clear' on it too.
//(Tbh there is a possibility that a command inside 'system' may
//return 127, but let's believe that won't ever happen xD )
//
//And if we cant get exit status of a command (not 'system') that
//we tried to run using 'system' there is an error too.
   if (system_status == 127 || !WIFEXITED(system_status)) 
                             
      {
         std::cout << KRED;
         std::cout <<  its_pitty;
         freeAtEnd(area);
         exit (EXIT_FAILURE);
      }

#endif //__unix__
   
   std::cout << KMAG;
   size_t k{};

   std::cout << "   " << ++k; 
   for (; k < area.cell.dX; ++k)
      {
      std::cout << " ";
      std::cout << k + 1;
      }
   std::cout << std::endl; 

   for (size_t i{}; i < area.cell.dY; ++i) {
       if (i < 9) //I hope the User wont order a table more than 99 xD
          std::cout << KMAG << i+1 << " "; 
       else 
         std::cout << KMAG << i+1;

       std::cout << KYEL << "|";
       for (size_t j{}; j < area.cell.dX; ++j) 
       {
         temp = getCellData(area.table, {{i}, {j}});
         switch (temp)
         {
         case HUMAN_MARK:
            std::cout << KGRN;
            break;
         case BOT_MARK:
            std::cout << KRED;
            break;
         default: //EMPTY_MARK
            std::cout << KYEL;
            break;
         }
         std::cout << temp;
         std::cout << KYEL << '|';
       }
       std::cout << std::endl;
   }
   std::cout << KNRM;
}

void showErrMsg (const std::string s)
	{
		std::cout << KRED;
		std::cout << "You've entered not correct value for ";
		std::cout << s << ". ";
		std::cout << "Try again." << std::endl;
		std::cout << KNRM;

	}
   
size_t getIntOnly(const std::string s)
	{

	size_t ret_val;

	std::cin >> ret_val;
	while (std::cin.fail())
		{
		showErrMsg(s);
		std::cout << "-> ";
		std::cin.clear();
		std::cin.ignore(32767,'\n');
		std::cin >> ret_val;
		}

	return ret_val;
	}

bool humanTurn(Area &area) {

   CELL cell;


   do {

	   std::cout << "Choose a cell's coordinates in range ";
	   std::cout << KMAG;
	   std::cout << "1 - "<< area.cell.dY; 
	   std::cout << KNRM;
	   std::cout << ":" << std::endl;

	   std::cout << "Enter ";
	   std::cout << KMAG << "X" << KNRM << " -> ";
	   cell.dY = getIntOnly("dimension X");

	   std::cout << "Enter " << KMAG;
	   std::cout << "Y " << KNRM << "-> ";
	   cell.dX = getIntOnly("dimension Y");

      cell.dX--; 
	   cell.dY--;

	   if (!isValidCell(area, cell)) 
         {
         refreshScreen(area);
	   	showErrMsg("cell");			 
         }

   } while (!isValidCell(area, cell) || !isCellEmpty(area, cell));

   setCellData(HUMAN_MARK, area.table, cell);

   return true;
}

bool botTurn(Area &area) {

   const CELL key_cells [] = {

         {{area.cell.dX/2},{area.cell.dY/2}},
         {{0},{0}},
         {{0},{area.cell.dY - 1}},
         {{area.cell.dX - 1},{0}},
         {{area.cell.dX - 1},{area.cell.dY - 1}},

   };

   CELL free_key_cells[sizeof (key_cells)/sizeof(*key_cells)];
   size_t fkc_iter {0};

   CELL cell;

   for (auto& x : key_cells)
      {
         if (getCellData(area.table,x) == EMPTY_MARK)
            {
              free_key_cells[fkc_iter] = x;
              fkc_iter++;
            }
      }

   if (fkc_iter)
      {
         setCellData(BOT_MARK, area.table, free_key_cells[getRandom(fkc_iter)]);
         return false;
      }

   do {

     cell.dX = getRandom(area.cell.dX);
     cell.dY = getRandom(area.cell.dY);

   } while (!isCellEmpty(area, cell));

   setCellData(BOT_MARK, area.table, cell);

   return false;
}

bool isVictory(const STATE ch, Area &area) {

   for (size_t y{}; y < area.cell.dX; y++) 
   	{
       for (size_t x{}; x < area.cell.dY; x++) 
	   {
           if (isLineFull(ch, area, y, x, 1, 0, area.victory)) return true;
           if (isLineFull(ch, area, y, x, 1, 1, area.victory)) return true;
           if (isLineFull(ch, area, y, x, 0, 1, area.victory)) return true;
           if (isLineFull(ch, area, y, x, 1, -1, area.victory)) return true;
       }
   }
   return false;
}

bool isLineFull(const STATE ch, Area &area, const size_t y, const size_t x,
                const size_t vx, const size_t vy, const size_t length) 
{

   CELL last;
   last.dX = x + (length - 1) * vx;
   last.dY = y + (length - 1) * vy;

   if (!isValidCell(area, last))
       return false;

   for (size_t i{}; i < length; i++)
   		{
          if (getCellData(area.table, {{y + i * vy}, {x + i * vx}}) != ch)
               return false;
		}

   return true;
}

bool isDraw(const Area &area) {

   for (size_t y{}; y < area.cell.dY; y++)
       for (size_t x{}; x < area.cell.dX; x++)
           if (isCellEmpty(area, {{x}, {y}}))
               return false;

   return true;
}

void game_process(const size_t table_size, const size_t win_line_length) {

   Area area;
   char ask;
   bool whose_move;

   std::string HUMAN_GRATS (KGRN); 
   HUMAN_GRATS.append("You have won, congrats!");

   std::string MACHINE_GRATS (KRED); 
   MACHINE_GRATS.append("Machine has won!");

   whose_move = (bool)getRandom(2); //бросаем жребий

   for (;;)
   {

       initGame(area, table_size, win_line_length);
       refreshScreen(area);

       while (true) 
       {

          if (!whose_move)
            {
            whose_move = humanTurn(area);
            refreshScreen(area);
   
            if (isGameEndedUp(HUMAN_MARK, area, HUMAN_GRATS)) break;
            }

         else if (whose_move)
            {
            whose_move = botTurn(area);
            refreshScreen(area);
   
            if (isGameEndedUp(BOT_MARK, area, MACHINE_GRATS)) break;
            }

       }

      std::cout << KNRM << "Do you want to play more? ";

	   freeAtEnd(area);

      std::cin >> ask;

      ask = tolower(ask);
      std::cin.ignore(32767, '\n');
      std::cin.clear();
      
      if (ask == 'y')
      	continue;
      else
         break;
      
   }
}

bool isGameEndedUp(STATE mark, Area &area, const std::string congrats) 
{
   if (isVictory(mark, area)) {
       std::cout << congrats << std::endl;
       return true;
   }
   if (isDraw(area)) {

       std::cout << KYEL;
       std::cout << "Draw." << std::endl;
       std::cout << KNRM;

       return true;
   }
   return false;
}

inline char getCellData(STATE** const array, const CELL cell) {

	char ch;

	ch = *((*(array + cell.dX) + cell.dY));

   return ch;
}

inline void setCellData(STATE data, STATE** array, const CELL cell) {

	*((*(array + cell.dX)) + cell.dY) = data;

}


bool isValidCell(const Area &area, const CELL cell)
	{

	bool isOK {false};

	if ((cell.dX >= 0 && cell.dX < area.cell.dX) &&
		(cell.dY >= 0 && cell.dY < area.cell.dY))
		isOK = true;

	return isOK;
	}

bool isCellEmpty(const Area &area, const CELL cell) {

   return (bool)(getCellData(area.table, cell) == EMPTY_MARK);

}

void usage (char ** argv){

   if (!strcmp(argv[1], "--help"))
      {
         std::cout << "Usage : " <<std::endl << KCYN << argv[0];
         std::cout << " [length of game table]" << KNRM;
         std::cout << std::endl << "Or:" << std::endl << KCYN;
         std::cout << argv[0] <<" [length of game table]";
         std::cout << " [length of a line to win]";
         std::cout << std::endl << KNRM;
         std::cout << "But if you issue only " << KCYN << argv[0] <<
                KNRM << ", " << "the game starts as: " << KCYN << 
                argv[0] << " 3" << " 3" << std::endl;

         exit(EXIT_FAILURE);
      }

   if (atoi(argv[1]) < 2)
      {
         std::cout << KRED;
         std::cout << "Game makes sense only on a table that has";
         std::cout << " a size more than " << KCYN << "2x2.";
         std::cout << std::endl;

         exit (EXIT_FAILURE);
      }
}

size_t getRandom (const size_t max)
   {
      
   std::random_device rd;
   std::mt19937 mt(rd());
   std::uniform_real_distribution <double> dist(0, max);

   return dist(mt);   
   }
