#include <random>
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

bool  (*ptrFunc [PTR_FUNC_SIZE]) (const Area &area, const bool direction,
    const STATE state, const bool force);

////////////////////
ptrFunc[0] = &messUpPlans_pos;
ptrFunc[1] = &messUpPlans_neg;
ptrFunc[2] = &messUpPlans_pos_diag;
ptrFunc[3] = &messUpPlans_neg_diag;
ptrFunc[4] = &messUpPlans_pos_diag_anti;
ptrFunc[5] = &messUpPlans_neg_diag_anti;
/////////////
ptrFunc[6] = &messUpPlans_pos;
ptrFunc[7] = &messUpPlans_neg; 
///////////////////

   CELL free_key_cells[sizeof (key_cells)/sizeof(*key_cells)];
   size_t fkc_iter {0};

   CELL cell;

//In case two marks have been settled by Bot, it will 
//set another one:
//--------------------------------------------------

for (size_t i{}; i < PTR_FUNC_SIZE - 2; ++i)
      if (ptrFunc[i] (area,true,BOT_MARK,false))
         return false;

for (size_t i{PTR_FUNC_SIZE - 2}; i < PTR_FUNC_SIZE; ++i)
      if (ptrFunc[i] (area,false,BOT_MARK,false))
         return false;


 // Lets check wether Human has alredy got 2 marks.
 // If so then the Bot has to mess up him:
 //--------------------- 

for (size_t i{}; i < PTR_FUNC_SIZE - 2; ++i)
      if (ptrFunc[i] (area,true,HUMAN_MARK,false))
         return false;

for (size_t i{PTR_FUNC_SIZE - 2}; i < PTR_FUNC_SIZE; ++i)
      if (ptrFunc[i] (area,false,HUMAN_MARK,false))
         return false;

   //If everything is OK (there are no bot-lines with two marks and
   //Human has not lines with two makrs too) the Bot will append one 
   //mark to to any other one: force = true.
   //--------------------------------------------------

for (size_t i{}; i < PTR_FUNC_SIZE - 2; ++i)
      if (ptrFunc[i] (area,true,BOT_MARK,true))
         return false;

for (size_t i{PTR_FUNC_SIZE - 2}; i < PTR_FUNC_SIZE; ++i)
      if (ptrFunc[i] (area,false,BOT_MARK,true))
         return false;

   //If Bot doesnt have any marks on a table game, it
   //will settle one in random mode on a key position.

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

//If all key positions are occupied, Bot will settle mark
//in a random mode anywhere on a game table.
//На самом деле эта ситуация will never happen. Но я оставил
//этот код "на всякий случай" (на будущее развитие программы)
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
           if (isLineFull(ch, area, {{y}, {x}}, 1, 0, area.victory)) return true;
           if (isLineFull(ch, area, {{y}, {x}}, 1, 1, area.victory)) return true;
           if (isLineFull(ch, area, {{y}, {x}}, 0, 1, area.victory)) return true;
           if (isLineFull(ch, area, {{y}, {x}}, 1, -1, area.victory)) return true;
       }
   }
   return false;
}

bool isLineFull(const STATE ch, Area &area, const CELL cell,
                const size_t vx, const size_t vy, const size_t length) 
{

   CELL last;
   last.dX = cell.dX + (length - 1) * vx;
   last.dY = cell.dY + (length - 1) * vy;

   if (!isValidCell(area, last))
       return false;

   for (size_t i{}; i < length; i++)
   		{
          if (getCellData(area.table, {{cell.dY + i * vy}, {cell.dX + i * vx}}) != ch)
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
   char answer;
   bool whose_move;

   std::string HUMAN_GRATS (KGRN); 
   HUMAN_GRATS.append("You have won, congrats!");

   std::string MACHINE_GRATS (KRED); 
   MACHINE_GRATS.append("Machine has won!");

//   whose_move = (bool)getRandom(2); //бросаем жребий

   for (;;)
   {

       initGame(area, table_size, win_line_length);
       refreshScreen(area);

       whose_move = (bool)getRandom(2); //бросаем жребий
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


      std::cin >> answer;

      answer = tolower(answer);
      std::cin.ignore(32767,'\n');
      std::cin.clear();
      
      if (answer == 'y')
         {
      	continue;
         }
      else
         {
            std::cout << "See you later!" << std::endl;
            exit (EXIT_SUCCESS);
         }
      
   }
}

bool isGameEndedUp(const STATE mark, Area &area, const std::string congrats) 
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

inline void setCellData(const STATE data, STATE** array, const CELL cell) {

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

bool messUpPlans_pos(const Area &area, const bool is_vert, 
   const STATE state, const bool force)
   {

      size_t Dx1{1};
      size_t Dy1{0};
      size_t Dx2{2};
      size_t Dy2{0};

      if (!is_vert)
         {
            Dx1 = 0;
            Dy1 = 1;
            Dx2 = 0;
            Dy2 = 2;
         }
      
      for (size_t x{}; x < area.cell.dX; ++x)
         {
            for (size_t y{}; y < area.cell.dY; ++y)
               {
               if(isValidCell(area,{{x},{y}}) && getCellData(area.table, {{x}, {y}}) == state) 
                  {
                     if (isValidCell(area,{{x+Dx1},{y+Dy1}}))
                        {
                           if ((state == BOT_MARK) && (force) && (getCellData(area.table, {{x+Dx1},{y+Dy1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x+Dx1},{y+Dy1}});
                                 return true;
                              }
                           if (getCellData(area.table, {{x+Dx1},{y+Dy1}}) == state)
                           {
                              if (isValidCell(area,{{x+Dx2},{y+Dy2}}) && getCellData(area.table,{{x+Dx2},{y+Dy2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x+Dx2}, {y+Dy2}});
                                    return true;
                                 }
                           }
                        }
                  }
               
            }   
         }

      return false;

   }

bool messUpPlans_neg(const Area &area, bool const is_vert, 
   const STATE state, const bool force)
   {

      size_t Dx1{1};
      size_t Dy1{0};
      size_t Dx2{2};
      size_t Dy2{0};

      if (!is_vert)
         {
            Dx1 = 0;
            Dy1 = 1;
            Dx2 = 0;
            Dy2 = 2;
         }
      
      for (int x{(int)area.cell.dX - 1}; x >= 0; --x) //Swtiched to 'int' cause if 'size_tl is less than zero then
         {                                       //segmentation fault is обеспечен.
            for (int y{(int)area.cell.dY - 1}; y >= 0; --y)
               {
               if(isValidCell(area,{{(size_t)0+ x},{(size_t)0 + y}}) && 
                  getCellData(area.table, {{(size_t)0 + x}, {(size_t)0 + y}}) == state) 
                  {
                     if (isValidCell(area,{{x-Dx1},{y-Dy1}}))
                        {
                           if ((state == BOT_MARK) && (force) && (getCellData(area.table, {{x-Dx1},{y-Dy1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x-Dx1},{y-Dy1}});
                                 return true;
                              }
                           if (getCellData(area.table, {{x-Dx1},{y-Dy1}}) == state)
                              {
                              if (isValidCell(area,{{x-Dx2},{y-Dy2}}) && getCellData(area.table,{{x-Dx2},{y-Dy2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x-Dx2}, {y-Dy2}});
                                    return true;
                                 }
                              }
                        }
                  }
               }
         }

      return false;

   }

bool messUpPlans_pos_diag(const Area &area, const bool from_0_0, const STATE state, const bool force)
   {

      size_t Dx1{0};
      size_t Dy1{0};
      size_t Dx2{area.cell.dX};
      size_t Dy2{area.cell.dY};

/* Тут, конечно, есть куда расти, но для наших целей и такой реализации будет достаточно xD
      if (!from_0_0)
         {
            Dx1 = area.cell.dX;
            Dy1 = area.cell.dY;
            Dx2 = 0;
            Dy2 = 0;
         }
         */
      
//      for (size_t x{}, y{}; x < area.cell.dX && y < area.cell.dY; ++x, ++y)
      for (size_t x{Dx1}, y{Dy1}; x < Dx2 && y < Dy2; ++x, ++y)
         {
               if(isValidCell(area,{{x},{y}}) && getCellData(area.table, {{x}, {y}}) == state) 
                  {
                     if (isValidCell(area,{{x+1},{y+1}}))
                        {
                           if ((state == BOT_MARK) && (force) && (getCellData(area.table, {{x+1},{y+1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x+1},{y+1}});

                                 return true;
                              }
                             
                           if (getCellData(area.table, {{x+1},{y+1}}) == state)
                              {
                              if (isValidCell(area,{{x+2},{y+2}}) && getCellData(area.table,{{x+2},{y+2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x+2}, {y+2}});
                                    return true;
                                 }
                              }
                        }
                  }
         }

      return false;

   }
bool messUpPlans_neg_diag(const Area &area, const bool from_max_max, const STATE state, const bool force)
   {

/*
      size_t Dx1{area.cell.dX};
      size_t Dy1{area.cell.dY};
      size_t Dx2{0};
      size_t Dy2{0}

      if (!from_max_max)
         {
            Dx1 = 0;
            Dy1 = 0;
            Dx2 = area.cell.dX;
            Dy2 = area.cell.dY;
         }
         */
      
      for (int x{(int)area.cell.dX}, y{(int)area.cell.dY}; x >= 0 && y >= 0; --x, --y)
         {
               if(isValidCell(area,{{x + (size_t) 0 },{y + (size_t)0 }}) 
                  && getCellData(area.table, {{x + (size_t) 0 }, {y + (size_t) 0 }}) == state) 
                  {
                     if (isValidCell(area,{{x- (size_t) 1},{y - (size_t) 1}}))
                        {
                           if ((state == BOT_MARK) && (force) &&
                               (getCellData(area.table, {{x-(size_t)1},{y-(size_t)1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x-(size_t)1},{y-(size_t)1}});

                                 return true;
                              }
                             
                           if (getCellData(area.table, {{x-(size_t)1},{y-(size_t)1}}) == state)
                              {
                              if (isValidCell(area,{{x-(size_t)2},{y-(size_t)2}})
                                     && getCellData(area.table,{{x-(size_t)2},{y-(size_t)2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x-(size_t)2}, {y-(size_t)2}});
                                    return true;
                                 }
                              }
                        }
                  }
         }

      return false;

   }

bool messUpPlans_pos_diag_anti(const Area &area, const bool direction, const STATE state, const bool force)
   {

         
      for (int x{(int)area.cell.dX}, y{}; x >= 0 && y < (int)area.cell.dY; --x, ++y)
         {
               if(isValidCell(area,{{x+(size_t)0},{y+(size_t(0))}}) &&
                   getCellData(area.table, {{x+(size_t)0}, {y+(size_t)0}}) == state) 
                  {
                     if (isValidCell(area,{{x-(size_t)1},{y+(size_t)1}}))
                        {
                           if ((state == BOT_MARK) && (force) &&
                               (getCellData(area.table, {{x-(size_t)1},{y+(size_t)1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x-(size_t)1},{y+(size_t)1}});

                                 return true;
                              }
                             
                           if (getCellData(area.table, {{x-(size_t)1},{y+(size_t)1}}) == state)
                              {
                              if (isValidCell(area,{{x-(size_t)2},{y+(size_t)2}}) &&
                                     getCellData(area.table,{{x-(size_t)2},{y+(size_t)2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x-(size_t)2}, {y+(size_t)2}});
                                    return true;
                                 }
                              }
                        }
                  }
         }

      return false;

   }
bool messUpPlans_neg_diag_anti(const Area &area, const bool direction, const STATE state, const bool force)
   {


      
      for (int x{}, y{(int)area.cell.dY}; x < (int)area.cell.dX && y >= 0; ++x, --y)
         {
               if(isValidCell(area,{{x + (size_t) 0 },{y + (size_t)0 }}) 
                  && getCellData(area.table, {{x + (size_t) 0 }, {y + (size_t) 0 }}) == state) 
                  {
                     if (isValidCell(area,{{x + (size_t) 1},{y - (size_t) 1}}))
                        {
                           if ((state == BOT_MARK) && (force) &&
                               (getCellData(area.table, {{x+(size_t)1},{y-(size_t)1}}) == EMPTY_MARK))
                              {
                                 setCellData(BOT_MARK, area.table,{{x+(size_t)1},{y-(size_t)1}});

                                 return true;
                              }
                             
                           if (getCellData(area.table, {{x+(size_t)1},{y-(size_t)1}}) == state)
                              {
                              if (isValidCell(area,{{x+(size_t)2},{y-(size_t)2}})
                                     && getCellData(area.table,{{x+(size_t)2},{y-(size_t)2}}) == EMPTY_MARK)
                                 {
                                    setCellData(BOT_MARK,area.table, {{x+(size_t)2}, {y-(size_t)2}});
                                    return true;
                                 }
                              }
                        }
                  }
         }

      return false;

   }

bool highQualityAI (bool dir, STATE who, bool force)
   {

   }