#include <stdio.h>

int main()

{

        int i = 0x00000001;

            if( ((char *)&i)[0] )

                        printf( "Littile Endian\n" );

                else

                            printf( "Big Endian\n" );

}

    
