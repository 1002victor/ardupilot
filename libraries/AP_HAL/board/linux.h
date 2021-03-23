#pragma once

#define HAL_BOARD_NAME "Linux"
#define HAL_CPU_CLASS HAL_CPU_CLASS_1000
#define HAL_MEM_CLASS HAL_MEM_CLASS_1000
#define HAL_OS_POSIX_IO 1
#define HAL_OS_SOCKETS 1
#define HAL_STORAGE_SIZE            16384
#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE

// make sensor selection clearer
#define PROBE_IMU_I2C(driver, bus, addr, args ...) ADD_BACKEND(AP_InertialSensor_ ## driver::probe(*this,GET_I2C_DEVICE(bus, addr),##args))
#define PROBE_IMU_SPI(driver, devname, args ...) ADD_BACKEND(AP_InertialSensor_ ## driver::probe(*this,hal.spi->get_device(devname),##args))
#define PROBE_IMU_SPI2(driver, devname1, devname2, args ...) ADD_BACKEND(AP_InertialSensor_ ## driver::probe(*this,hal.spi->get_device(devname1),hal.spi->get_device(devname2),##args))

#define PROBE_BARO_I2C(driver, bus, addr, args ...) ADD_BACKEND(AP_Baro_ ## driver::probe(*this,std::move(GET_I2C_DEVICE(bus, addr)),##args))
#define PROBE_BARO_SPI(driver, devname, args ...) ADD_BACKEND(AP_Baro_ ## driver::probe(*this,std::move(hal.spi->get_device(devname)),##args))

#define PROBE_MAG_I2C(driver, bus, addr, args ...) ADD_BACKEND(DRIVER_ ##driver, AP_Compass_ ## driver::probe(GET_I2C_DEVICE(bus, addr),##args))
#define PROBE_MAG_SPI(driver, devname, args ...) ADD_BACKEND(DRIVER_ ##driver, AP_Compass_ ## driver::probe(hal.spi->get_device(devname),##args))
#define PROBE_MAG_IMU(driver, imudev, imu_instance, args ...) ADD_BACKEND(DRIVER_ ##driver, AP_Compass_ ## driver::probe_ ## imudev(imu_instance,##args))
#define PROBE_MAG_IMU_I2C(driver, imudev, bus, addr, args ...) ADD_BACKEND(DRIVER_ ##driver, AP_Compass_ ## driver::probe_ ## imudev(GET_I2C_DEVICE(bus,addr),##args))

#if CONFIG_HAL_BOARD_SUBTYPE == HAL_BOARD_SUBTYPE_LINUX_NONE
    #define HAL_BOARD_LOG_DIRECTORY "logs"
    #define HAL_BOARD_TERRAIN_DIRECTORY "terrain"
    #define HAL_BOARD_STORAGE_DIRECTORY "."
    #define HAL_INS_DEFAULT HAL_INS_HIL
    #define HAL_BARO_DEFAULT HAL_BARO_HIL
    #define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL
#else
    #error "no Linux board subtype set"
#endif

#ifndef HAL_COMPASS_DEFAULT
    #define HAL_COMPASS_DEFAULT -1
#endif

#ifndef HAL_OPTFLOW_PX4FLOW_I2C_ADDRESS
    #define HAL_OPTFLOW_PX4FLOW_I2C_ADDRESS 0x42
#endif

#ifndef HAL_OPTFLOW_PX4FLOW_I2C_BUS
    #define HAL_OPTFLOW_PX4FLOW_I2C_BUS 1
#endif

#define HAL_HAVE_BOARD_VOLTAGE 1
#define HAL_HAVE_SAFETY_SWITCH 0


#ifndef HAL_HAVE_SERVO_VOLTAGE
    #define HAL_HAVE_SERVO_VOLTAGE 0
#endif

#ifndef AP_STATEDIR
    #define HAL_BOARD_STATE_DIRECTORY "/var/lib/ardupilot"
#else
    #define HAL_BOARD_STATE_DIRECTORY AP_STATEDIR
#endif

#ifndef HAL_BOARD_LOG_DIRECTORY
    #define HAL_BOARD_LOG_DIRECTORY HAL_BOARD_STATE_DIRECTORY "/logs"
#endif

#ifndef HAL_BOARD_TERRAIN_DIRECTORY
    #define HAL_BOARD_TERRAIN_DIRECTORY HAL_BOARD_STATE_DIRECTORY "/terrain"
#endif

#ifndef HAL_BOARD_STORAGE_DIRECTORY
    #define HAL_BOARD_STORAGE_DIRECTORY HAL_BOARD_STATE_DIRECTORY
#endif

#ifndef HAL_BOARD_CAN_IFACE_NAME
    #define HAL_BOARD_CAN_IFACE_NAME "can0"
#endif

// if bus masks are not setup above then use these defaults
#ifndef HAL_LINUX_I2C_BUS_MASK
    #define HAL_LINUX_I2C_BUS_MASK 0xFFFF
#endif

#ifndef HAL_LINUX_I2C_INTERNAL_BUS_MASK
    #define HAL_LINUX_I2C_INTERNAL_BUS_MASK 0xFFFF
#endif

#ifndef HAL_LINUX_I2C_EXTERNAL_BUS_MASK
    #define HAL_LINUX_I2C_EXTERNAL_BUS_MASK 0xFFFF
#endif

#include <AP_HAL_Linux/Semaphores.h>
#define HAL_Semaphore Linux::Semaphore
#define HAL_Semaphore_Recursive Linux::Semaphore

