#include "stdafx.h"

#ifndef STDAFX
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
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

#define U(x) ((x) >> 1)
#define LU(x) ((x) >> 13)
#define L(x) ((x) >> 12)
#define LD(x) ((x) >> 11)
#define D(x) ((x) << 1)
#define RD(x) ((x) << 13)
#define R(x) ((x) << 12)
#define RU(x) ((x) << 11)

/*
The bitboard representation of the tetris field
 */
class Board {
public:
	
	// each represents half of the field i.e. [0] = lower 10 rows + 1 bottom, [1] = upper 10 rows + 1 top
	u128 field[2];

	// working area & sanctified working area
	u128 wAr, swA;

	// shifted working area
	u128 sfA[8] = { 0 };

	// the height of ten columns
	uint8_t hist[11][4] = { {} };

	// lower/upper bound of a half field (0th/11th row), the inverse of row 1 & 11, half of the field without boundaries
	static const u128 LBDRY, UBDRY, nROW1, nROW10, HLF;

	// tetrimino patterns
	static const u128 TMNO[19];

	// directions to shift the field
	enum shift { R, RU, U, LU, L, LD, D, RD };

	// tetrimino types
	enum TMO { L0, L1, L2, L3, J0, J1, J2, J3, S0, S1, Z0, Z1, T0, T1, T2, T3, I3, I4, O0 };

	static inline int getPiece(const u128* board, int x, int y);
	static inline int getPiece(const u128 board, int x, int y);

	// gets the index of the point
	static inline int getIndex(int x, int y);

	// lesbian - returns the least significant bit
	static inline u128 LSB(register u128 num);

	static void printBoard(const u128* board0, const u128* board1);

	// sanctify the working area (fill holes) and put into swA
	inline void sanctify();

	// gets the greater number
	inline uint8_t max(uint8_t a, uint8_t b);
	inline uint8_t max(uint8_t a, uint8_t b, uint8_t c);
	inline uint8_t max(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

	// finds the possible y of a tetrimino when cetered at (x, )
	int cenY(int x, TMO type);

	// applies the type of tetrimino to the specified position
	u128 apply(int x, int y, TMO type);

	// gets the histogram of columns
	void getHist();

	// gets the shifted working area
	u128 getShift(shift ori);

	// gets possible positions of center of a type of tetrimino
	u128 cenPos(TMO);
};

int main() {
	using std::cout;

	Board brd;
	u128 b = brd.UBDRY;
	
	brd.wAr = (Board::TMNO[1] >> 12 | Board::TMNO[10] | Board::TMNO[8] << 41);
	brd.sanctify();
	u128 cen = brd.cenPos(Board::J1);

	brd.getHist();
	time_t time = clock();
	for (int j = 0; j < 1; j++) {
		brd.sanctify();
		for (int i = 1; i < 10; i++) {
			int y = brd.cenY(i, Board::J1);
			if (y < 10) {
				//brd.apply(i, y, Board::J1);
				u128 arr0[2]{ brd.swA, 0 };
				u128 arr1[2]{ brd.apply(i, y, Board::J1), 0 };
				brd.printBoard(arr0, arr1);
			}
		}
	}
	cout << (float)(clock() - time) / CLOCKS_PER_SEC << std::endl;

	u128 arr0[2]{ brd.swA, 0 };
	u128 arr1[2]{ brd.apply(5, 5, Board::J1), 0 };
	/*u128 lsb = brd.LSB(brd.cenPos(Board::J1));
	u128 arr1[2] { brd.cenPos(Board::J1) ^ lsb, 0 };*/
	brd.getHist();
	brd.printBoard(arr0, arr1);
	for (int i = 1; i < 10; i++) {
		cout << "  " << brd.cenY(i, Board::J1);
	}
	 

	/*u128 full = ~(Board::UBDRY | Board::LBDRY);
	cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << full.hi << "ull" << std::endl;
	cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << full.lo << "ull" << std::endl;*/
	std::cin.get();
	return 0;
}

/* magic numbers */

const u128 Board::UBDRY = (u128)0x0001001001001001ull << 60 | 0x0001001001001001ull;
const u128 Board::LBDRY = (u128)0x0800800800800800ull << 60 | 0x0800800800800800ull;
const u128 Board::nROW10 = (u128)0xdffdffdffdffdffdull << 60 | 0xdffdffdffdffdffdull;
const u128 Board::nROW1 = (u128)0xfbffbffbffbffbffull << 60 | 0xfbffbffbffbffbffull;
const u128 Board::HLF = (u128)0x07fe7fe7fe7fe7feull << 60 | 0xe7fe7fe7fe7fe7feull;

// tetrimino patterns
const u128 Board::TMNO[19] = {
	 0x0000008008018000ull, 0x000001001c000000ull, 0x000000c008008000ull, 0x000000001c004000ull, // 0L
	 0x0000018008008000ull, 0x000000401c000000ull, 0x000000800800c000ull, 0x000000001c010000ull, // 1J
	 0x0000008018010000ull, 0x000001800c000000ull, // 2S
	 0x0000010018008000ull, 0x000000c018000000ull, // 3Z
	 0x000000800c008000ull, 0x000000001c008000ull, 0x0000008018008000ull, 0x000000801c000000ull, // 4T
	 0x000000003c000000ull, 0x0008008008008000ull, // 5I
	 0x0000000018018000ull, // 6O
};


/* magic numbers */

/* tells if there is a block at (x, y). 1 <= x <= W, 1 <= y <= H */
inline int Board::getPiece(const u128* board, int x, int y) {
	if (y > 10)
		return (u64)((board[1] >> ((--x) * CH + (21 - y))) & 1);
	else
		return (u64)((board[0] >> ((--x) * CH + 11 - y)) & 1);
}

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
inline uint8_t Board::max(uint8_t a, uint8_t b) {
	return a > b ? a : b;
}

/* gets the greater of the three */
inline uint8_t Board::max(uint8_t a, uint8_t b, uint8_t c) {
	return max(max(a, b), c);
}

/* gets the greater of the four */
inline uint8_t Board::max(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
	return max(max(a, b), max(c, d));
}

/* finds the possible y of a tetrimino when cetered at (x, ) */
int Board::cenY(int x, TMO type) {
	switch (type) {
	case Board::L0:
		break;
	case Board::L1:
		break;
	case Board::L2:
		break;
	case Board::L3:
		break;
	case Board::J0:
		break;
	case Board::J1: return max(hist[x][2], hist[x + 1][1]);
		break;
	case Board::J2:
		break;
	case Board::J3:
		break;
	case Board::S0:
		break;
	case Board::S1:
		break;
	case Board::Z0:
		break;
	case Board::Z1:
		break;
	case Board::T0:
		break;
	case Board::T1:
		break;
	case Board::T2:
		break;
	case Board::T3:
		break;
	case Board::I3:
		break;
	case Board::I4:
		break;
	case Board::O0:
		break;
	default:
		break;
	}
	return 0;
}

/* applies the type of tetrimino to the specified position */
u128 Board::apply(int x, int y, TMO type) {
	int index = getIndex(x, y);
	if (index > 27)
		return TMNO[type] << (index - 27);
	else
		return TMNO[type] >> (27 - index);
}

/* gets the histogram of columns */
void Board::getHist() {
	for (int i = 1; i < 11; i++) {
		for (int j = 1; j < 12; j++)
			if (getPiece(swA, i, j)) {
				hist[i][0] = j;
				hist[i][1] = j - 1;
				hist[i][2] = j + 1;
				hist[i][3] = j + 2;
				break;
			}
	}
}

/* gets the shifted working area */
u128 Board::getShift(shift ori) {
	if (sfA[ori])
		return sfA[ori];
	switch (ori) {
	case Board::R:	sfA[ori] = R(swA);
		break;
	case Board::RU:	sfA[ori] = RU(swA) & nROW1;
		break;
	case Board::U:	sfA[ori] = U(swA) & nROW1;
		break;
	case Board::LU:	sfA[ori] = LU(swA) & nROW1;
		break;
	case Board::L:	sfA[ori] = L(swA);
		break;
	case Board::LD:	sfA[ori] = LD(swA) & nROW10;
		break;
	case Board::D:	sfA[ori] = D(swA) & nROW10;
		break;
	case Board::RD:	sfA[ori] = RD(swA) & nROW10;
		break;
	default:
		break;
	}
	return sfA[ori];
}

/* gets possible positions of center of a type of tetrimino */
u128 Board::cenPos(TMO type) {
	register u128 raw = swA;

	switch (type) {
	case Board::L0:	raw &= getShift(L) & getShift(R) & getShift(RU);
		break;
	case Board::L1:	raw &= getShift(U) & getShift(D) & getShift(LU);
		break;
	case Board::L2:	raw &= getShift(L) & getShift(R) & getShift(LD);
		break;
	case Board::L3:	raw &= getShift(U) & getShift(D) & getShift(RD);
		break;
	case Board::J0:	raw &= getShift(L) & getShift(R) & getShift(LU);
		break;
	case Board::J1:	raw &= getShift(U) & getShift(D) & getShift(LD);
		break;
	case Board::J2:	raw &= getShift(L) & getShift(R) & getShift(RD);
		break;
	case Board::J3:	raw &= getShift(U) & getShift(D) & getShift(RU);
		break;
	case Board::S0:	raw &= getShift(U) & getShift(L) & getShift(RU);
		break;
	case Board::S1:	raw &= getShift(D) & getShift(L) & getShift(LU);
		break;
	case Board::Z0:	raw &= getShift(U) & getShift(R) & getShift(LU);
		break;
	case Board::Z1:	raw &= getShift(U) & getShift(L) & getShift(LD);
		break;
	case Board::T0:	raw &= getShift(L) & getShift(R) & getShift(D);
		break;
	case Board::T1:	raw &= getShift(U) & getShift(D) & getShift(R);
		break;
	case Board::T2:	raw &= getShift(L) & getShift(R) & getShift(U);
		break;
	case Board::T3:	raw &= getShift(U) & getShift(D) & getShift(L);
		break;
	case Board::I3:	raw &= getShift(U) & getShift(D) & U(getShift(U)) & nROW1;
		break;
	case Board::I4:	raw &= getShift(L) & getShift(R) & L(getShift(L));
		break;
	case Board::O0:	raw &= getShift(D) & getShift(R) & getShift(RD);
		break;
	default:
		break;
	}

	// moves raw upward and & with ~itself to get the lower edges of the possible position area
	return raw & ~U(raw);
}
