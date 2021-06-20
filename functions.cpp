#include <random>
#include <algorithm>
#include "header.hpp"



void initGame(Area &area, const size_t table_size, const size_t win_line_length) {

   area.victory = win_line_length;
   area.dX = area.dY = table_size;

   area.table = new (std::nothrow) STATE * [sizeof(STATE *)*area.dY];

   for (size_t y{}; y < area.dY; ++y) 
      {

       *(area.table + y) = new (std::nothrow) STATE[sizeof(STATE)*area.dX];

       for (size_t x{}; x < area.dX; ++x) 
          {
           setCellData(EMPTY_MARK, area.table, y, x);
          }
      }
}

void freeAtEnd(Area &area) {
	
   for (size_t y{}; y < area.dY; ++y)
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
   for (; k < area.dX; ++k)
      {
      std::cout << " ";
      std::cout << k + 1;
      }
   std::cout << std::endl; 

   for (size_t i{}; i < area.dY; ++i) {
       if (i < 9) //I hope the User wont order a table more than 99 xD
          std::cout << KMAG << i+1 << " "; 
       else 
         std::cout << KMAG << i+1;

       std::cout << KYEL << "|";
       for (size_t j{}; j < area.dX; ++j) 
       {
         temp = getCellData(area.table, i, j);
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

   size_t x, y;

   do {

	   std::cout << "Choose a cell's coordinates in range ";
	   std::cout << KMAG;
	   std::cout << "1 - "<< area.dY; 
	   std::cout << KNRM;
	   std::cout << ":" << std::endl;

	   std::cout << "Dimension ";
	   std::cout << KMAG << "X" << KNRM << " -> ";
	   x = getIntOnly("dimension X");

	   std::cout << "Dimension " << KMAG;
	   std::cout << "Y " << KNRM << "-> ";
	   y = getIntOnly("dimension Y");

      x--; 
	   y--;

	   if (!isValidCell(area, x, y)) 
         {
         refreshScreen(area);
	   	showErrMsg("cell");			 
         }

   } while (!isValidCell(area, x, y) || !isCellEmpty(area, x, y));

   setCellData(HUMAN_MARK, area.table, y, x);

   return true;
}

bool botTurn(Area &area) {

   size_t x;
   size_t y;

   if (getCellData(area.table,area.dX/2, area.dY/2) == EMPTY_MARK)
      {
         setCellData(BOT_MARK, area.table, area.dX/2, area.dY/2);
         return false;
      }
     
   else if (getCellData(area.table,0,0) == EMPTY_MARK)
      {
         setCellData(BOT_MARK, area.table, 0,0);
         return false;
      }
   else if (getCellData(area.table,0,area.dY - 1) == EMPTY_MARK)
      {
         setCellData(BOT_MARK, area.table, 0, area.dY - 1);
         return false;
      } 
   else if (getCellData(area.table, area.dX - 1, 0) == EMPTY_MARK)
     {
         setCellData(BOT_MARK, area.table, area.dX - 1,0);
         return false;
     }
   else if (getCellData(area.table, area.dX - 1, area.dY - 1) == EMPTY_MARK)
    {
         setCellData(BOT_MARK, area.table, area.dX - 1, area.dY - 1);
         return false;
    }
   
   do {

     x = getRandom(area.dX);
     y = getRandom(area.dY);

   } while (!isCellEmpty(area, x, y));

   setCellData(BOT_MARK, area.table, y, x);

   return false;
}

bool isVictory(const STATE ch, Area &area) {

   for (size_t y{}; y < area.dX; y++) 
   	{
       for (size_t x{}; x < area.dY; x++) 
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

   size_t lastX = x + (length - 1) * vx;
   size_t lastY = y + (length - 1) * vy;

   if (!isValidCell(area, lastX, lastY))
       return false;

   for (size_t i{}; i < length; i++)
   		{
          if (getCellData(area.table, (y + i * vy), (x + i * vx)) != ch)
               return false;
		}

   return true;
}

bool isDraw(const Area &area) {

   for (size_t y{}; y < area.dY; y++)
       for (size_t x{}; x < area.dX; x++)
           if (isCellEmpty(area, x, y))
               return false;

   return true;
}

void game_process(const size_t table_size, const size_t win_line_length) {

   Area area;
   char ask;
   bool which_turn;

   std::string HUMAN_GRATS (KGRN); 
   HUMAN_GRATS.append("You have won, congrats!");

   std::string MACHINE_GRATS (KRED); 
   MACHINE_GRATS.append("Machine has won!");

   which_turn = (bool)getRandom(2); //бросаем жребий

   for (;;)
   {

       initGame(area, table_size, win_line_length);
       refreshScreen(area);

       while (true) 
       {

          if (!which_turn)
            {
            which_turn = humanTurn(area);
            refreshScreen(area);
   
            if (isGameEndedUp(HUMAN_MARK, area, HUMAN_GRATS)) break;
            }

         else if (which_turn)
            {
            which_turn = botTurn(area);
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

inline char getCellData(STATE** const array, const size_t x, const size_t y) {

	char ch;

	ch = *((*(array + x)) + y);

   return ch;
}

inline void setCellData(STATE data, STATE** array, const size_t x, const size_t y) {

	*((*(array + x)) + y) = data;

}


bool isValidCell(const Area &area, const size_t x, const size_t y)
	{

	bool isOK {false};

	if ((x >= 0 && x < area.dX) &&
		(y >= 0 && y < area.dY))
		isOK = true;

	return isOK;
	}

bool isCellEmpty(const Area &area, const size_t x, const size_t y) {

   return (bool)(getCellData(area.table, y, x) == EMPTY_MARK);

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
