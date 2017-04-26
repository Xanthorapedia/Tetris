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

#define U(x) ((x) << 1)
#define D(x) ((x) >> 1)
#define L(x) ((x) >> 12)
#define R(x) ((x) << 12)

/*
The bitboard representation of the tetris field
 */
class Board {
public:
	
	// lower/upper bound of a half field (0th/11th row)
	static const u128 LBDRY, UBDRY;

	// each represents half of the field i.e. [0] = lower 10 rows + 1 bottom, [1] = upper 10 rows + 1 top
	u128 field[2];

	// tetrimino patterns
	static const u128 TMNO[7][4];
	//static const enum TMO {L0, L1, L2, L3, J0, J1, J2, J3, S0, S1, S2, S3, 
	//	Z0, Z1, Z2, Z3, T0, T1, T2, T3, L0, L1, L2, L3, O0};

	static inline int getPiece(const u128* board, int x, int y);

	static void printBoard(const u128* board, const char table0[H][W], const char table1[H][W]);
};

int main() {
	Board brd;

	using std::cout;
	u64 a = 1 << 27;
	a |= U((a)) | L((a)) | L(U(a));
	u128 b = brd.UBDRY;

	u128 arr[2]{ Board::TMNO[6][0], 0};
	//u128 arr[2]{ a, 0 };
	for (int i = 0; i < 28; i++) {
		if (Board::TMNO[0][i] != (u128)0)
		brd.printBoard(Board::TMNO[0] + i, NULL, NULL);
		//system("cls");
	}
	cout << "0x" << std::setfill('0') << std::setw(16) << std::hex << a << "ull";
	std::cin.get();
	return 0;
}

/* magic numbers */

const u128 Board::LBDRY = (u128)0x0001001001001001ull << 60 | 0x0001001001001001ull;
const u128 Board::UBDRY = (u128)0x0800800800800800ull << 60 | 0x0800800800800800ull;

const u128 Board::TMNO[7][4] = {
	{ 0x000000800800c000ull, 0x000000401c000000ull, 0x0000018008008000ull, 0x000000001c010000ull }, // 0L
	{ 0x000000c008008000ull, 0x000001001c000000ull, 0x0000008008018000ull, 0x000000001c004000ull }, // 1J
	{ 0x000000800c004000ull, 0x000000c018000000ull }, // 2S
	{ 0x000000400c008000ull, 0x000001800c000000ull }, // 3Z
	{ 0x0000008018008000ull, 0x000000001c008000ull, 0x000000800c008000ull, 0x000000801c000000ull }, // 4T
	{ 0x000000003c000000ull, 0, 0, 0x0008008008008000ull }, // 5I
	{ 0x0000000018018000ull } // 6O
};

/* magic numbers */

/* tells if there is a block at (x, y). 1 <= x <= W, 1 <= y <= H*/
inline int Board::getPiece(const u128* board, int x, int y) {
	if (y > 10)
		return (u64)((board[1] >> ((--x) * CH + (y - 10))) & 1);
	else
		return (u64)((board[0] >> ((--x) * CH + y)) & 1);
}

void Board::printBoard(const u128* board, const char table0[H][W], const char table1[H][W]) {
	using std::cout;
	using std::endl;
	using std::setw;
	using std::string;

	string str;
	cout << "  ----------------------------------" << endl;
	for (int y = 10; y >= 1; y--) {
		cout << setw(2) << y << "|";
		for (int x = 1; x <= W; x++) {
			str = "";
			// if has block, print char from table1 if applicable, else from table0
			if (getPiece(board, x, y))
				if (table1 != NULL && table1[x] != NULL)
					str = str + table1[x][y] + table1[x][y];
				else
					str = "##";
			else
				if (table0 != NULL && table0[x] != NULL)
					str = str + table0[x][y] + table0[x][y];
				else
					str = "  ";
			cout << str << "|";
		}
		cout << endl << "  ----------------------------------" << endl;
	}
	cout << "   1  2  3  4  5  6  7  8  9  10" << endl;
}
