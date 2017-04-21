#include <mach/task.h>
#include <mach/mach_init.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

static bool amIAnInferior(void)
{
	mach_msg_type_number_t count = 0;
	exception_mask_t masks[EXC_TYPES_COUNT];
	mach_port_t ports[EXC_TYPES_COUNT];
	exception_behavior_t behaviors[EXC_TYPES_COUNT];
	thread_state_flavor_t flavors[EXC_TYPES_COUNT];
	exception_mask_t mask = EXC_MASK_ALL & ~(EXC_MASK_RESOURCE | EXC_MASK_GUARD);

	kern_return_t result = task_get_exception_ports(mach_task_self(), mask, masks, &count, ports, behaviors, flavors);
	if (result == KERN_SUCCESS)
	{
		for (mach_msg_type_number_t portIndex = 0; portIndex < count; portIndex++)
		{
			if (MACH_PORT_VALID(ports[portIndex]))
			{
				return true;
			}
		}
	}
	return false;
}

int main(int argc, const char *argv[])
{
    while(1){
        sleep(1);

        if(amIAnInferior()){
            printf("I am an inferior\n");
        }else{
            printf("I am not! \n ");
        }
    }
    return 0;
}
