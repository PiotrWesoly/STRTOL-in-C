
#include <ctype.h>
#include <limits.h>
#include <errno.h>



long strol(const char* nPtr, char **endPtr, int base) {
	char *current_char = (char*)nPtr;
	long return_value = 0;
	int sign = 1;
	int current_int;
	int overflow_flag = 0;

	if (endPtr)  *endPtr = (char*)nPtr;

	// pomijanie biaï¿½ych znakï¿½w
	while (isspace(*current_char)) {
		++current_char;
	}
	// sprawdzanie znaku
	if (*current_char == '-') {
		sign = 0;
		++current_char;
	}
	else if (*current_char == '+') {
		++current_char;
	}
	// sprawdzenie systemu liczbowego
	if ((base == 0 || base == 16) && *current_char == '0'
		&& (*(current_char + 1) == 'x' || *(current_char + 1) == 'X')) {
		current_char += 2;
		base = 16;
	}
	if (base == 0) {
		if (*current_char == '0') {
			base = 8;
		}
		else {
			base = 10;
		}
	}
	if (base < 2 || base > 36) {
		errno = EINVAL;
		return 0;
	}
	int didstep = 0; // czy by³ ruch
	do {
		if (isdigit(*current_char)) {
			current_int = *current_char - '0';
		}
		else if (isalpha(*current_char)) {
			if (isupper(*current_char)) {
				current_int = (*current_char - 'A') + 10;
			}
			else {
				current_int = (*current_char - 'a') + 10;
			}
		}
		else {
			break;
		}

		if (current_int >= base) { // sprawdzanie czy na nasza liczba miesi sie w systemie liczbowym
			break;
		}

		if (sign) {
			if (overflow_flag || return_value > (LONG_MAX - current_int) / base) {
				overflow_flag = 1;
				++current_char;
			}
			else {
				return_value = return_value * base + current_int;
				++current_char;
			}
		}
		else {
			if (overflow_flag || return_value < (LONG_MIN + current_int) / base) {
				overflow_flag = 1;
				++current_char;
			}
			else {
				return_value = return_value * base - current_int;
				++current_char;
			}
		}
		didstep = 1;  // wykonaliœmy krok
	} while (*current_char != '\0');

	if (overflow_flag) {
		if (sign) {
			return_value = LONG_MAX;
		}
		else {
			return_value = LONG_MIN;
		}
		errno = ERANGE;
	} 

	if (endPtr != 0) {
		if (overflow_flag || didstep == 1) {
			*endPtr = current_char; // pointer w miejscu w ktorym wykonalismy krok
		}
		else {
			*endPtr = (char*)nPtr;  // pointer na pocztku
		}
	}

	return return_value;
}
