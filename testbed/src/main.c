#include "core/logger.h"
#include "core/kassert.h"
#include "platform/platform.h"

int main(void)
{
    KDEUG("test message %i, %f", 1, 10.0);
    KWARN("test message %i, %f", 1, 10.0);
    KERROR("test message %i, %f", 1, 10.0);
    KFETAL("test message %i, %f", 1, 10.0);
    KINFO("test message %i, %f", 1, 10.0);
    KTRACE("test message %i, %f", 1, 10.0);
    // KASSERT_MSG(1==0, "incorrect evaluation");
    platform_state plat_state;

    if(platform_startup(&plat_state, "Keplar Engine", 100,100,1920,1080))
    {
        while(TRUE)
        {
            platform_pump_messages(&plat_state);
        }
    }
    return 0;
}