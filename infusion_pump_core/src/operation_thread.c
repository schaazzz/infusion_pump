/*
 * File    - operation_thread.c
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <hw/inout.h>
#include <unistd.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <sys/mman.h>

/*
 * Local header files
 */
#include "utility.h"

/*
 * Local definitions
 */
#define GPIO_MEM_SIZE                  (0x08)
#define GPIO_BASE_ADDR                 (0x10000003)

#define GPIO_SERVO_PIN                 (1 << 0)

#define __DEFALT_DUTY_CYCLE            (500)

/*
 * Timer pulse
 */
typedef union
{
   struct _pulse   pulse;
}
timer_pulse_t;

/*
 * External variables
 */
extern float vol;
extern float min_rate;
extern float flow_rate;
extern uint32_t operation;
extern uint32_t op_percentage;
extern pthread_mutex_t operation_mutex;

/*
 * Global scope variables
 */
bool_t infusion = TRUE;

/*
 * Local scope variables
 */
#if defined (__ARM__)
static sint32_t gpio_base;
#endif   // #if defined (__ARM__)

static sint32_t channel_id;
static float local_min_rate;
static uint32_t start_vol = 0;
static uint32_t local_vol = 0;
static struct sigaction action;
static timer_pulse_t timer_msg;
static float local_flowrate = 0;
static uint32_t state = __OP_IDLE;
static uint32_t num_steps, steps_high, steps_low;

/*
 * Function     - __pwm_signal_handler()
 *
 * Arguments    - <signal> Signal received
 *
 * Return Value - None
 */
static void __pwm_signal_handler(sint32_t signal)
{
   if(__SIG_SETUP_OP == signal)
   {
      if(infusion && (flow_rate != 0) && (__OP_IDLE == state))
      {
         start_vol = vol;
         local_vol = vol;
         local_min_rate = min_rate;
         local_flowrate = flow_rate;
         num_steps = (uint32_t)(local_flowrate / local_min_rate);
         steps_high = num_steps + 500;
         steps_low = __MAX_NUM_STEPS - steps_high;
         state = __OP_START_INFUSION;
      }
      else if(!infusion && (flow_rate != 0) && (__OP_IDLE == state))
      {
         start_vol = vol;
         local_vol = vol;
         local_min_rate = min_rate;
         local_flowrate = flow_rate;
         num_steps = (uint32_t)(local_flowrate / local_min_rate);
         steps_high = num_steps;
         steps_low = __MAX_NUM_STEPS - steps_high;
         state = __OP_START_FILLING;
      }
      else
      {
         state = __OP_IDLE;
      }
   }
}

/*
 * Function     - __pwm_setup()
 *
 * Arguments    - None
 *
 * Return Value - None
 */
static __inline void __pwm_setup(void)
{
   timer_t  timer_id;
   struct sigevent event;
   struct itimerspec interval_time;

   // Set-up signal handler for handling __SIG_SETUP_OP
   action.sa_handler = __pwm_signal_handler;
   action.sa_flags = SA_SIGINFO;
   sigaction(__SIG_SETUP_OP, &action, NULL);

#if defined (__ARM__)
   // Map GPIO address into virtual memory
   gpio_base = (sint32_t)mmap_device_memory(0,
                                            GPIO_MEM_SIZE,
                                            (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                                            0,
                                            (uint32_t)GPIO_BASE_ADDR);

   if(gpio_base == MAP_DEVICE_FAILED)
   {
      perror("Mapping GPIO base address\n");
      exit(0);
   }
#endif   // #if defined (__ARM__)

   // Create channel for timer pulses
   channel_id = ChannelCreate(0);

   // Setup timer pulse event
   event.sigev_notify = SIGEV_PULSE;
   event.sigev_priority = getprio(0);
   event.sigev_code = __TIMER_PULSE_CODE;
   event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, channel_id, _NTO_SIDE_CHANNEL, 0);

   // Create the timer
   timer_create(CLOCK_REALTIME, &event, &timer_id);

   // Set timer interval
   interval_time.it_value.tv_sec = 0;
   interval_time.it_value.tv_nsec = __TIMER_TICK_NS;
   interval_time.it_interval.tv_sec = 0;
   interval_time.it_interval.tv_nsec = __TIMER_TICK_NS;

   // Timer will start after this call
   timer_settime(timer_id, 0, &interval_time, NULL);
}

/*
 * Function     - operation_thread()
 *
 * Arguments    - <args> Pointer to this thread's arguments
 *
 * Return Value - Pointer to the return value
 */
void * operation_thread(void * args)
{
   uint8_t port_val = 0;
   uint32_t total_time = 0;
   uint32_t count = 0, sec = 0;
   sint32_t rcv_id;

#if defined (__ARM__)
   sint32_t temp_high = 0, temp_low = 0;
#endif   // #if defined (__ARM__)

   // Setup the timer for PWM
   __pwm_setup();

   // Wait for timer pulse
   for (;;)
   {
      rcv_id = MsgReceive(channel_id, &timer_msg, sizeof(timer_msg), NULL);
      if(rcv_id == 0)
      {
         if(timer_msg.pulse.code == __TIMER_PULSE_CODE)
         {
            if(__OP_IDLE == state)
            {
               sec = 0;
               count = 0;
               total_time = 0;
            }
            else
            {
               if(0 == local_vol)
               {
                  state = __OP_IDLE;

                  if(0 == pthread_mutex_lock(&operation_mutex))
                  {
                     operation = __OP_COMPLETE;
                     pthread_mutex_unlock(&operation_mutex);
                  }
               }

               if(1000 == ++count)
               {
                  count = 0;
                  sec++;
               }

               if(60 == sec)
               {
                  sec = 0;
                  total_time++;

                  if(local_vol >= local_flowrate)
                     local_vol -= local_flowrate;
                  else
                     local_vol = 0;

                  if(0 == pthread_mutex_lock(&operation_mutex))
                  {
                     op_percentage = 100 - ((local_vol * 100) / start_vol);
                     pthread_mutex_unlock(&operation_mutex);
                  }
               }
            }

#if defined (__ARM__)
            // PWM pulse
            if(0 < --temp_high)
            {
               port_val = in8(gpio_base + 0);
               port_val |= GPIO_SERVO_PIN;
               out8(gpio_base + 0, port_val);
            }
            else if(0 < --temp_low)
            {
               port_val = in8(gpio_base + 0);
               port_val &= (uint8_t)(~(GPIO_SERVO_PIN));
               out8(gpio_base + 0, port_val);
            }
            else if(__OP_IDLE == state)
            {
               temp_high = __DEFALT_DUTY_CYCLE;
               temp_low = __DEFALT_DUTY_CYCLE;
            }
            else
            {
               temp_high = steps_high;
               temp_low = steps_low;
            }
#endif   // #if defined (__ARM__)
         }
      }
   }

   return ((void *)NULL);
}
