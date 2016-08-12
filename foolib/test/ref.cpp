#include <stdio.h>

class Ref {

public:
	Ref& my(int i) {
		if (i==0) {
			return *this;
		} else {
			return NULL;
		}
	}

};


int main()
{
	Ref ref;
	Ref my;

	my = ref.my(0);

	printf("%p\n", &my);
}
