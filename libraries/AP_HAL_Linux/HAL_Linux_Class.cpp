#include "HAL_Linux_Class.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/utility/RCOutput_Tap.h>
#include <AP_HAL/utility/getopt_cpp.h>
#include <AP_HAL_Empty/AP_HAL_Empty.h>
#include <AP_HAL_Empty/AP_HAL_Empty_Private.h>
#include <AP_Module/AP_Module.h>

#include "AnalogIn_ADS1115.h"
#include "AnalogIn_IIO.h"
#include "AnalogIn_Navio2.h"
#include "GPIO.h"
#include "I2CDevice.h"
#include "OpticalFlow_Onboard.h"
#include "RCInput.h"
#include "RCInput_AioPRU.h"
#include "RCInput_Navio2.h"
#include "RCInput_PRU.h"
#include "RCInput_RPI.h"
#include "RCInput_SoloLink.h"
#include "RCInput_UART.h"
#include "RCInput_UDP.h"
#include "RCInput_Multi.h"
#include "RCInput_ZYNQ.h"
#include "RCInput_RCProtocol.h"
#include "RCOutput_AioPRU.h"
#include "RCOutput_Bebop.h"
#include "RCOutput_Disco.h"
#include "RCOutput_PCA9685.h"
#include "RCOutput_PRU.h"
#include "RCOutput_Sysfs.h"
#include "RCOutput_ZYNQ.h"
#include "SPIDevice.h"
#include "SPIUARTDriver.h"
#include "Scheduler.h"
#include "Storage.h"
#include "UARTDriver.h"
#include "Util.h"
#include "Util_RPI.h"

using namespace Linux;


static Util utilInstance;


// 5 serial ports on Linux
static UARTDriver uartADriver(true);
static UARTDriver uartCDriver(false);
static UARTDriver uartDDriver(false);
static UARTDriver uartEDriver(false);
static UARTDriver uartFDriver(false);
static UARTDriver uartGDriver(false);
static UARTDriver uartHDriver(false);

static I2CDeviceManager i2c_mgr_instance;
static SPIDeviceManager spi_mgr_instance;


static UARTDriver uartBDriver(false);



static Empty::AnalogIn analogIn;


static Storage storageDriver;

/*
  use the BBB gpio driver on ERLE, PXF, BBBMINI, BLUE and PocketPilot
 */

static Empty::GPIO gpioDriver;


/*
  use the PRU based RCInput driver on ERLE and PXF
 */

static RCInput rcinDriver;


/*
  use the PRU based RCOutput driver on ERLE and PXF
 */

static Empty::RCOutput rcoutDriver;


static Scheduler schedulerInstance;


static Empty::OpticalFlow opticalFlow;


static Empty::Flash flashDriver;

HAL_Linux::HAL_Linux() :
    AP_HAL::HAL(
        &uartADriver,
        &uartBDriver,
        &uartCDriver,
        &uartDDriver,
        &uartEDriver,
        &uartFDriver,
        &uartGDriver,
        &uartHDriver,
        &i2c_mgr_instance,
        &spi_mgr_instance,
        &analogIn,
        &storageDriver,
        &uartADriver,
        &gpioDriver,
        &rcinDriver,
        &rcoutDriver,
        &schedulerInstance,
        &utilInstance,
        &opticalFlow,
        &flashDriver,
        nullptr)
{}

void _usage(void)
{
    printf("Usage: -A uartAPath -B uartBPath -C uartCPath -D uartDPath -E uartEPath -F uartFPath -G uartGpath -H uartHpath\n");
    printf("Options:\n");
    printf("\tserial:\n");
    printf("                    -A /dev/ttyO4\n");
    printf("\t                  -B /dev/ttyS1\n");
    printf("\tnetworking tcp:\n");
    printf("\t                  -C tcp:192.168.2.15:1243:wait\n");
    printf("\t                  -A tcp:11.0.0.2:5678\n");
    printf("\t                  -A udp:11.0.0.2:14550\n");
    printf("\tnetworking UDP:\n");
    printf("\t                  -A udp:11.0.0.255:14550:bcast\n");
    printf("\t                  -A udpin:0.0.0.0:14550\n");
    printf("\tcustom log path:\n");
    printf("\t                  --log-directory /var/APM/logs\n");
    printf("\t                  -l /var/APM/logs\n");
    printf("\tcustom terrain path:\n");
    printf("\t                   --terrain-directory /var/APM/terrain\n");
    printf("\t                   -t /var/APM/terrain\n");
#if AP_MODULE_SUPPORTED
    printf("\tmodule support:\n");
    printf("\t                   --module-directory %s\n", AP_MODULE_DEFAULT_DIRECTORY);
    printf("\t                   -M %s\n", AP_MODULE_DEFAULT_DIRECTORY);
#endif
}

void HAL_Linux::run(int argc, char* const argv[], Callbacks* callbacks) const
{
#if AP_MODULE_SUPPORTED
    const char *module_path = AP_MODULE_DEFAULT_DIRECTORY;
#endif
    
    assert(callbacks);

    int opt;
    const struct GetOptLong::option options[] = {
        {"uartA",         true,  0, 'A'},
        {"uartB",         true,  0, 'B'},
        {"uartC",         true,  0, 'C'},
        {"uartD",         true,  0, 'D'},
        {"uartE",         true,  0, 'E'},
        {"uartF",         true,  0, 'F'},
        {"uartG",         true,  0, 'G'},
        {"uartH",         true,  0, 'H'},
        {"log-directory",       true,  0, 'l'},
        {"terrain-directory",   true,  0, 't'},
        {"storage-directory",   true,  0, 's'},
        {"module-directory",    true,  0, 'M'},
        {"help",                false,  0, 'h'},
        {0, false, 0, 0}
    };

    GetOptLong gopt(argc, argv, "A:B:C:D:E:F:l:t:s:he:SM:",
                    options);

    /*
      parse command line options
     */
    while ((opt = gopt.getoption()) != -1) {
        switch (opt) {
        case 'A':
            uartADriver.set_device_path(gopt.optarg);
            break;
        case 'B':
            uartBDriver.set_device_path(gopt.optarg);
            break;
        case 'C':
            uartCDriver.set_device_path(gopt.optarg);
            break;
        case 'D':
            uartDDriver.set_device_path(gopt.optarg);
            break;
        case 'E':
            uartEDriver.set_device_path(gopt.optarg);
            break;
        case 'F':
            uartFDriver.set_device_path(gopt.optarg);
            break;
        case 'G':
            uartGDriver.set_device_path(gopt.optarg);
            break;
        case 'H':
            uartHDriver.set_device_path(gopt.optarg);
            break;
        case 'l':
            utilInstance.set_custom_log_directory(gopt.optarg);
            break;
        case 't':
            utilInstance.set_custom_terrain_directory(gopt.optarg);
            break;
        case 's':
            utilInstance.set_custom_storage_directory(gopt.optarg);
            break;
#if AP_MODULE_SUPPORTED
        case 'M':
            module_path = gopt.optarg;
            break;
#endif
        case 'h':
            _usage();
            exit(0);
        default:
            printf("Unknown option '%c'\n", (char)opt);
            exit(1);
        }
    }

    setup_signal_handlers();

    scheduler->init();
    gpio->init();
    rcout->init();
    rcin->init();
    uartA->begin(115200);
    uartE->begin(115200);
    uartF->begin(115200);
    uartG->begin(115200);
    uartH->begin(115200);
    analogin->init();
    utilInstance.init(argc+gopt.optind-1, &argv[gopt.optind-1]);

    // NOTE: See commit 9f5b4ffca ("AP_HAL_Linux_Class: Correct
    // deadlock, and infinite loop in setup()") for details about the
    // order of scheduler initialize and setup on Linux.
    scheduler->system_initialized();

    // possibly load external modules
#if AP_MODULE_SUPPORTED
    if (module_path != nullptr) {
        AP_Module::init(module_path);
    }
#endif

#if AP_MODULE_SUPPORTED
    AP_Module::call_hook_setup_start();
#endif
    callbacks->setup();
#if AP_MODULE_SUPPORTED
    AP_Module::call_hook_setup_complete();
#endif

    while (!_should_exit) {
        callbacks->loop();
    }

    rcin->teardown();
    I2CDeviceManager::from(i2c_mgr)->teardown();
    SPIDeviceManager::from(spi)->teardown();
    Scheduler::from(scheduler)->teardown();
}

void HAL_Linux::setup_signal_handlers() const
{
    struct sigaction sa = { };

    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = HAL_Linux::exit_signal_handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

static HAL_Linux halInstance;

void HAL_Linux::exit_signal_handler(int signum)
{
    halInstance._should_exit = true;
}

const AP_HAL::HAL &AP_HAL::get_HAL()
{
    return halInstance;
}
