#include <iostream>
#include <cstdlib>
#include <cmath>
#include "Integer.h"

Integer &Integer_add(const Integer &ax, const Integer &bx, Integer &lhs);
Integer &Integer_sub(const Integer &ax, const Integer &bx, Integer &lhs);

Integer& Integer::reset() {
	if (this->data) {
		if (this->byte != 4) {
			free(this->data);
			this->byte = sizeof(int);
			this->data = (unsigned char *)malloc(sizeof(int));
		}
	}
	else {
		this->byte = sizeof(int);
		this->data = (unsigned char *)malloc(sizeof(int));
	}
	for (int i = 0; i < (int)sizeof(int); i++) {
		(this->data)[i] = (unsigned char)0;
	}
	this->zero = 1;
	this->init = 1;
	return *this;
}
Integer& Integer::expand() {
	return expand(1);
}
Integer& Integer::expand(int d) {
	if (d > 0) {
		if (this->init == 0) {
			this->init = 1;
			this->zero = 1;
			this->sign = 0;
			this->byte = d;
			this->data = (unsigned char *)malloc(d * sizeof(unsigned char));
			memset(this->data, 0, sizeof(unsigned char));
		}
		else {
			int bytes = (this->byte);
			this->byte += d;
			this->data = (unsigned char *)realloc(this->data, this->byte);

			for (int i = bytes; i < (int)(this->byte); i++)
				(this->data)[i] = 0;
		}
	}
	return *this;
}
Integer Plus(Integer &ax, Integer &bx) {
	Integer result;
	if (ax.zero == 1 || bx.zero == 1) {
		if (ax.zero == 1)
			result = bx;
		else 
			result = ax;
	}
	else {
		if (ax.sign == bx.sign) {
			Integer_add(ax, bx, result);
			result.sign = ax.sign;
		}
		else
			switch (Integer_compare_abs(ax, bx)) {
			case -1:
				Integer_sub(bx, ax, result);
				result.sign = bx.sign;
				break;
			case  1:
				Integer_sub(ax, bx, result);
				result.sign = ax.sign;
				break;
			case 0:
				result = 0;
				break;
			}
	}

	return result;
}
Integer Plus(Integer &ax, int bx) {
	Integer temp = bx;
	return Plus(ax, temp);
}
Integer Plus(int ax, Integer &bx) {
	Integer temp = ax;
	return Plus(temp, bx);
}
Integer Plus(int ax, int bx) {
	Integer tax = ax, tbx = bx;
	return Plus(tax, tbx);
}
Integer operator+(Integer &ax, Integer &bx) {
	return Plus(ax, bx);
}
Integer operator+(Integer &ax, int bx) {
	Integer temp = bx;
	return Plus(ax, temp);
}
Integer operator+(int ax, Integer &bx) {
	Integer temp = ax;
	return Plus(temp, bx);
}

///a bad lazy implement
Integer Subtract(Integer &ax, Integer &bx) {
	bx.sign = 1 - bx.sign;
	Integer result = Plus(ax, bx);
	bx.sign = 1 - bx.sign;
	
	return result;
}
Integer Subtract(Integer &ax, int bx) {
	Integer temp = bx;
	return Subtract(ax, temp);
}
Integer Subtract(int ax, Integer &bx) {
	Integer temp = ax;
	return Subtract(temp, bx);
}
Integer Subtract(int ax, int bx) {
	Integer tax = ax, tbx = bx;
	return Subtract(tax, tbx);
}
Integer operator-(Integer &ax, Integer &bx) {
	return Subtract(ax, bx);
}
Integer operator-(Integer &ax, int bx) {
	Integer temp = bx;
	return Subtract(ax, temp);
}
Integer operator-(int ax, Integer &bx) {
	Integer temp = ax;
	return Subtract(temp, bx);
}

Integer& Integer::add(const Integer &c) {
	return *this;
}

Integer &Integer_add(Integer &ax, Integer &bx, Integer &lhs) {
	Integer *max, *min;
	int max_byte, min_byte;

	if (ax.byte == bx.byte) {
		max = &ax;
		min = &bx;
		max_byte = max->byte;
		min_byte = min->byte;

		if ((max->data)[max->byte - 1] + (min->data)[min->byte - 1] >= 0xfe)
			lhs.expand(max->byte + 1 - lhs.byte);
		else
			lhs.expand(max->byte - lhs.byte);
	}
	else {
		if (ax.byte > bx.byte) {
			max = &ax;
			min = &bx;
		}
		else {
			min = &ax;
			max = &bx;
		}

		max_byte = max->byte;
		min_byte = min->byte;

		if ((max->data)[max->byte - 1] >= 0xfe)
			lhs.expand(max->byte + 1 - lhs.byte);
		else
			lhs.expand(max->byte - lhs.byte);
	}

	unsigned char CF = 0;
	for (int i = 0; i < min_byte; i++) {
		int temp = (int)((max->data)[i] + (min->data)[i] + CF);

		(lhs.data)[i] = temp % 256;
		CF = temp / 256;
	}
	for (int i = min_byte; i < max_byte; i++) {
		int temp = (int)((max->data)[i] + CF);
		(lhs.data)[i] = temp % 256;
		CF = temp / 256;
	}
	if (lhs.byte > max->byte)
		(lhs.data)[max->byte] = CF;
	if (lhs.zero == 1)
		lhs.zero = 0;

	return lhs;
}
/// abs(ax) is always bigger than abs(bx)
Integer &Integer_sub(Integer &ax, Integer &bx, Integer &lhs) {
	unsigned char CF = 0;
	int temp;
	int ax_real_byte = (int)ceil((double)(ax.bidigits()) / 8),
		bx_real_byte = (int)ceil((double)(bx.bidigits()) / 8);
	
	lhs.expand(ax_real_byte - lhs.byte);

	for (int i = 0; i < bx_real_byte; i++) {
		temp = (ax.data)[i] - (bx.data)[i] - CF;
		CF = (temp < 0);
		(lhs.data)[i] = (char)(256 * CF + temp);
	}
	for (int i = bx_real_byte; i < ax_real_byte; i++) {
		temp = (ax.data)[i] - CF;
		CF = (temp < 0);
		(lhs.data)[i] = (char)(256 * CF + temp);
	}
	if (lhs.zero)
		lhs.zero = 0;

	return lhs;
}
