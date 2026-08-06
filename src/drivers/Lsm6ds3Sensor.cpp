#include "drivers/Lsm6ds3Sensor.h"

#include "AlgoParams.hpp"

#include <LSM6DS3.h>
#include <Wire.h>

namespace
{
LSM6DS3 imu(I2C_MODE, AlgoParams::IMU_I2C_ADDRESS);
} // namespace

bool Lsm6ds3Sensor::begin()
{
    // Initialize the I2C bus before talking to the IMU.
    Wire.begin();
    Wire.setClock(AlgoParams::IMU_I2C_CLOCK_HZ);

    // Configure accelerometer settings for the expected motion range.
    imu.settings.accelEnabled = 1;
    imu.settings.accelRange = AlgoParams::IMU_ACCEL_RANGE_G;
    imu.settings.accelSampleRate = AlgoParams::IMU_ODR_HZ;
    imu.settings.accelBandWidth = 100;

    // Configure gyroscope settings for the expected rotation range.
    imu.settings.gyroEnabled = 1;
    imu.settings.gyroRange = AlgoParams::IMU_GYRO_RANGE_DPS;
    imu.settings.gyroSampleRate = AlgoParams::IMU_ODR_HZ;
    imu.settings.gyroBandWidth = 100;

    // Disable the temperature sensor because it is not used.
    imu.settings.tempEnabled = 0;

    // Attempt to initialize the IMU and record whether it succeeded.
    ready_ = (imu.begin() == IMU_SUCCESS);
    if (!ready_)
    {
        // If the IMU did not initialize, count an I2C error for diagnostics.
        ++i2cErrors_;
    }

    return ready_;
}

bool Lsm6ds3Sensor::read(MotionData& out)
{
    if (!ready_)
    {
        // Do not attempt reads if the sensor has not been initialized.
        ++i2cErrors_;
        return false;
    }

    // Read the current accelerometer and gyroscope values.
    out.accelX = imu.readFloatAccelX();
    out.accelY = imu.readFloatAccelY();
    out.accelZ = imu.readFloatAccelZ();
    out.gyroX = imu.readFloatGyroX();
    out.gyroY = imu.readFloatGyroY();
    out.gyroZ = imu.readFloatGyroZ();
    out.isStep = false;

    return true;
}

uint32_t Lsm6ds3Sensor::i2cErrorCount() const
{
    return i2cErrors_;
}
