#include <stdio.h>

int f(unsigned int n) {
	if (n == 0) return 1;
	else if (n == 1) return 1;
	return f(n - 2) + f(n - 1);
}
int main(int argc, char **argv) {
	unsigned int n = 0;
	scanf_s("%u", n);
	n = f(n);
	printf("f(n) = %u\n", n);
	return 0;
}