#ifndef _ESPFC_MODEL_H_
#define _ESPFC_MODEL_H_

#include "helper_3dmath.h"
#include "Pid.h"
#include "Kalman.h"

namespace Espfc {

enum GyroRate {
  GYRO_RATE_500 = 0x00,
  GYRO_RATE_333 = 0x01,
  GYRO_RATE_250 = 0x02,
  GYRO_RATE_200 = 0x03,
  GYRO_RATE_166 = 0x04,
  GYRO_RATE_100 = 0x05,
  GYRO_RATE_50  = 0x06
};

enum GyroDlpf {
  GYRO_DLPF_256 = 0x00,
  GYRO_DLPF_188 = 0x01,
  GYRO_DLPF_98  = 0x02,
  GYRO_DLPF_42  = 0x03,
  GYRO_DLPF_20  = 0x04,
  GYRO_DLPF_10  = 0x05,
  GYRO_DLPF_5   = 0x06
};

enum GyroFsr {
  GYRO_FS_250  = 0x00,
  GYRO_FS_500  = 0x01,
  GYRO_FS_1000 = 0x02,
  GYRO_FS_2000 = 0x03
};

enum AccelFsr {
  ACCEL_FS_2  = 0x00,
  ACCEL_FS_4  = 0x01,
  ACCEL_FS_8  = 0x02,
  ACCEL_FS_16 = 0x03
};

enum MagGain {
  MAG_GAIN_1370 = 0x00,
  MAG_GAIN_1090 = 0x01,
  MAG_GAIN_820  = 0x02,
  MAG_GAIN_660  = 0x03,
  MAG_GAIN_440  = 0x04,
  MAG_GAIN_390  = 0x05,
  MAG_GAIN_330  = 0x06,
  MAG_GAIN_220  = 0x07,
};

enum MagRate {
  MAG_RATE_3    = 0x00,
  MAG_RATE_7P5  = 0x01,
  MAG_RATE_15   = 0x02,
  MAG_RATE_30   = 0x03,
  MAG_RATE_75   = 0x04,
};

enum MagAvg {
  MAG_AVERAGING_1 = 0x00,
  MAG_AVERAGING_2 = 0x01,
  MAG_AVERAGING_4 = 0x02,
  MAG_AVERAGING_8 = 0x03
};

enum FlightMode {
  MODE_DISARMED = 0x01,
  MODE_RATE = 0x02,
  MODE_ANGLE = 0x03,
};

enum ModelFrame {
  FRAME_QUAD_X = 0x01
};
const size_t OUTPUT_CHANNELS = 4;
const size_t INPUT_CHANNELS = 8;

const size_t AXES = 4;
const size_t AXIS_ROLL   = 0;  // x
const size_t AXIS_PITH   = 1;  // y
const size_t AXIS_YAW    = 2;  // z
const size_t AXIS_THRUST = 3;  // throttle channel index

// working data
struct ModelState
{
  unsigned long timestamp;

  VectorInt16 gyroRaw;
  VectorInt16 accelRaw;
  VectorInt16 magRaw;

  VectorFloat gyro;
  VectorFloat gyroPose;
  VectorFloat accel;
  VectorFloat accelPose;
  VectorFloat mag;
  VectorFloat magPose;
  VectorFloat pose;
  Quaternion poseQ;

  VectorFloat rate;
  VectorFloat angle;
  Quaternion angleQ;

  long flightMode;

  float altitude;
  float altitudeVelocity;

  PidState innerPid[AXES];
  PidState outerPid[AXES];

  long inputUs[INPUT_CHANNELS];
  float input[INPUT_CHANNELS];

  float rateDesired[AXES];
  float angleDesired[AXES];

  float rateMax[AXES];
  float angleMax[AXES];

  float output[OUTPUT_CHANNELS];
  long outputUs[OUTPUT_CHANNELS];

  // other state
  Kalman kalman[AXES];
  VectorFloat accelPrev;

  float accelScale;
  float gyroScale;
  VectorFloat gyroBias;
  float gyroBiasAlpha;
  float gyroBiasSamples;
  long gyroBiasValid;

  long gyroSampleRate;
  long gyroSampleInterval;
  float gyroSampleIntervalFloat;

  long magSampleInterval;
  long magSampleRate;
  float magScale;

  unsigned long magTimestamp;
  unsigned long controllerTimestamp;
  unsigned long telemetryTimestamp;
};

// persistent data
struct ModelConfig
{
  long ppmPin;
  long gyroFifo;
  long gyroDlpf;
  long gyroFsr;
  long accelFsr;
  long gyroSampleRate;

  long magSampleRate;
  long magFsr;
  long magAvr;

  float gyroFilterAlpha;
  float accelFilterAlpha;
  float magFilterAlpha;

  long modelFrame;
  long flightModeChannel;

  long inputMin[INPUT_CHANNELS];
  long inputNeutral[INPUT_CHANNELS];
  long inputMax[INPUT_CHANNELS];
  long inputMap[INPUT_CHANNELS];

  long outputMin[OUTPUT_CHANNELS];
  long outputNeutral[OUTPUT_CHANNELS];
  long outputMax[OUTPUT_CHANNELS];

  long outputPin[OUTPUT_CHANNELS];

  Pid innerPid[AXES];
  Pid outerPid[AXES];
  float rateMax[AXES];
  float angleMax[AXES];

  long telemetry;
  long telemetryInterval;
};

class Model
{
  public:
    Model() {
      config.ppmPin = D4; // GPIO2
      config.gyroFifo = true;
      config.gyroDlpf = GYRO_DLPF_188;
      config.gyroFsr  = GYRO_FS_2000;
      config.accelFsr = ACCEL_FS_8;
      config.gyroSampleRate = GYRO_RATE_200;
      config.magSampleRate = MAG_RATE_75;
      config.magAvr = MAG_AVERAGING_1;

      config.accelFilterAlpha = 0.99f;
      config.gyroFilterAlpha = 0.5f;
      config.magFilterAlpha = 0.5f;

      config.telemetry = true;
      config.telemetryInterval = 50;

      for(size_t i = 0; i < OUTPUT_CHANNELS; i++)
      {
        config.outputMin[i] = 1000;
        config.outputNeutral[i] = 1050;
        config.outputMax[i] = 2000;
      }
      config.outputPin[0] = D5;
      config.outputPin[1] = D6;
      config.outputPin[2] = D7;
      config.outputPin[3] = D8;

      config.inputMap[0] = 0; // roll
      config.inputMap[1] = 1; // pitch
      config.inputMap[2] = 3; // yaw
      config.inputMap[3] = 2; // throttle
      config.inputMap[4] = 4; // flight mode
      config.inputMap[5] = 5; // free
      config.inputMap[6] = 6; // free
      config.inputMap[7] = 7; // free
      config.flightModeChannel = 4;

      for(size_t i = 0; i < INPUT_CHANNELS; i++)
      {
        config.inputMin[i] = 1000;
        config.inputNeutral[i] = 1500;
        config.inputMax[i] = 2000;
      }
      config.inputNeutral[AXIS_THRUST] = 1050; // override for thrust

      for(size_t i = 0; i < AXES; i++)
      {
        state.kalman[i] = Kalman();
        state.outerPid[i] = PidState();
        state.innerPid[i] = PidState();
        config.outerPid[i] = Pid(1, 0, 0, 0.3);
        config.innerPid[i] = Pid(1, 0, 0, 0.3);
      }

      config.rateMax[AXIS_PITH] = config.rateMax[AXIS_ROLL] = 200; // deg/s
      config.angleMax[AXIS_PITH] = config.angleMax[AXIS_ROLL] = 35; // deg
      config.rateMax[AXIS_YAW] = 200; // deg/s
    }
    union {
      ModelState state;
      char * stateAddr;
    };
    union {
      ModelConfig config;
      char * configAddr;
    };
};

}

#endif