#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

#include "sensors.h"
#include "dev/button-sensor.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#include "er-apps.h"

PROCESS(rest_server_er_app, "Erbium Application Server");
//AUTOSTART_PROCESSES(&rest_server_er_app);

PROCESS_THREAD(rest_server_er_app, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Starting Erbium Application Server\n");
  
  /* Initialize the REST engine. */
  rest_init_engine();
  
    /* Activate the application-specific resources. */
#if RS_HELLOWORLD
  rest_activate_resource(&resource_helloworld);
#endif

#if RS_LEDS
  rest_activate_resource(&resource_leds);
#endif
  
#if RS_BUTTON
  rest_activate_event_resource(&resource_button);
#endif
  
#if RS_VOLTAGE
#if VOLTAGE_PERIODIC
   rest_activate_periodic_resource(&periodic_resource_voltage);
#else
  rest_activate_event_resource(&resource_voltage);
#endif
#endif

#if RS_TANDH
#if TANDH_PERIODIC
   rest_activate_periodic_resource(&periodic_resource_DHT11);
#else
  rest_activate_event_resource(&resource_DHT11);
#endif
#endif
  
#if RS_UIPV6
  rest_activate_periodic_resource(&periodic_resource_uipv6); 
#endif
  
  rest_activate_event_resource(&resource_DI1);
  rest_activate_periodic_resource(&periodic_resource_AI1);
  rest_activate_resource(&resource_DO1);
  rest_activate_resource(&resource_AO1);
  
  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == sensors_event) {
#if RS_BUTTON
      if (data == &button_sensor) {
      /* Call the event_handler for this application-specific event. */
        button_event_handler(&resource_button);
      }
#endif
      
#if REST_RES_SEPARATE && WITH_COAP>3
      /* Also call the separate response example handler. */
      separate_finalize_handler();
#endif
    }

  } /* while (1) */

  PROCESS_END();
}
