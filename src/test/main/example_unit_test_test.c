#include <stdio.h>
#include <string.h>
#include "unity.h"

static void print_banner(const char* text);

void app_main()
{
    /* These are the different ways of running registered tests.
     * In practice, only one of them is usually needed.
     *
     * UNITY_BEGIN() and UNITY_END() calls tell Unity to print a summary
     * (number of tests executed/failed/ignored) of tests executed between these calls.
     */
    print_banner("Executing tests by name");
    UNITY_BEGIN();
    unity_run_test_by_name("mqtt_init");
    unity_run_test_by_name("mqtt_sub");
    unity_run_test_by_name("mqtt_send");
    unity_run_test_by_name("config_reset");
    UNITY_END();

    /* This function will not return, and will be busy waiting for UART input.
     * Make sure that task watchdog is disabled if you use this function.
     */
    //unity_run_menu();
}

static void print_banner(const char* text)
{
    printf("\n#### %s #####\n\n", text);
}

