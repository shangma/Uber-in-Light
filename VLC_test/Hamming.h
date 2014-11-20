#pragma once
/***************************************************************************
*               Hamming Encoding and Decoding Implementation
*
*   File    : hamming.c
*   Purpose : Implementation Hamming encode and decode routines.  Contains
*             different style functions for Hamming encoding and decoding.
*   Author  : Michael Dipperstein
*   Date    : December 29, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: hamming.c,v 1.4 2007/09/19 13:08:14 michael Exp $
*   $Log: hamming.c,v $
*   Revision 1.4  2007/09/19 13:08:14  michael
*   Licensed under LGPL V3.
*
*   Revision 1.3  2005/10/24 12:33:12  michael
*   corrected errors in decode matrices pointed out by Ivan Piasini
*   <furettoo@gmail.com>
*
*   Revision 1.2  2005/01/03 14:41:56  michael
*   Renamed code generator matrix G so that it matches documentation.
*
*   Revision 1.1.1.1  2005/01/02 05:06:45  michael
*   Initial version
*
*
****************************************************************************
*
* Hamming: A collection of ANSI C Hamming Encoding/Decoding routines
* Copyright (C) 2004, 2007 by Michael Dipperstein (mdipper@cs.ucsb.edu)
*
* This file is part of the Hamming library.
*
* The Hamming library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The Hamming library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <limits.h>

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
/* number of uncoded data bits and data values */
#define DATA_BITS       4
#define DATA_VALUES     (1 << DATA_BITS)

/* number of parity bits and data values */
#define PARITY_BITS     3
#define PARITY_VALUES   (1 << PARITY_BITS)

/* number of code bits (data + parity) and data values */
#define CODE_BITS       (DATA_BITS + PARITY_BITS)
#define CODE_VALUES     (1 << CODE_BITS)

#if (CODE_BITS > CHAR_BIT)
#error Encoded data must fit in an unsigned char
#endif


/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/* code matrix 2nd, 3rd, and 4th MSB define parity bits */
const unsigned char gg[DATA_BITS] =
{
	0x38,   /* 0 1 1 | 1 0 0 0 */
	0x54,   /* 1 0 1 | 0 1 0 0 */
	0x62,   /* 1 1 0 | 0 0 1 0 */
	0x71    /* 1 1 1 | 0 0 0 1 */
};

/* code matrix trasposed for ease of use in routines */
const unsigned char gT[CODE_BITS] =
{
	0x07,   /* 0 1 1 1 */
	0x0B,   /* 1 0 1 1 */
	0x0D,   /* 1 1 0 1 */
	/* ------- */
	0x08,   /* 1 0 0 0 */
	0x04,   /* 0 1 0 0 */
	0x02,   /* 0 0 1 0 */
	0x01    /* 0 0 0 1 */
};

/* table of Hamming codes hammingCodes[x] is the x encoded */
const unsigned char hammingCodes[DATA_VALUES] =
{
	0x00,   /* 0 */
	0x71,   /* 1 */
	0x62,   /* 2 */
	0x13,   /* 3 */
	0x54,   /* 4 */
	0x25,   /* 5 */
	0x36,   /* 6 */
	0x47,   /* 7 */
	0x38,   /* 8 */
	0x49,   /* 9 */
	0x5A,   /* A */
	0x2B,   /* B */
	0x6C,   /* C */
	0x1D,   /* D */
	0x0E,   /* E */
	0x7F    /* F */
};

/* parity check matrix.  4 LSBs indicate which bits a parity bit represents */
const unsigned char h[PARITY_BITS] =
{
	0x47,   /* 1 0 0 | 0 1 1 1 */
	0x2B,   /* 0 1 0 | 1 0 1 1 */
	0x1D    /* 0 0 1 | 1 1 0 1 */
};

/* convert syndrome (H x data) to mask that corrects data error */
const unsigned char syndromeMask[PARITY_VALUES] =
{
	0x00,   /* syndrome = 0 0 0 */
	0x10,   /* syndrome = 0 0 1 */
	0x20,   /* syndrome = 0 1 0 */
	0x08,   /* syndrome = 0 1 1 */
	0x40,   /* syndrome = 1 0 0 */
	0x04,   /* syndrome = 1 0 1 */
	0x02,   /* syndrome = 1 1 0 */
	0x01    /* syndrome = 1 1 1 */
};

/* table convering encoded value (with error) to original data */
/* hammingDecodeValues[code] = original data */
const unsigned char hammingDecodeValues[CODE_VALUES] =
{
	0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0E, 0x07,     /* 0x00 to 0x07 */
	0x00, 0x09, 0x0E, 0x0B, 0x0E, 0x0D, 0x0E, 0x0E,     /* 0x08 to 0x0F */
	0x00, 0x03, 0x03, 0x03, 0x04, 0x0D, 0x06, 0x03,     /* 0x10 to 0x17 */
	0x08, 0x0D, 0x0A, 0x03, 0x0D, 0x0D, 0x0E, 0x0D,     /* 0x18 to 0x1F */
	0x00, 0x05, 0x02, 0x0B, 0x05, 0x05, 0x06, 0x05,     /* 0x20 to 0x27 */
	0x08, 0x0B, 0x0B, 0x0B, 0x0C, 0x05, 0x0E, 0x0B,     /* 0x28 to 0x2F */
	0x08, 0x01, 0x06, 0x03, 0x06, 0x05, 0x06, 0x06,     /* 0x30 to 0x37 */
	0x08, 0x08, 0x08, 0x0B, 0x08, 0x0D, 0x06, 0x0F,     /* 0x38 to 0x3F */
	0x00, 0x09, 0x02, 0x07, 0x04, 0x07, 0x07, 0x07,     /* 0x40 to 0x47 */
	0x09, 0x09, 0x0A, 0x09, 0x0C, 0x09, 0x0E, 0x07,     /* 0x48 to 0x4F */
	0x04, 0x01, 0x0A, 0x03, 0x04, 0x04, 0x04, 0x07,     /* 0x50 to 0x57 */
	0x0A, 0x09, 0x0A, 0x0A, 0x04, 0x0D, 0x0A, 0x0F,     /* 0x58 to 0x5F */
	0x02, 0x01, 0x02, 0x02, 0x0C, 0x05, 0x02, 0x07,     /* 0x60 to 0x67 */
	0x0C, 0x09, 0x02, 0x0B, 0x0C, 0x0C, 0x0C, 0x0F,     /* 0x68 to 0x6F */
	0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x06, 0x0F,     /* 0x70 to 0x77 */
	0x08, 0x01, 0x0A, 0x0F, 0x0C, 0x0F, 0x0F, 0x0F      /* 0x78 to 0x7F */
};

/* packed table convering encoded value (with error) to original data */
/* hammingPackedDecodeValues[code / 2] & 0x0F = original data for odd codes */
/* hammingPackedDecodeValues[code / 2] >> 4 = original data for even codes */
const unsigned char hammingPackedDecodeValues[CODE_VALUES] =
{
	0x00, 0x03, 0x05, 0xE7,     /* 0x00 to 0x07 */
	0x09, 0xEB, 0xED, 0xEE,     /* 0x08 to 0x0F */
	0x03, 0x33, 0x4D, 0x63,     /* 0x10 to 0x17 */
	0x8D, 0xA3, 0xDD, 0xED,     /* 0x18 to 0x1F */
	0x05, 0x2B, 0x55, 0x65,     /* 0x20 to 0x27 */
	0x8B, 0xBB, 0xC5, 0xEB,     /* 0x28 to 0x2F */
	0x81, 0x63, 0x65, 0x66,     /* 0x30 to 0x37 */
	0x88, 0x8B, 0x8D, 0x6F,     /* 0x38 to 0x3F */
	0x09, 0x27, 0x47, 0x77,     /* 0x40 to 0x47 */
	0x99, 0xA9, 0xC9, 0xE7,     /* 0x48 to 0x4F */
	0x41, 0xA3, 0x44, 0x47,     /* 0x50 to 0x57 */
	0xA9, 0xAA, 0x4D, 0xAF,     /* 0x58 to 0x5F */
	0x21, 0x22, 0xC5, 0x27,     /* 0x60 to 0x67 */
	0xC9, 0x2B, 0xCC, 0xCF,     /* 0x68 to 0x6F */
	0x11, 0x21, 0x41, 0x6F,     /* 0x70 to 0x77 */
	0x81, 0xAF, 0xCF, 0xFF      /* 0x78 to 0x7F */
};

class MyHamming
{
private:
	/***************************************************************************
	*                                FUNCTIONS
	***************************************************************************/

	/***************************************************************************
	*   Function   : SumBitsModulo2
	*   Description: This function performs a modulo 2 sum of the least
	*                significant CODE_BITS bits in an unsigned char.
	*   Parameters : bits - right justified array of CODE_BITS bits to be
	*                       summed modulo 2.
	*   Effects    : None
	*   Returned   : 0 if even number of bits set to 1.
	*                1 if odd number of bits set to 1.
	***************************************************************************/
	unsigned char SumBitsModulo2(unsigned char bits)
	{
		unsigned char sum, mask;

		sum = 0;

		for (mask = 0x01; mask < (0x01 << CODE_BITS); mask <<= 1)
		{
			if (bits & mask)
			{
				sum++;
			}
		}

		return (sum & 0x01);
	}

	/***************************************************************************
	*   Function   : HammingMatrixEncode
	*   Description: This function uses a generator matrix G (above) to
	*                determine the Hamming code for a DATA_BITS long value.
	*                Actually, G Transpose is used in the calculation, because
	*                it makes the C code a little easier.
	*                Notice that '&' is used for modulo 2 multiplication.
	*   Parameters : data - DATA_BITS long series of data bits to encode.
	*   Effects    : None
	*   Returned   : Hamming code used to represent data
	***************************************************************************/
	unsigned char HammingMatrixEncode(unsigned char data)
	{
		unsigned char i, code;
		unsigned char rowBits;

		code = 0;

		for (i = 0; i < CODE_BITS; i++)
		{
			code <<= 1;     /* code bits are accumlated from right to left */

			/* each component of data row times column c[i] */
			rowBits = (gT[i] & data);       /* mod 2 multiplication of vectors */

			/* determine modulo 2 sum of compoments */
			if (SumBitsModulo2(rowBits))
			{
				code |= 1;
			}
		}

		return code;
	}

	/***************************************************************************
	*   Function   : HammingTableEncode
	*   Description: This function uses a lookup table to determine the
	*                Hamming code for a DATA_BITS long value.  The lookup table
	*                is based on the codes generated by the matrix G (above).
	*   Parameters : data - DATA_BITS long series of data bits to encode.
	*   Effects    : None
	*   Returned   : Hamming code used to represent data
	***************************************************************************/
	unsigned char HammingTableEncode(unsigned char data)
	{
		return hammingCodes[data];
	}

	/***************************************************************************
	*   Function   : HammingMatrixDecode
	*   Description: This function uses the matrix H (above) to determine the
	*                value encoded by a CODE_BITS long code.  H is a parity
	*                check matrix based on the encoding matrix G.  The result
	*                of multiplying the code by H is called the syndrome.  If
	*                there are no errors in the code, the syndrome will be a 0
	*                vector.  If the syndrome is not 0, it will match a column
	*                in H.  The column it matches is likely the errored bit.
	*                Toggle the errored bit and the resulting code is the
	*                nearest matching correct code.
	*   Parameters : code - CODE_BITS long series of code bits to decode.
	*   Effects    : None
	*   Returned   : Nearest value to encoded data
	***************************************************************************/
	unsigned char HammingMatrixDecode(unsigned char code)
	{
		unsigned char i, syndromeVal;
		unsigned char syndromeColBits;  /* sum of bits is bit in syndrome */

		syndromeVal = 0;

		for (i = 0; i < PARITY_BITS; i++)
		{
			syndromeVal <<= 1;

			/* components of row h[i] times column code */
			syndromeColBits = (h[i] & code);

			/* determine modulo 2 sum of compoments */
			if (SumBitsModulo2(syndromeColBits))
			{
				syndromeVal |= 1;
			}
		}

		/* return the data corrected for error */
		return ((code ^ syndromeMask[syndromeVal]) & (0xFF >> DATA_BITS));
	}

	/***************************************************************************
	*   Function   : HammingTableDecode
	*   Description: This function uses a lookup table to determine the value
	*                encoded by a CODE_BITS long code.  It assumes that the
	*                code is based on the codes generated by the matrix G
	*                (above), but errors are allowed.  The value returned is
	*                the value with the code nearest (with the least error) to
	*                the code passed as a parameter.
	*   Parameters : code - CODE_BITS long series of code bits to decode.
	*   Effects    : None
	*   Returned   : Nearest value to encoded data
	***************************************************************************/
	unsigned char HammingTableDecode(unsigned char code)
	{
		return hammingDecodeValues[code];
	}

	/***************************************************************************
	*   Function   : HammingPackedTableDecode
	*   Description: This function uses a packed lookup table to determine the
	*                value encoded by a CODE_BITS long code.  It assumes that
	*                the code is based on the codes generated by the matrix G
	*                (above), but errors are allowed.  The value returned is
	*                the value with the code nearest (with the least error) to
	*                the code passed as a parameter.
	*                NOTE: The table packing will only work if the size of a
	*                      table entry is larger than 2 * DATA_BITS.
	*   Parameters : code - CODE_BITS long series of code bits to decode.
	*   Effects    : None
	*   Returned   : Nearest value to encoded data
	***************************************************************************/
	unsigned char HammingPackedTableDecode(unsigned char code)
	{
		unsigned char decoded;

		decoded = hammingPackedDecodeValues[code / 2];

		if (code % 2)
		{
			/* even value.  keep LS nibble */
			decoded &= (0xFF >> DATA_BITS);
		}
		else
		{
			/* odd value.  move MS nibble to LS half */
			decoded >>= DATA_BITS;
		}

		return decoded;
	}

	///////////////// Testing code //////////////////////

	/***************************************************************************
	*   Function   : TestAll
	*   Description: This function verifies that the Hamming encode function
	*                produces resutls which are correctly decoded by the decode
	*                function.  Then it verifies that all versions of the encode
	*                function produce identical results and all versions of the
	*                decode function produce identical results.  The results of
	*                each operation and any error that may occur are written
	*                to stdout.
	*   Parameters : None
	*   Effects    : Results of verify are written to stdout
	*   Returned   : None
	***************************************************************************/
	void testHamming(void)
	{
		unsigned char testValue;
		unsigned char result1, result2, result3;
		unsigned char error;

		/* verify that decode is the reverse of encode */
		printf("Verifying Matched Encode/Decode ...\n");
		printf("Value\tEncoded\tDecoded\n");
		for (testValue = 0x00; testValue < DATA_VALUES; testValue++)
		{
			result1 = HammingMatrixEncode(testValue);
			result2 = HammingMatrixDecode(result1);

			if (testValue != result2)
			{
				printf("*** Error Decoding: %02X ****\n", testValue);
			}

			printf("%02X\t%02X\t%02X\n", testValue, result1, result2);
		}

		/* verify that all versions of encode produce the same results */
		printf("\nVerifying Encodes Match ...\n");
		printf("Value\tMatrix\tTable\n");
		for (testValue = 0x00; testValue < DATA_VALUES; testValue++)
		{
			result1 = HammingMatrixEncode(testValue);
			result2 = HammingTableEncode(testValue);

			if (result1 != result2)
			{
				printf("*** Error Encoding: %02X ****\n", testValue);
			}

			printf("%02X\t%02X\t%02X\n", testValue, result1, result2);
		}

		/* verify that all versions of decode produce the same results */
		printf("\nVerifying Decodes Match ...\n");
		printf("Value\tMatrix\tTable\tPacked\n");
		for (testValue = 0x00; testValue < CODE_VALUES; testValue++)
		{
			result1 = HammingMatrixDecode(testValue);
			result2 = HammingTableDecode(testValue);
			result3 = HammingPackedTableDecode(testValue);

			if ((result1 != result2) || (result1 != result3))
			{
				printf("*** Error Decoding: %02X ****\n", testValue);
			}

			printf("%02X\t%02X\t%02X\t%02X\n",
				testValue, result1, result2, result3);
		}

		/* verify that all single bit errors are corrected */
		printf("Verifying Single Bit Errors Are Corrected ...\n");
		printf("Value\tEncoded\tError\tDecoded\n");
		for (testValue = 0x00; testValue < DATA_VALUES; testValue++)
		{
			result1 = HammingMatrixEncode(testValue);

			for (error = 0x01; error < 0x80; error <<= 1)
			{
				result2 = result1 ^ error;
				result3 = HammingMatrixDecode(result2);

				if (testValue != result3)
				{
					printf("*** Error Decoding: %02X ****\n", result2);
				}

				printf("%02X\t%02X\t%02X\t%02X\n",
					testValue, result1, result2, result3);
			}
		}
	}

	/***************************************************************************
	*   Function   : BuildCodeTable
	*   Description: This function uses HammingMatrixEncode to output text
	*                suitable to be used for replacing the array hammingCodes
	*                in hamming.c.
	*   Parameters : None
	*   Effects    : Table in format sutibale for replacing hammingCodes in
	*                hamming.c is written to stdout.
	*   Returned   : None
	***************************************************************************/
	void BuildCodeTable(void)
	{
		unsigned char value;
		unsigned char result;

		printf("Dumping Encode Table ...\n");
		printf("const unsigned char hammingCodes[DATA_VALUES] =\n");
		printf("{\n    ");
		for (value = 0x00; value < (DATA_VALUES - 1); value++)
		{
			result = HammingMatrixEncode(value);
			printf("0x%02X,  /* %X */\n    ", result, value);
		}

		result = HammingMatrixEncode(value);
		printf("0x%02X   /* %X */\n};\n", result, value);
	}

	/***************************************************************************
	*   Function   : BuildDecodeTables
	*   Description: This function uses HammingMatrixDecode to output text
	*                suitable to be used for replacing the arrays
	*                hammingDecodeValues and hammingPackedDecodeValues in
	*                hamming.c.
	*   Parameters : None
	*   Effects    : Tables in format sutibale for replacing
	*                hammingDecodeValues and hammingPackedDecodeValues in
	*                hamming.c are written to stdout.
	*   Returned   : None
	***************************************************************************/
	void BuildDecodeTables(void)
	{
		unsigned char value;
		unsigned char result;

		printf("Dumping Decode Table ...\n");
		printf("const unsigned char hammingDecodeValues[CODE_VALUES] =\n");
		printf("{\n    ");
		for (value = 0x00; value < (CODE_VALUES - 1); value++)
		{
			result = HammingMatrixDecode(value);
			printf("0x%02X, ", result);

			if (7 == (value % 8))
			{
				printf("    /* 0x%02X to 0x%02X */\n    ", (value - 7), value);
			}
		}

		result = HammingMatrixDecode(value);
		printf("0x%02X      /* 0x%02X to 0x%02X */\n};\n",
			result, (value - 7), value);

		printf("\nDumping Packed Decode Table ...\n");
		printf("const unsigned char hammingPackedDecodeValues[CODE_VALUES] =\n");
		printf("{\n    ");
		for (value = 0x00; value < (CODE_VALUES - 2);)
		{
			result = HammingMatrixDecode(value++);
			printf("0x%1X", result);

			result = HammingMatrixDecode(value++);
			printf("%1X, ", result);

			if (0 == (value % 8))
			{
				printf("    /* 0x%02X to 0x%02X */\n    ",
					(value - 8), (value - 1));
			}
		}

		result = HammingMatrixDecode(value++);
		printf("0x%1X", result);

		result = HammingMatrixDecode(value);
		printf("%1X      /* 0x%02X to 0x%02X */\n};\n", result, (value - 7), value);
	}
public:
	/***************************************************************************
	*   Function   : EncodeMessage
	*   Description: This function takes message as vector<short> and convert to
					 the char representation used in this class then encodes and
					 returns the result
	*   Parameters : vector<short> original message of zeros and ones
	*   Returned   : vector<short> with the encoded message
	***************************************************************************/
	vector<short> EncodeMessage(vector<short> msg,bool interleave)
	{
		vector<short> result;
		// assume the message is multiple of 4
		// take each four bits
		for (int i = 0; i < msg.size() - (msg.size() % 4);)
		{
			unsigned char input = 0;
			for (int j = 0; j < 4; j++,i++)
			{
				input = ((input << 1) | msg[i]);
			}
			unsigned char output = HammingMatrixEncode(input);
			for (int j = 6; j >= 0; j--)
			{
				result.push_back((output >> j) & 1);
			}
		}
		if (interleave)
		{
			vector<short> temp = result;
			// each two codes 7x2 bits should be combined together
			for (int i = 0; i < temp.size(); i += 14)
			{
				for (int j = 0; j < 7; j++)
				{
					result[i + (j * 2)] = temp[i + j];
					result[i + (j * 2) + 1] = temp[i + j + 7];
				}
			}
		}

		return result;
	}
	/***************************************************************************
	*   Function   : DecodeMessage
	*   Description: This function takes received message as vector<short> and convert to
						the char representation used in this class then decodes and
						returns the result
	*   Parameters : vector<short> received message of zeros and ones
	*   Returned   : vector<short> with the dencoded message
	***************************************************************************/
	vector<short> DecodeMessage(vector<short> msg,bool interleave)
	{
		vector<short> result;
		// assume the message is multiple of 7
		// take each four bits
		if (interleave)
		{
			vector<short> temp = msg;
			// each two codes 7x2 bits should be combined together
			for (int i = 0; i < temp.size(); i += 14)
			{
				if (temp.size() - 14 >= i)
				{
					for (int j = 0; j < 7; j++)
					{
						msg[i + j] = temp[i + (j * 2)];
						msg[i + j + 7] = temp[i + (j * 2) + 1];
					}
				}
			}
		}
		for (int i = 0; i < msg.size() - (msg.size() % 7);)
		{
			unsigned char input = 0;
			for (int j = 0; j < 7; j++,i++)
			{
				input = ((input << 1) | msg[i]);
			}
			unsigned char output = HammingMatrixDecode(input);
			for (int j = 3; j >= 0; j--)
			{
				result.push_back((output >> j) & 1);
			}
		}
		return result;
	}
};