#ifndef _ESPFC_SENSOR_ACCEL_SENSOR_H_
#define _ESPFC_SENSOR_ACCEL_SENSOR_H_

#include "BaseSensor.h"
#include "Device/GyroDevice.h"

namespace Espfc {

namespace Sensor {

class AccelSensor: public BaseSensor
{
  public:
    AccelSensor(Model& model): _model(model) {}
    
    int begin()
    {
      _model.state.accel.z = ACCEL_G;

      _gyro = Hardware::getGyroDevice(_model);
      if(!_gyro) return 0;

      switch(_model.config.accelFsr)
      {
        case ACCEL_FS_16: _model.state.accelScale = 16.f * ACCEL_G / 32768.f; break;
        case ACCEL_FS_8:  _model.state.accelScale =  8.f * ACCEL_G / 32768.f; break;
        case ACCEL_FS_4:  _model.state.accelScale =  4.f * ACCEL_G / 32768.f; break;
        case ACCEL_FS_2:  _model.state.accelScale =  2.f * ACCEL_G / 32768.f; break;
      }
      _gyro->setFullScaleAccelRange(_model.config.accelFsr);
      _model.logger.info().log(F("ACCEL INIT")).log(FPSTR(Device::GyroDevice::getName(_gyro->getType()))).log(_model.state.accelTimer.rate).log(_model.state.accelTimer.interval).logln(_model.state.accelPresent);

      for(size_t i = 0; i < 3; i++)
      {
        _filter[i].begin(FilterConfig(FILTER_PT1, 0), _model.state.accelTimer.rate);
      }

      return 1;
    }

    int update()
    {
      if(!_model.accelActive()) return 0;

      if(!_model.state.accelTimer.check()) return 0;

      {
        Stats::Measure measure(_model.state.stats, COUNTER_ACCEL_READ);
        _gyro->readAccel(_model.state.accelRaw);
      }

      {
        Stats::Measure measure(_model.state.stats, COUNTER_ACCEL_FILTER);

        align(_model.state.accelRaw, _model.config.accelAlign);

        _model.state.accel = (VectorFloat)_model.state.accelRaw * _model.state.accelScale;
        for(size_t i = 0; i < 3; i++)
        {
          _model.state.accel.set(i, _filter[i].update(_model.state.accel[i]));
          _model.state.accel.set(i, _model.state.accelFilter[i].update(_model.state.accel[i]));
        }

        if(_model.state.accelBiasSamples > 0)
        {
          //_model.state.accelBias = (_model.state.accelBias * (1.0f - _model.state.accelBiasAlpha)) + (_model.state.accel * _model.state.accelBiasAlpha);
          _model.state.accelBias += (_model.state.accel - _model.state.accelBias) * _model.state.accelBiasAlpha;
          _model.state.accelBiasSamples--;
          if(_model.state.accelBiasSamples == 0)
          {
            _model.state.accelBias.z -= ACCEL_G;
            _model.logger.info().log(F("ACCEL CAL")).log(_model.state.accelBias.x).log(_model.state.accelBias.y).logln(_model.state.accelBias.z);
          }
        }
        else
        {
          _model.state.accel -= _model.state.accelBias;
        }
      }

      return 1;
    }

  private:
    Model& _model;
    Device::GyroDevice * _gyro;
    Filter _filter[3];
};

}

}
#endif