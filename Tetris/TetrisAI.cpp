#include "stdafx.h"

#ifndef STDAFX
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cfloat>
#include <stdio.h>
#endif // STDAFX

#ifdef _MSC_VER
typedef uint128_t u128;
typedef unsigned __int64 u64;
#endif
#ifdef __GNUG__
typedef unsigned __int128 u128;
typedef uint64_t u64;
#endif

#define H 20
#define W 10
#define CH 12

#define DEPTH 3

#define U(x) ((x) >> 1)
#define LU(x) ((x) >> 13)
#define L(x) ((x) >> 12)
#define LD(x) ((x) >> 11)
#define D(x) ((x) << 1)
#define RD(x) ((x) << 13)
#define R(x) ((x) << 12)
#define RU(x) ((x) << 11)
class Board;
void print(u128 u);
void printHist(Board);
/*
The bitboard representation of the tetris field
*/
class Board {
public:

	// each represents half of the field i.e. [0] = lower 10 rows + 1 bottom, [1] = upper 10 rows + 1 top
	u128 field[2];

	// working area & sanctified working area
	u128 wAr, swA;  //wAr 10列10行 一头一尾

	// shifted working area
	//u128 sfA[8] = { 0 };

	// the height of ten columns
	uint8_t hist[11] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };//储存的是每一列最高高度y+1即最低空方格位置hist  注意 这是没有把洞洞当成一格自动下拉填满

	/*
	lower/upper bound of a half field (0th/11th row), the inverse of row 1 & 11
	half of the field without boundaries, masks for selecting rows for elimination check
	*/
	static const u128 LBDRY, UBDRY, nROW1, nROW10, HLF, ROW[11], ELMMSK[10], nELMMSK[10];

	// the rage of x of each type of tetrimino, index of each piece
	static const uint8_t XRANGE[19][2], PIECE[11][11];

	//static const uint32_t _1110 = 0x1110, _1100 = 0x1100, _0110 = 0x0110, _0100 = 0x;

	// tetrimino patterns
	static const u128 TMNO[19];

	// directions to shift the field
	enum shift { R, RU, U, LU, L, LD, D, RD };

	// tetrimino types
	enum TMO { L0, L1, L2, L3, J0, J1, J2, J3, S0, S1, Z0, Z1, T0, T1, T2, T3, I3, I4, O0 };

	static inline int getPiece(const u128* board, int x, int y);//能够get到一个board上x，y这个位置有没有填满
	static inline int getPiece(const u128 board, int x, int y);

	// gets the index of the point
	static inline int getIndex(int x, int y);//getIndex得到的是x，y这个块的bit index

	// lesbian - returns the least significant bit
	static inline u128 LSB(register u128 num);//get的是最低的1bit

	// count the number of 1's in board
	static inline int get1Count(u64 board);

	static void printBoard(const u128* board0, const u128* board1);

	// sanctify the working area (fill holes) and put into swA
	inline void sanctify();//得到一个把洞洞填满的board 相当于每列填满的方块数量是对应列hist的值 sanctify以后的结果会存在swA里面

	// gets the greater number
	inline uint8_t max(int8_t a, int8_t b);
	inline uint8_t max(int8_t a, int8_t b, int8_t c);
	inline uint8_t max(int8_t a, int8_t b, int8_t c, int8_t d);

	// finds the possible y of a tetrimino when cetered at (x, )
	int cenY(int x, TMO type);//cenY得到的是这一种tetrimino在x上中心的最低位置 判断能不能把方块放下去

	// tries to eliminate y'th row of board, returns the row mask if sccess, 0 if fail
	static inline const void elim(u128& ori, u128& applied, u128& elim, int y, int& count);//applied是一个（可能）填满了一行的board  ori是在填入最后一个方块之前的board  elim会在applied搜索y这一行，看能不能消去，如果能，会从ori里面提取这一行，放到elim里面 然后把count+1 这样就能得到去除被消去方块的消去行
	
	// updates histogram
	inline void updateHist(uint8_t* hist, u64 half);//udpateHist会用四分之一个board（half）更新hist 

	// applies the type of tetrimino to the specified position
	void apply(u128& board, int x, int y, TMO type, u128& elim, int& count);//在board上的x，y（方块中心）这个位置上放置type这种tetrimino 并把消去的结果存到elim里面，count+=消去行数 

	int prevX = 0;
	TMO lastType = static_cast<enum Board::TMO>(-1);
	int inline hasNext(TMO type);//hasNext会检查type能不能放下  具体的x位置是prevX的值 
	void inline nextMove(Board& newBoard, TMO type);//nextMove会以当前的wAr为基础在newBoard上放置type（不更改wAr） 然后prevX++就相当于循环枚举当前wAr上type可以放的位置，直到nextMove里面call到hasNext发现没有next为止

	// make a move of type and store to newBoard, returns the y of next move, 0 if no more
	int inline move(Board& newBoard, TMO type);

	float eval();

	////// gets the histogram of columns
	void getHist();
};

class Simulator {
public:

	Board board;

	uint8_t moveCounter = 0;

	uint8_t typeCount[7] = { 4, 4, 2, 2, 4, 2, 1 };

	// giver's tetrimino statistics
	uint8_t tStat[7] = { 2, 2, 2, 2, 2, 2, 2 };
	Board::TMO types[7] = { Board::L0, Board::J0, Board::S0, Board::Z0, Board::T0, Board::I3, Board::O0 };

	Board::TMO feed();
	void updateFeed(int type);

	int hasNextDrop(Board & board, int type);

	inline void dropBlock(Board&, int type);

	// next tetrimino to place
	Board::TMO nextBlock;
};

/* magic numbers */

const u128 Board::UBDRY = (u128)0x0001001001001001ull << 60 | 0x0001001001001001ull;
const u128 Board::LBDRY = (u128)0x0800800800800800ull << 60 | 0x0800800800800800ull;
const u128 Board::nROW10 = (u128)0xdffdffdffdffdffdull << 60 | 0xdffdffdffdffdffdull;
const u128 Board::nROW1 = (u128)0xfbffbffbffbffbffull << 60 | 0xfbffbffbffbffbffull;
const u128 Board::HLF = (u128)0x07fe7fe7fe7fe7feull << 60 | 0xe7fe7fe7fe7fe7feull;
const u128 Board::ROW[11] = { 0, (u128)0x0400400400400400ull << 60 | 0x0400400400400400ull,
(u128)0x0200200200200200ull << 60 | 0x0200200200200200ull, (u128)0x0100100100100100ull << 60 | 0x0100100100100100ull,
(u128)0x0080080080080080ull << 60 | 0x0080080080080080ull, (u128)0x0040040040040040ull << 60 | 0x0040040040040040ull,
(u128)0x0020020020020020ull << 60 | 0x0020020020020020ull, (u128)0x0010010010010010ull << 60 | 0x0010010010010010ull,
(u128)0x0008008008008008ull << 60 | 0x8008008008008008ull, (u128)0x0004004004004004ull << 60 | 0x4004004004004004ull,
(u128)0x0002002002002002ull << 60 | 0x2002002002002002ull,
};
const uint8_t Board::PIECE[11][11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13,
35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25,
47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37,
59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49,
71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61,
83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73,
95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85,
107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97,
119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, };
const u128 Board::ELMMSK[10] = {
	HLF, U(ELMMSK[0]) & HLF, U(ELMMSK[1]) & HLF, U(ELMMSK[2]) & HLF, U(ELMMSK[3]) & HLF, U(ELMMSK[4]) & HLF,
	U(ELMMSK[5]) & HLF, U(ELMMSK[6]) & HLF, U(ELMMSK[7]) & HLF, U(ELMMSK[8]) & HLF
};
const u128 Board::nELMMSK[10] = {
	~ELMMSK[0] & HLF, ~ELMMSK[0] & HLF, ~ELMMSK[1] & HLF, ~ELMMSK[2] & HLF, ~ELMMSK[3] & HLF, ~ELMMSK[4] & HLF,
	~ELMMSK[5] & HLF, ~ELMMSK[6] & HLF, ~ELMMSK[7] & HLF, ~ELMMSK[8] & HLF
};
const uint8_t Board::XRANGE[19][2] = {
	{ 2, 9 },{ 1, 9 },{ 2, 9 },{ 2, 10 },
	{ 2, 9 },{ 1, 9 },{ 2, 9 },{ 2, 10 },
	{ 2, 9 },{ 1, 9 },
	{ 2, 9 },{ 1, 9 },
	{ 2, 9 },{ 2, 10 },{ 2, 9 },{ 1, 9 },
	{ 1, 10 },{ 2, 8 },
	{ 2, 10 },
};

// tetrimino patterns
const u128 Board::TMNO[19] = {
	0x0000008008018000ull, 0x000001001c000000ull, 0x000000c008008000ull, 0x000000001c004000ull, // 0L
	0x0000018008008000ull, 0x000000401c000000ull, 0x000000800800c000ull, 0x0000000038020000ull, // 1J
	0x0000008018010000ull, 0x000001800c000000ull, // 2S
	0x0000010018008000ull, 0x000000c018000000ull, // 3Z
	0x000000800c008000ull, 0x000000001c008000ull, 0x0000008018008000ull, 0x000000801c000000ull, // 4T
	0x000000003c000000ull, 0x0008008008008000ull, // 5I
	0x000000000c00c000ull, // 6O
};


/* magic numbers */

/* tells if there is a block at (x, y). 1 <= x <= W, 1 <= y <= H */
inline int Board::getPiece(const u128* board, int x, int y) {
	if (y > 10)
		return (u64)((board[1] >> ((--x) * CH + (21 - y))) & 1);
	else
		return (u64)((board[0] >> ((--x) * CH + 11 - y)) & 1);
}//首先计算bit的index（(--x) * CH + (21 - y)）然后把整个board移动这么多格（相当于这个bit被移动到0的位置了）

inline int Board::getPiece(const u128 board, int x, int y) {
	return (u64)((board >> ((--x) * CH + 11 - y)) & 1);
}

/* gets the index of the point */
inline int Board::getIndex(int x, int y) {
	return (--x) * CH + 11 - y;
}

inline u128 Board::LSB(register u128 num) {
	//return num & (num - (u128)1);
	return num & -num;
}

/* gets the count of set bits */
inline int Board::get1Count(u64 board) {
#ifdef _MSC_VER
	board -= (board >> 1) & 0x5555555555555555;
	board = (board & 0x3333333333333333) + ((board >> 2) & 0x3333333333333333);
	board = (board + (board >> 4)) & 0x0f0f0f0f0f0f0f0f;
	board = (board * 0x0101010101010101) >> 56;
	return (int)board;
#endif
#ifdef __GNUG__
	return __builtin_popcountll(board);
#endif
}

void Board::printBoard(const u128* board0, const u128* board1) {
	using std::cout;
	using std::endl;
	using std::setw;
	using std::string;

	string str;
	cout << "  -------------------------------" << endl;
	for (int y = 10; y >= 1; y--) {
		cout << setw(2) << y << "|";
		for (int x = 1; x <= W; x++) {
			// if the second board is unavailable, prints %% for occupied for the first board
			// else prints @+ if the bit is occupied by both, @@ if only board0, ++ if only board1
			if (getPiece(board0, x, y))
				if (board1 == NULL)
					str = "%%";
				else
					if (getPiece(board1, x, y))
						str = "@+";
					else
						str = "@@";
			else
				if (board1 == NULL)
					str = "  ";
				else
					if (getPiece(board1, x, y))
						str = "+@";
					else
						str = "  ";
			cout << str << "|";
		}
		cout << endl << "  -------------------------------" << endl;
	}
	cout << "   1  2  3  4  5  6  7  8  9  10" << endl;
}

/* fills the holes in the field */
inline void Board::sanctify() {
	// wAr | LBDRY to make a bottom for filling 1 (in case there is a empty column)
	// - UBDRY propagates trailing 1 to the top of the column
	swA = (wAr | LBDRY) - UBDRY;
	// ~wAr clears the original part, HLF clears the residue of UBDRY
	swA &= (~wAr & HLF);
}

/* gets the greater of the two */
inline uint8_t Board::max(int8_t a, int8_t b) {
	return a > b ? a : b;
}

/* gets the greater of the three */
inline uint8_t Board::max(int8_t a, int8_t b, int8_t c) {
	return max(max(a, b), c);
}

/* gets the greater of the four */
inline uint8_t Board::max(int8_t a, int8_t b, int8_t c, int8_t d) {
	return max(max(a, b), max(c, d));
}

/* finds the possible y of a tetrimino when cetered at (x, ) */
int Board::cenY(int x, TMO type) {
	switch (type) {
	case Board::L0: return max(hist[x - 1] + 1, hist[x], hist[x + 1]);
		break;
	case Board::L1: return max(hist[x] + 1, hist[x + 1] + 1);
		break;
	case Board::L2:
	case Board::J2:
	case Board::T0:return max(hist[x - 1], hist[x], hist[x + 1]);
		break;
	case Board::L3: return max(hist[x - 1] - 1, hist[x] + 1);
		break;
	case Board::J0: return max(hist[x - 1], hist[x], hist[x + 1] + 1);
		break;
	case Board::J1: return max(hist[x] + 1, hist[x + 1] - 1);
		break;
	case Board::J3: return max(hist[x - 1] + 1, hist[x] + 1) + 1;
		break;
	case Board::S0: return max(hist[x - 1] + 1, hist[x] + 1, hist[x + 1]);
		break;
	case Board::S1: return max(hist[x], hist[x + 1] + 1);
		break;
	case Board::Z0: return max(hist[x - 1], hist[x] + 1, hist[x + 1] + 1);
		break;
	case Board::T3:
	case Board::Z1: return max(hist[x] + 1, hist[x + 1]);
		break;
	case Board::T1: return max(hist[x - 1], hist[x] + 1);
		break;
	case Board::T2: return max(hist[x - 1], hist[x] + 1, hist[x + 1]);
		break;
	case Board::I3: return hist[x] + 2;
		break;
	case Board::I4: return max(hist[x - 1], hist[x], hist[x + 1], hist[x + 2]);
		break;
	case Board::O0: return max(hist[x - 1], hist[x]);
		break;
	default:
		break;
	}
	return 0;
}

/* tries to eliminate y'th row of board, returns the row mask if sccess, 0 if fail */
inline const void Board::elim(u128& ori, u128& applied, u128& elim, int y, int& count) {
	const u128* mask = ROW + y;

	// if row full
	if ((applied & *mask) == *mask) {
		count++;
		elim |= U(elim) | (ori & *mask) << (y - 1);
		applied = (applied & nELMMSK[y]) | D(applied & ELMMSK[y]);
	}
}

//const u64 colMask[5] = { 0x00000000000007feull, 0x000000000007fe000ull, 0x000000007fe000000ull, 0x000007fe000000000ull, 0x007fe0000000000ull };

inline void Board::updateHist(uint8_t* hist, u64 half) {
	// flip
	half = ~half & 0x07fe7fe7fe7fe7feull;
	half = ((half & 0x0554554554554554ull) >> 1) + (half & 0x02aa2aa2aa2aa2aaull);
	half = ((half & 0x0198198198198198ull) >> 2) + (half & 0x0666666666666666ull);
	half = ((half & 0x0600600600600600ull) >> 8) + ((half & 0x01e01e01e01e01e0ull) >> 4) + (half & 0x001e01e01e01e01eull);
	half = (half & 0x00000000000007feull) >> 1 | (half & 0x00000000007fe000ull) >> 5 | (half & 0x00000007fe000000ull) >> 9 |
		(half & 0x00007fe000000000ull) >> 13 | (half & 0x07fe000000000000ull) >> 17;
	half += 0x0000010101010101;
	*((u64*)(void*)hist) = (half & 0x000000ffffffffff) | (*((u64*)(void*)hist) & 0xffffff0000000000);
}

/* applies the type of tetrimino to the specified position, elm contains the eliminated rows, count is the
number of rows eliminated*/
void Board::apply(u128& board, int x, int y, TMO type, u128& elm, int& elmCnt) { // TODO load from field if depleted
	u128 tmp = board;
	int count = 0;

	// update board
	int index = PIECE[x][y];
	if (index > 27)
		board |= TMNO[type] << (index - 27);
	else
		board |= TMNO[type] >> (27 - index);

	// check for elimination
	switch (type) {
		// this & below
	case Board::L0:
	case Board::J0:
	case Board::S1:
	case Board::Z1:
	case Board::T1:
	case Board::T2:
	case Board::T3:
		elim(tmp, board, elm, y, count);
		elim(tmp, board, elm, y - 1, count);
		/*if (x == 4 && y == 2)
		print(elm);*/
		break;
		// below
	case Board::L1:
	case Board::J3:
	case Board::S0:
	case Board::Z0:
		elim(tmp, board, elm, y - 1, count);
		break;
		// this
	case Board::L2:
	case Board::J2:
	case Board::T0:
	case Board::I4:
		elim(tmp, board, elm, y, count);
		break;
		// this & above & below
	case Board::L3:
	case Board::J1:
		elim(tmp, board, elm, y + 1, count);
		elim(tmp, board, elm, y, count);
		elim(tmp, board, elm, y - 1, count);
		break;
		// this & above & below & bblow
	case Board::I3:
		elim(tmp, board, elm, y + 1, count);
		elim(tmp, board, elm, y, count);
		elim(tmp, board, elm, y - 1, count);
		elim(tmp, board, elm, y - 2, count);
		break;
		// this & above
	case Board::O0:
		elim(tmp, board, elm, y + 1, count);
		elim(tmp, board, elm, y, count);
		break;
	default:
		break;
	}

	// update hist
	y++;
	switch (type) {
	case Board::L0:
	case Board::J0:
	case Board::T2: hist[x - 1] = hist[x] = hist[x + 1] = y;
		break;
	case Board::L1: hist[x] = y + 1; hist[x + 1] = y - 1;
		break;
	case Board::L2: hist[x - 1] = hist[x] = y; hist[x + 1] = y + 1;
		break;
	case Board::L3:
	case Board::O0: hist[x - 1] = hist[x] = y + 1;
		break;
	case Board::J1:	hist[x] = hist[x + 1] = y + 1;
		break;
	case Board::J2: hist[x - 1] = y + 1; hist[x] = hist[x + 1] = y;
		break;
	case Board::J3: hist[x - 1] = y - 1; hist[x] = y + 1;
		break;
	case Board::S0: hist[x - 1] = y - 1; hist[x] = hist[x + 1] = y;
		break;
	case Board::S1:
	case Board::T3: hist[x] = y + 1; hist[x + 1] = y;
		break;
	case Board::Z0: hist[x - 1] = hist[x] = y; hist[x + 1] = y - 1;
		break;
	case Board::Z1: hist[x] = y; hist[x + 1] = y + 1;
		break;
	case Board::T0: hist[x - 1] = hist[x + 1] = y; hist[x] = y + 1;
		break;
	case Board::T1: hist[x - 1] = y; hist[x] = y + 1;
		break;
	case Board::I3: hist[x] = y + 1;
		break;
	case Board::I4: hist[x - 1] = hist[x] = hist[x + 1] = hist[x + 2] = y;
		break;
	default:
		break;
	}

	// update hist after elimination
	if (count) {
		sanctify();
		//print(swA);
		updateHist(hist + 1, (u64)(swA & 0x0FFFFFFFFFFFFFFFF));
		updateHist(hist + 6, (u64)((swA >> 60) & 0x0FFFFFFFFFFFFFFFF));
		elmCnt += count;
		/*return;
		u64* h = (u64*)(void*)hist;
		*h -= 0x0000010101010100 * count;
		h = (u64*)(void*)&hist[6];
		*h -= 0x0000000101010101 * count;*/
	}
}

/* determines if there is a next move for this type, returns y of next move */
int inline Board::hasNext(TMO type) {
	// a new iteration, update x to start searching
	if (lastType != type) {
		lastType = type;
		prevX = XRANGE[type][0];
	}

	int y;
	while (prevX <= XRANGE[type][1]) {
		if ((y = cenY(prevX, type)) < 11)
			return y;
		prevX++;
	}
	return 0;
}

/* makes next move of type to newBoard */
inline void Board::nextMove(Board & newBoard, TMO type) {
	int y = hasNext(type);
	if (y) {
		newBoard = *this;
		int count;
		u128 elim;
		newBoard.apply(newBoard.wAr, prevX++, y, type, elim, count);//TODO null, count
		count = 0;
	}
}

/* OBSOLETE */
/* make a move of type and store to newBoard, returns the y of next move, 0 if no more */
int inline Board::move(Board& newBoard, TMO type) {
	static int prevX = 0;// TODO change to instance variable
	static int prevY = 0;

	// first time, don't copy, set x to new type's starting index
	if (prevY != 0) {
		newBoard = *this;
		int count;
		u128 elim;
		newBoard.apply(newBoard.wAr, prevX, prevY, type, elim, count);//TODO null, count
		count = 0;
		prevX++;
		//print(newBoard.wAr);
	}
	else
		prevX = XRANGE[type][0];

	while (prevX <= XRANGE[type][1]) {
		if ((prevY = cenY(prevX, type)) < 11)
			return prevY;
	}

	return prevY = 0;
}

float Board::eval() {//TODO
	////////////////////////tmp
	// find max height
	int maxHeight = 0;
	for (int i = 1; i < 11; i++)
		if (hist[i] > maxHeight)
			maxHeight = hist[i];
	if (maxHeight > 10)
		return -FLT_MAX;
	////////////////////////tmp

	sanctify();
	
	// counting holes
	u128 diff = (~wAr ^ swA) & HLF;
	u64* p = (u64*)&diff;
	int holeCount = get1Count(p[0]) + get1Count(p[1]);

	print(wAr);
	std::cout << "eval = " << -(holeCount + maxHeight) << std::endl;

	return -(holeCount + maxHeight);
}

Board::TMO Simulator::feed() {
	while (moveCounter < 7) {
		if (tStat[moveCounter])
			return types[moveCounter++];
		moveCounter++;
	}

	return static_cast<enum Board::TMO>(-1);
}

void Simulator::updateFeed(int type) {
	// all used up, fill up
	u64 something = *((u64*)(void*)tStat) & 0x00ffffffffffffff;
	if (something == 0 || something == 0x0001010101010101)
		*((u64*)(void*)tStat) = 0x0002020202020202;
	tStat[type]--;
}

inline int Simulator::hasNextDrop(Board& board, int type) {
	while (moveCounter < typeCount[type]) {
		int y = board.hasNext(static_cast<enum Board::TMO>(type + moveCounter));
		if (y)
			return y;
		moveCounter++;
	}

	return 0;
}

inline void Simulator::dropBlock(Board& board, int type) {
	if (hasNextDrop(this->board, type))
		this->board.nextMove(board, static_cast<enum Board::TMO>(type + moveCounter));
	else
		moveCounter++;
}

float negaMax(Simulator& sim, int depth, float alpha, float beta, int player) {
	if (depth == 0)
		return sim.board.eval();

	Simulator newSim;
	float best = -FLT_MAX;

	// feeder moves
	if (player == 1) {
		while ((newSim.nextBlock = sim.feed()) != -1) {
			newSim = sim;
			newSim.moveCounter = 0;
			newSim.updateFeed(sim.moveCounter - 1);

			float score = -negaMax(newSim, depth - 1, -beta, -alpha, -player);
			best = (score > best ? score : best);
			alpha = (alpha > best ? alpha : best);
			if (alpha > beta)
				break;
		}
	}
	// droper moves
	else {
		// move ordering by center height
		int y = -1;
		// at most 10 moves have the same height
		Board results[10][40];
		// 10 different height
		uint8_t stat[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		// for each possible drop
		while (y = sim.hasNextDrop(sim.board, sim.nextBlock)) {// BUG: block feeding does not update
			// go to the row corresponding to that height
			sim.dropBlock(results[y][stat[y]++], sim.nextBlock);
		}

		// if no next move, die
		if (y == -1)
			return -FLT_MAX;

		// get each move
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < stat[i]; j++) {
				newSim = sim;
				newSim.moveCounter = 0;
				newSim.board = results[i][j];
				float score = -negaMax(newSim, depth - 1, -beta, -alpha, -player);
				best = (score > best ? score : best);
				alpha = (alpha > best ? alpha : best);
				if (alpha > beta)
					break;
			}
		}
	}
	return best;
}

///* gets the histogram of columns */
void Board::getHist() {
	for (int i = 1; i < 11; i++) {
		for (int j = 1; j < 12; j++)
			if (getPiece(swA, i, j)) {
				hist[i] = j;
				break;
			}
	}
}


void print(u128 u) {
	u128 arr[2] = { u, 0 };
	Board::printBoard(arr, 0);
}

void printHist(Board b) {
	for (int i = 1; i < 11; i++)
		std::cout << "  " << (int)b.hist[i];
	std::cout << std::endl;
}

/* OBSOLETE */
void printMove(Board b, Board::TMO type) {
	Board m;
	m.wAr = 0;
	while (b.move(m, type)) {
		print(m.wAr);
	}
	print(m.wAr);
}

void printMoveN(Board b, Board::TMO type) {
	Board m;
	m.wAr = 0;
	while (b.hasNext(type)) {
		b.nextMove(m, type);
		print(m.wAr);
		std::cout << "eval = " << m.eval() << std::endl;
	}
}

int table[4][7] = {
	0, 4,  8, 10, 12, -1, 18,
	1, 5,  9, 11, 13, -1, -1,
	2, 6, -1, -1, 14, 16, -1,
	3, 7, -1, -1, 15, 17, -1,
};

void simplePlay() {
	using std::cin;
	using std::cout;

	Board brd;
	brd.wAr = 0;
	print(brd.wAr);

	u128 elim = 0;
	int count = 0;

	brd.apply(brd.wAr, 2, 1, static_cast<enum Board::TMO>(6), elim, count);
	//printMove(brd, Board::J1);
	brd.apply(brd.wAr, 6, 1, static_cast<enum Board::TMO>(18), elim, count);
	brd.apply(brd.wAr, 8, 1, static_cast<enum Board::TMO>(12), elim, count);
	brd.apply(brd.wAr, 2, 3, static_cast<enum Board::TMO>(3), elim, count);
	brd.apply(brd.wAr, 7, 4, static_cast<enum Board::TMO>(16), elim, count);
	//printMove(brd, Board::J2);
	brd.apply(brd.wAr, 10, 2, static_cast<enum Board::TMO>(13), elim, count);
	print(brd.wAr);
	brd.apply(brd.wAr, 4, 2, static_cast<enum Board::TMO>(13), elim, count);

	//print(brd.wAr);
	/*if (elim != (u128)0) {
	print(brd.wAr);
	printHist(brd);
	print(elim);
	cout << "eliminated: " << count << std::endl;
	}*/

	Board nb;
	time_t time = clock();
	for (int j = 0; j < 0; j++)
		while (brd.move(nb, Board::J1));
	cout << (float)(clock() - time) / CLOCKS_PER_SEC << std::endl;
	cout << "size" << sizeof(nb) << std::endl;

	brd.wAr = 0;
	brd.swA = 0;
	memset(brd.hist, 1, 11);
	elim = 0;
	count = 0;

	while (true) {
		print(brd.wAr);
		cout << "eval = " << brd.eval() << std::endl;
		int x, y;
		char o, b;
		cin >> x >> y >> o >> b;

		if (o == 'C') {
			brd.wAr = 0;
			memset(brd.hist, 1, 11);
		}
		else {
			switch (b) {
			case 'l':
			case 'L': b = 0;
				break;
			case 'j':
			case 'J': b = 1;
				break;
			case 's':
			case 'S': b = 2;
				break;
			case 'z':
			case 'Z': b = 3;
				break;
			case 't':
			case 'T': b = 4;
				break;
			case 'i':
			case 'I': b = 5;
				break;
			case 'o':
			case 'O': b = 6;
				break;
			default: b = -1;
				break;
			}

			o -= 48;

			if ((o < 0 || o > 3) && (o + 48)) {
				cout << "o out of range" << std::endl;
				continue;
			}
			if (b < 0 || o > 6) {
				cout << "b out of range" << std::endl;
				continue;
			}
			int t = table[o][b];
			if (t < 0) {
				cout << "no such type" << std::endl;
				continue;
			}

			Board::TMO type = static_cast<enum Board::TMO>(t);

			if (x == y && y == -1) {
				printMoveN(brd, type);
				continue;
			}

			if (brd.cenY(x, type) != y) {
				cout << "invalid y" << std::endl;
				continue;
			}

			if (x < Board::XRANGE[type][0] || x > Board::XRANGE[type][1]) {
				cout << "invalid x" << std::endl;
				continue;
			}


			brd.apply(brd.wAr, x, y, type, elim, count);
		}

		if (elim != (u128)0) {
			print(elim);
			cout << "eliminated: " << count << std::endl;
			elim = 0;
			count = 0;
		}
		//brd.wAr = 0;
	}
}

int main() {
	using std::cout;
	using std::endl;
	Board brd;
	Simulator sml;
	sml.nextBlock = Board::J0;
	negaMax(sml, 3, -FLT_MAX, FLT_MAX, -1);
	//sml.updateFeed(0);
	//sml.updateFeed(1);
	//sml.updateFeed(2);
	//sml.updateFeed(3);
	//sml.updateFeed(4);
	//sml.updateFeed(5);
	//sml.updateFeed(6);
	//sml.updateFeed(0);
	//cout << "feed = " << sml.feed() << endl;
	//cout << "feed = " << sml.feed() << endl;
	/*for (int i = 0; i < 7; i++) {
	cout << (int)sml.tStat[i];
	}*/
	/*for (int i = 0; i < 11; i++) {
	for (int j = 0; j < 11; j++) {
	cout << (i - 1) * CH + 11 - j << ", ";
	}
	cout << std::endl;
	}*/
	//simplePlay();

	////Board brd;
	//u128 k;
	//int l;

	//brd.wAr = (Board::TMNO[1] >> 12 | Board::TMNO[10] | Board::TMNO[8] << 41);
	//brd.sanctify();
	//brd.getHist();
	//print(brd.wAr);
	time_t time = clock();

	//for (int i = 1; i < 10; i++) {
	//	brd.cenY(i, Board::J1);
	//	brd.apply(brd.wAr, 5, 5, Board::J1, k, l);
	//	//std::cout << brd.cenY(i, Board::J1) << "  ";
	//}
	cout << (float)(clock() - time) / CLOCKS_PER_SEC << std::endl;


	//cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << full.hi << "ull" << std::endl;
	//cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << full.lo << "ull" << std::endl;
	std::cin.get();
	return 0;
}