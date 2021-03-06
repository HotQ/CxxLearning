#include <cstdlib>
#include <cmath>
#include <cstring>
#include "Integer.h"

#ifdef SHOWLOG
#include <iostream>
#endif // SHOWLOG

#define Log_2_10 3.3219280959

static unsigned char product_256_nooffset[10][4] = {
	{ 0,0,0,0 },
	{ 0,2,5,6 },
	{ 0,5,1,2 },
	{ 0,7,6,8 },
	{ 1,0,2,4 },
	{ 1,2,8,0 },
	{ 1,5,3,6 },
	{ 1,7,9,2 },
	{ 2,0,4,8 },
	{ 2,3,0,4 }
};
static inline unsigned char chr_2_0x(char chr);
static inline int divide256(char *dividend_offset);
static inline void divident_sub_256(char *dividend_offset, int quotient);
static inline int zero_number(char *dividend);


Integer::Integer()
	:init(0), byte(0), data(nullptr)
{
#ifdef SHOWLOG
	std::clog << this << "\t default" << std::endl;
#endif // SHOWLOG
}
Integer::Integer(int int_src)
	:init(1)
{
#ifdef SHOWLOG
	std::clog << this << "\t int" << std::endl;
#endif // SHOWLOG
	auto temp = (unsigned char*)malloc(sizeof(int));
	if (temp)
		this->data = temp;

	if (int_src == 0) {
		this->sign = 0;
		this->zero = 1;
	}
	else
		this->zero = 0;

	this->byte = (int)sizeof(int);

	if (int_src > 0)
		this->sign = 0;
	else if (int_src < 0) {
		this->sign = 1;
		int_src = -int_src;
	}

	for (int i = 0; i < (int)sizeof(int); i++) {
		data[i] = int_src % 0x100;
		int_src = int_src >> 8;
	}
}
Integer::Integer(long long int_src)
	:init(1)
{
	auto temp = (unsigned char*)malloc(sizeof(int));
	if (temp)
		this->data = temp;
	this->byte = (int)sizeof(long long);

	if (int_src == 0)
		this->zero = 1;
	else
		this->zero = 0;

	if (int_src > 0)
		this->sign = 0;
	else {
		this->sign = 1;
		int_src = -int_src;
	}

	for (int i = 0; i < (int)sizeof(long long); i++) {
		data[i] = int_src % 0x100;
		int_src = int_src >> 8;
	}
}
Integer::Integer(const char *cchr_src)
	:init(1)
{
#ifdef SHOWLOG
	std::clog << this << "\t cchr" << std::endl;
#endif // SHOWLOG

	int length = (int)strlen(cchr_src),
		lengthOffset = 0,
		i, j;

	if (cchr_src[0] == '-') {
		this->sign = 1;
		lengthOffset++;
	}
	else
		this->sign = 0;

	if (cchr_src[lengthOffset] == '0' && length == 1 + lengthOffset) {
		this->zero = 1;
		this->data = (unsigned char *)malloc(sizeof(int));
		memset(this->data, 0, sizeof(int));
	}
	else if (cchr_src[lengthOffset] == '0' && !(cchr_src[lengthOffset + 1] == 'x' || cchr_src[lengthOffset + 1] == 'X')) {
		/// string to octonary

		lengthOffset++;
		length -= lengthOffset;
		const char * str_src = cchr_src + lengthOffset;

		int digits = (int)ceil((double)length * 3 / 8);
		digits = (sizeof(int) > digits ? sizeof(int) : digits);

		this->byte = digits;
		this->data = (unsigned char *)calloc(digits, sizeof(unsigned char));


		for (i = 0; i < length; i++) {
			j = 3 * (length - 1 - i);
			unsigned char chr = str_src[i] - '0';

			this->data[(j + 0) / 8] += (chr & (unsigned char)1) / 1 * (unsigned char)pow(2, (j + 0) % 8);
			this->data[(j + 1) / 8] += (chr & (unsigned char)2) / 2 * (unsigned char)pow(2, (j + 1) % 8);
			this->data[(j + 2) / 8] += (chr & (unsigned char)4) / 4 * (unsigned char)pow(2, (j + 2) % 8);
		}
	}
	else if (cchr_src[lengthOffset] == '0' && (cchr_src[lengthOffset + 1] == 'x' || cchr_src[lengthOffset + 1] == 'X')) {
		/// string to hexadecimal

		lengthOffset += 2;
		length -= lengthOffset;
		const char * str_src = cchr_src + lengthOffset;


		int digits = (int)ceil((double)length / 2);
		digits = (sizeof(int) > digits ? sizeof(int) : digits);

		this->byte = digits;
		this->data = (unsigned char *)calloc(digits, sizeof(unsigned char));

		j = 0;
		for (i = length - 1; i > 0; i -= 2)
			(this->data)[j++] = chr_2_0x(str_src[i - 1]) * 16 + chr_2_0x(str_src[i]);
		if (i == 0)
			(this->data)[j] = chr_2_0x(str_src[0]);
	}
	else {
		/// string to decimal

		length -= lengthOffset;
		int digits = (int)ceil((double)length* Log_2_10 / 8);
		digits = (sizeof(int) > digits ? sizeof(int) : digits);

		auto temp = (unsigned char *)calloc(digits, sizeof(unsigned char));
		if (temp) {
			this->data = temp;
			this->byte = digits;
		}

		if (length == 1) {
			(this->data)[0] = (unsigned char)(cchr_src[lengthOffset] - '0');
		}
		else if (length>1) {

			char *dividend = new char[length + 1],
				*quotient = new char[length + 1](),
				*swap;

			dividend[0] = 0;
			for (i = 1; i < length + 1; i++)
				dividend[i] = cchr_src[i - 1 + lengthOffset] - '0';

			j = 0;
			while (length + 1 - zero_number(dividend) >= 3) {

				for (i = 0; i <= length - 3; i++) {
					quotient[i + 3] = divide256(dividend + i);
				}
				(this->data)[j++] = (unsigned char)(100 * dividend[length - 2] + 10 * dividend[length - 1] + dividend[length]);

				swap = dividend;
				dividend = quotient;
				quotient = swap;

				quotient[length] = 0;
				quotient[length - 1] = 0;
				quotient[length - 2] = 0;
			}
			if (j != digits)
				(this->data)[j++] = (unsigned char)(100 * dividend[length - 2] + 10 * dividend[length - 1] + dividend[length]);
			delete[] dividend;
			delete[] quotient;
		}
	}
}
Integer::Integer(const Integer& c)
	:init(c.init), byte(c.byte)
{
#ifdef SHOWLOG
	std::clog << this << "\t copy" << std::endl;
#endif // SHOWLOG

	this->init = c.init;
	this->byte = c.byte;
	if (c.init) {
		this->sign = c.sign;
		this->zero = c.zero;
		if (c.byte) {
			this->data = (unsigned char *)malloc(c.byte);
			if (this->data)
				memcpy(this->data, c.data, c.byte);
		}
		else
			this->data = NULL;

	}
	else
		this->data = NULL;
}
Integer::Integer(Integer&& c)
	: init(c.init), byte(c.byte), sign(c.sign), zero(c.zero)
{
#ifdef SHOWLOG
	std::clog << this << "\t move !!!" << std::endl;
#endif // SHOWLOG

	if (c.data) {
		this->data = c.data;
		c.data = nullptr;
	}
}
Integer& Integer::operator=(const Integer& c) {
#ifdef SHOWLOG
	std::clog << this << "\t\t\t copy assignment" << std::endl;
#endif // SHOWLOG
	free(this->data);
	this->init = c.init;
	this->byte = c.byte;
	if (c.init) {
		this->sign = c.sign;
		this->zero = c.zero;
		if (c.byte) {
			this->data = (unsigned char *)malloc(c.byte);
			if (this->data)
				memcpy(this->data, c.data, c.byte);
		}
		else
			this->data = NULL;
	}
	else
		this->data = NULL;

	return *this;
}
Integer& Integer::operator=(Integer&& c) {
#ifdef SHOWLOG
	std::clog << this << "\t\t\t move assignment" << std::endl;
#endif // SHOWLOG
	this->init = c.init;
	this->byte = c.byte;
	if (c.init) {
		this->sign = c.sign;
		this->zero = c.zero;
		this->data = c.data;
		c.data = nullptr;
	}

	return *this;
}
Integer& Integer::operator=(int int_src) {
	Integer temp(int_src);
	*this = temp;
	return *this;
}
Integer& Integer::operator=(long long int_src) {
	Integer temp(int_src);
	*this = temp;
	return *this;
}
Integer& Integer::operator=(const char *cchr_src) {
	Integer temp(cchr_src);
	*this = temp;
	return *this;
}

Integer::~Integer() {
#ifdef SHOWLOG
	if (this->data)
		std::clog << this << "\t\t ! has destroyed" << std::endl;
	else
		std::clog << this << "\t\t ~ has destroyed" << std::endl;
#endif // SHOWLOG
	if (this->data)
		free(this->data);
}

static inline unsigned char chr_2_0x(char chr) {
	if (chr >= 'a' && chr <= 'f') return (unsigned char)(chr - 'a' + 10);
	if (chr >= '0' && chr <= '9') return (unsigned char)(chr - '0');
	return -1;
}
static inline int divide256(char *dividend_offset) {
	switch (dividend_offset[0])
	{
	case 0:
		switch (dividend_offset[1]) {
		case 0:
		case 1:
			return 0;
		case 2:
			if (dividend_offset[2] < 5 || (dividend_offset[2] == 5 && dividend_offset[3] < 6))
				return 0;
		case 3:
		case 4:
			divident_sub_256(dividend_offset, 1);
			return 1;
		case 5:
			if (dividend_offset[2] < 1 || (dividend_offset[2] == 1 && dividend_offset[3] < 2)) {
				divident_sub_256(dividend_offset, 1);
				return 1;
			}
		case 6:
			divident_sub_256(dividend_offset, 2);
			return 2;
		case 7:
			if (dividend_offset[2] < 6 || (dividend_offset[2] == 6 && dividend_offset[3] < 8)) {
				divident_sub_256(dividend_offset, 2);
				return 2;
			}
		case 8:
		case 9:
			divident_sub_256(dividend_offset, 3);
			return 3;
		}
	case 1:
		switch (dividend_offset[1]) {
		case 0:
			if (dividend_offset[2] < 2 || (dividend_offset[2] == 2 && dividend_offset[3] < 4)) {
				divident_sub_256(dividend_offset, 3);
				return 3;
			}
		case 1:
			divident_sub_256(dividend_offset, 4);
			return 4;
		case 2:
			if (dividend_offset[2] < 8) {
				divident_sub_256(dividend_offset, 4);
				return 4;
			}
		case 3:
		case 4:
			divident_sub_256(dividend_offset, 5);
			return 5;
		case 5:
			if (dividend_offset[2] < 3 || (dividend_offset[2] == 3 && dividend_offset[3] < 6)) {
				divident_sub_256(dividend_offset, 5);
				return 5;
			}
		case 6:
			divident_sub_256(dividend_offset, 6);
			return 6;
		case 7:
			if (dividend_offset[2] < 9 || (dividend_offset[2] == 9 && dividend_offset[3] < 2)) {
				divident_sub_256(dividend_offset, 6);
				return 6;
			}
		case 8:
		case 9:
			divident_sub_256(dividend_offset, 7);
			return 7;
		}
	case 2:
		switch (dividend_offset[1])
		{
		case 0:
			if (dividend_offset[2] < 4 || (dividend_offset[2] == 4 && dividend_offset[3] < 8)) {
				divident_sub_256(dividend_offset, 7);
				return 7;
			}
		case 1:
		case 2:
			divident_sub_256(dividend_offset, 8);
			return 8;
		case 3:
			if (dividend_offset[2] < 0 || (dividend_offset[2] == 0 && dividend_offset[3] < 4)) {
				divident_sub_256(dividend_offset, 8);
				return 8;
			}
		case 4:
		case 5:
			divident_sub_256(dividend_offset, 9);
			return 9;
		}
	}
	return -1;
}
static inline void divident_sub_256(char *dividend_offset, int quotient) {
	int CF = 0, temp;
	for (int i = 3; i >= 0; i--) {
		temp = dividend_offset[i] - CF - (int)product_256_nooffset[quotient][i];
		CF = (temp < 0);
		dividend_offset[i] = (char)(10 * CF + temp);
	}

}
static inline int zero_number(char *dividend) {
	int i = 0;
	while (dividend[i] == 0)i++;
	return i;
}
