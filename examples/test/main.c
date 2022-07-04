#include "../../cbuild.h"

#include <stdio.h>

int main(int argc, char** argv)
{
	_shift(&argc, &argv);

	while (argc > 0)
	{
		const char* flag = _shift(&argc, &argv);

		if (STREQL(flag, "hello"))
		{
			fprintf(stdout, "Hello to you too!!\n");
		}
		else if (STREQL(flag, "?"))
		{
			fprintf(stdout, "You will figure it out :D!\n");
		}
		else
		{
			fprintf(stdout, "I am lost for words...\n");
		}
	}

	fprintf(stdout, "I am test for CBuild!\n");
	return 0;
}
