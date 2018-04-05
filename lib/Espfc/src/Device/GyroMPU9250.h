#ifndef _ESPFC_GYRO_MPU9250_H_
#define _ESPFC_GYRO_MPU9250_H_

#include "BusDevice.h"
#include "GyroMPU6050.h"
#include "helper_3dmath.h"
#include "Debug.h"

namespace Espfc {

namespace Device {

class GyroMPU9250: public GyroMPU6050
{
  public:
    int begin(BusDevice * bus, uint8_t addr = -1) override
    {
      _bus = bus;
      _addr = addr != -1 ? addr : MPU6050_DEFAULT_ADDRESS;

      if(!testConnection()) return 0;

      //_bus->writeByte(_addr, MPU6050_RA_PWR_MGMT_1, 0x80); // reset
      //delay(1);

      setClockSource(MPU6050_CLOCK_PLL_XGYRO);
      setSleepEnabled(false);

      return 1;
    }

    GyroDeviceType getType() const override
    {
      return GYRO_MPU9250;
    }

    bool testConnection() override
    {
      uint8_t whoami = 0;
      _bus->readByte(_addr, MPU6050_RA_WHO_AM_I, &whoami);
      //D("mpu9250_whoami", _addr, whoami);
      return whoami == 0x71 || whoami == 0x73;
    }
};

}

}

#endif