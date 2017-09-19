#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "periph/gpio.h"
#include "xtimer.h"
//#include "msg.h"

#ifdef MODULE_NETIF
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#endif

gpio_t trigger_pin = GPIO_PIN(0, 2);
gpio_t echo_pin = GPIO_PIN(0, 5);

volatile uint32_t echo_time_start;
volatile uint32_t echo_time;

//msg_t distance_msg;

void echo_cb(void* arg){
	//printf("%s: got interrupt\n", __func__);
	int val = gpio_read(echo_pin);
	uint32_t echo_time_stop;

	if(val){
		// got rising edge -> start time measuring
		echo_time_start = xtimer_now_usec();
		//printf("start time: %ld\n", echo_time_start);
	} else{
		// got falling edge -> stop time measuring
		echo_time_stop = xtimer_now_usec();
		echo_time = echo_time_stop - echo_time_start;

		//printf("stop time: %ld\n", echo_time_stop);
		//printf("echo time: %ld\n", echo_time);
		//distance_msg.content.value = echo_time;
		//msg_send(&distance_msg, );
	}
}

int main(void)
{

#ifdef MODULE_NETIF
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
#endif

	gpio_init(trigger_pin, GPIO_OUT);
	//gpio_init_int(echo_pin, GPIO_IN_PD, GPIO_RISING, &echo_cb, NULL);
		gpio_init_int(echo_pin, GPIO_IN_PD, GPIO_BOTH, &echo_cb, NULL);

    (void) puts("Welcome to RIOT!");

	uint32_t distance;
	while(1){
		echo_time = 0;
		printf("triggering sensor...\n");
		gpio_clear(trigger_pin);
		xtimer_usleep(20);
		//xtimer_usleep(1000);
		//xtimer_usleep(1000* 1000);
		gpio_set(trigger_pin);

		printf("waiting...\n");
		xtimer_usleep(100*1000);

		printf("echo_time=%ld\n", echo_time);
		if(echo_time > 0){
			distance = echo_time/58;
			printf("distance=%ld\n", distance);
		}
		//xtimer_usleep(900*1000);
		xtimer_usleep(1000*1000);
	}

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
