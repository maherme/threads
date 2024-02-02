#include "dinner_phi.h"
#include "threads.h"

int
main()
{
    spoons_init();
    philosophers_init();
    philosophers_create();

    pthread_exit(NULL);
}
