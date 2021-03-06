#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include "cross_platf_vars.hpp"

enum STATE {HUMAN_MARK='X',BOT_MARK='O', EMPTY_MARK='_'};

typedef struct {

   size_t dX;
   size_t dY;

} CELL;

#pragma pack (push, 1)
typedef struct {

   STATE** table;
   CELL cell;
   size_t victory;

} Area;
#pragma pack (pop)


const std::string its_pitty { "Cant clear screen. But it is necessary for our game, sorry." };

const size_t KEY_POS_SIZE { 5 };

const size_t PTR_FUNC_SIZE { 8 };

inline char getCellData (STATE ** const array, const CELL cell);

inline void setCellData(const STATE data, STATE ** array, const CELL cell);

bool isValidCell(const Area &area, const CELL cell);

bool isCellEmpty(const Area &area, const CELL cell);

size_t getIntOnly(const std::string str);

void initGame(Area &area, size_t const size_of_table, const size_t size_of_line2win);

void freeAtEnd(Area &area);


void refreshScreen(Area &area);


bool humanTurn(Area &area);


bool botTurn(Area &area, const CELL pos [], bool  (*ptrFunc [PTR_FUNC_SIZE])
 (const Area &area, const bool direction, const STATE state, const bool force));


bool isVictory(const STATE ch, Area &area);


bool isLineFull(const STATE ch, Area &area, const CELL cell, 
                 const size_t vx, const size_t vy, const size_t length);


bool isDraw(const Area &area);


void game_process(const size_t table_size, const size_t line_length);


bool isGameEndedUp(const STATE mark, Area &area, const std::string str);


void showErrMsg(const std::string str);

void usage(char ** argv);//It is possible to make argv as 'const',
                         //but traditions demands not to do so.

size_t getRandom (const size_t maximum);


bool messUpPlans_pos(const Area &area, const bool horiz,
    const STATE state, const bool force);

bool messUpPlans_neg(const Area &area, const bool horiz,
    const STATE state, const bool force);

bool messUpPlans_pos_diag(const Area &area,
 const bool direction, const STATE state, const bool force);

bool messUpPlans_neg_diag(const Area &area, 
 const bool direction, const STATE state, const bool force);

bool messUpPlans_pos_diag_anti(const Area &area, 
 const bool direction, const STATE state, const bool force);

bool messUpPlans_neg_diag_anti(const Area &area, 
 const bool direction, const STATE state, const bool force);

bool highQualityAI (bool (*ptrFunc[]) (const Area &area, const bool direction,
 const STATE state, const bool force), const Area &area, const STATE who, const bool force);

#endif // HEADER_HPP
