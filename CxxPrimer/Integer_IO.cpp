#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include "Integer.h"

#define Log_10_2 0.3010299957

struct intString {
	int   digits;
	unsigned char *string;
};
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

static intString* intString_init(int d);
static intString* intString_init(int d, int int_src);
static intString* intString_init(int d, unsigned char chr_src);
static void intString_print(intString *intStr);
static void intString_destroy(intString* intStr);
static intString* intString_add(intString *ax, intString *bx);
static intString* intString_add(intString *ax, unsigned char bx);
static intString* intString_mul256(intString **ax);


void Integer::print() {
	int digits = 0;
	intString *temp_intString;

	if (this->zero)
		std::cout << '0';
	else {
		if (this->sign)std::cout << '-';
		int digits = this->bidigits();
		if (digits <= 8)
			std::cout << (unsigned)(this->data)[0];
		else {
			temp_intString = intString_init((int)ceil(Log_10_2*(double)digits), (this->data)[this->byte - 1]);
			for (int i = this->byte - 2; i >= 0; i--) {
				intString_add(intString_mul256(&temp_intString), (this->data)[i]);
			}
			intString_print(temp_intString);
			intString_destroy(temp_intString);
		}
	}
}

int Integer::toInt() {
	int result = (((int)(this->data[3])) << 24) + (((int)(this->data[1])) << 16) + (((int)(this->data[1])) << 8) + (int)(this->data[0]);
	if (this->sign)
		return 0 - result;
	else
		return  result;
}

std::string Integer::toString() {
	int digits = 0;
	intString *temp_intString;
	std::string result;
	if (this->zero)
		result = '0';
	else {
		if (this->sign)	result = '-';
		int digits = this->bidigits();
		if (digits <= 8)
		{
			char *temp = new char[3];
			sprintf_s(temp, 3, "%d", (unsigned)(this->data)[0]);
			result += temp;
			delete[] temp;
		}
		else {
			temp_intString = intString_init((int)ceil(Log_10_2*(double)digits), (this->data)[this->byte - 1]);
			for (int i = this->byte - 2; i >= 0; i--) {
				intString_add(intString_mul256(&temp_intString), (this->data)[i]);
			}
			int i = (temp_intString->digits) - 1;
			while ((temp_intString->string)[i] == 0)i--;
			for (; i >= 0; i--) {
				result += (char)((temp_intString->string)[i] + '0');
			}
			intString_destroy(temp_intString);
		}
	}
	return result;
}
std::ostream& operator<<(std::ostream &os, Integer &c) {
	c.print();
	return os;
}
std::istream& operator>>(std::istream &is, Integer &c) {
	std::string tempStr;
	is >> tempStr;
	c = tempStr.c_str();
	return is;
}


static intString* intString_init(int d) {
	intString *p;
	auto temp1 = (intString*)malloc(sizeof(intString));
	auto temp2 = (unsigned char*)calloc(d, sizeof(int));
	if (temp1) {
		p = temp1;
		if (temp2) {
			p->string = temp2;
			p->digits = d;
		}
		return p;
	}
	return nullptr;
}
static intString* intString_init(int d, int int_src) {
	intString *result = intString_init(d);
	int int_src_digits = (int)ceil(log10((double)(int_src + 1)));
	for (int i = 0; i < int_src_digits; i++) {
		(result->string)[i] = int_src % 10;
		int_src /= 10;
	}
	return result;
}
static intString* intString_init(int d, unsigned char chr_src) {
	intString *result = intString_init(d);
	for (int i = 0; i < 3; i++) {
		(result->string)[i] = chr_src % 10;
		chr_src /= 10;
	}
	return result;
}
static void intString_print(intString *intStr) {
	int i = (intStr->digits) - 1;
	while ((intStr->string)[i] == 0)i--;
	for (; i >= 0; i--) {
		std::cout << (char)((intStr->string)[i] + '0');
	}
}
static void intString_destroy(intString* intStr) {
	free((unsigned char *)(intStr->string));
	free(intStr);
}
/// <returns> ax += bx </returns>
static intString* intString_add(intString *ax, intString *bx) {
	int CF = 0, temp;
	for (int i = 0; i < ax->digits; i++) {
		temp = (ax->string)[i] + (bx->string)[i] + CF;
		CF = temp / 10;
		(ax->string)[i] = temp % 10;
	}
	return ax;
}
static intString* intString_add(intString *ax, unsigned char bx) {
	int CF = 0, temp;
	for (int i = 0; i < ax->digits; i++) {
		temp = (ax->string)[i] + bx % 10 + CF;
		CF = temp / 10;
		bx /= 10;
		(ax->string)[i] = temp % 10;
	}
	return ax;
}
/// <returns> *ax += 256 </returns>
static intString* intString_mul256(intString **ax) {
	intString *result = intString_init((*ax)->digits);
	unsigned char CF = 0, temp = 0;

	for (int i = 0; i < (*ax)->digits; i++) {
		CF = 0;
		temp = 0;
		unsigned char *product = product_256_nooffset[(int)((*ax)->string)[i]];
		for (int j = 0; j < (4 < ((*ax)->digits - i) ? 4 : (*ax)->digits - i); j++) {
			temp = (result->string)[i + j] + product[3 - j] + CF;
			CF = temp / 10;
			(result->string)[i + j] = temp % 10;
		}
	}
	intString_destroy(*ax);
	(*ax) = result;
	return (*ax);
}
