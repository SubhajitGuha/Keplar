#include "core/logger.h"
int main(void)
{
    KDEUG("test message %i, %f", 1, 10.0);
    KWARN("test message %i, %f", 1, 10.0);
    KERROR("test message %i, %f", 1, 10.0);
    KFETAL("test message %i, %f", 1, 10.0);
    KINFO("test message %i, %f", 1, 10.0);
    KTRACE("test message %i, %f", 1, 10.0);

    return 0;
}