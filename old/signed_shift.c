#include <stdio.h>

int main(void) {
	for (int i = 0; i < 255; i++) {
		signed char signed_char = i;
		unsigned char unsigned_char = i;
		printf("Signed:   %#010b (%4d); >> 4 → %#010b (%4d)    |    Unsigned: %#010b (%4u); >> 4 → %#010b (%4u)\n", unsigned_char, signed_char, (unsigned char) (signed_char >> 4), signed_char >> 4, unsigned_char, unsigned_char, unsigned_char >> 4, unsigned_char >> 4);
	}
}
