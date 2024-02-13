///*
// * crsf.c
// *
// *  Created on: Jun 27, 2023
// *      Author: alkaz
// */
// crc code from https://github.com/stepinside/Arduino-CRSF/tree/main
//
#include "mtcu.h"
#include "targets.h"
#include "common.h"
#include "functions.h"
#include "signal.h"

uint8_t mtcu_buffer[100] = {0};

//uint32_t invalid_crc = 0;
#define AddressA 131          // Device has two possible addresses, set by dipswitch
#define AddressB 132          // These are within the range of addresses also used by Dimension Engineering Sabertooth devices (128-135)
uint8_t MyAddress = AddressA; // Address of device. Set by dipswitch.



void setChannelsMTCU()
{
  uint8_t startByte = 0;
  uint8_t tempByte = 0;
    uint8_t validData = 0;
  static char input_line[4] = {0};
   

  for(int i = 0; i< 100;i++)
        {
            if(mtcu_buffer[i] == MyAddress)
            {
              startByte = i;
              break;
            }
        }
  for(int i = 0; i< 4;i++)
    {
      tempByte = startByte + i; 
        if(tempByte > 99)
      {
        tempByte = tempByte - 100;
      }
      input_line[i] = mtcu_buffer[tempByte];
    }
  
    if (input_line[0] == MyAddress) // for some reason mtcu_buffer[2] is the first byte in the buffer
    {
      //playBrushedStartupTune();
      //playDuskingTune();
     
        uint8_t check = (input_line[0] + input_line[1] + input_line[2]) & 0x7F;
        if (input_line[3] == check)
        {
          //playDuskingTune();
          //playBrushedStartupTune();
          //playInputTune();
          newinput = 1000;
            validData = 1;
        }
        /*for(int i = 0; i<= SENTENCE_BYTES;i++)

        {
            input_line[i] = mtcu_buffer[i];
        }*/
    }
    
    //  Address bytes have values greater than 127
    if (validData == 1)
    {
        switch (input_line[1])
        {
        case 0:
            // Motor 1 Forward
            newinput = map(input_line[2], 0, 127, 1000, 2000);
            // setSpeed(1, getSpeedCommand_fromSerial(mtcu_buffer[2]));
            break;

        case 1:
            // Motor 1 Reverse
            //playInputTune();
            newinput = map(input_line[2], 0, 127, 1000, 0);
            // setSpeed(1, -getSpeedCommand_fromSerial(mtcu_buffer[2]));
            break;

        case 2:
            // Set minimum voltage
            // Used to set a custom minimum voltage for the battery supplying power (essentially LVC level). If battery voltage drops below this value,
            // motors will be turned off. This value is cleared at startup, so must be set each run. The value is sent in .2 volt
            // increments with a command of zero corresponding to 6v, which is the minimum. Valid data is from 0 to 50 (6-16 volts).
            // The function for converting volts to command data is Value = (desired volts-6) x 5

            // If valid value sent, update MinVoltage
            if (input_line[2] <= 50)
            {
                // MinVoltage = 6.0 + ((float)mtcu_buffer[2] * 0.2);
            }
            break;

        case 3:
            // If voltage exceeds the value set here, the motors will be turned off, however the valute set here can also not exceed AbsMaxVoltage which is
            // determined by the hardware on the board. MaxVoltage is cleared at startup (and set to default of AbsMaxVoltage) so must be re-set by serial
            // at each boot. The value is sent in 0.2 volt increments with valid values being:
            // 30 to 140 (6 volts to 28 volts)
            // However again this is subject to AbsMaxVoltage which for all curent board revisios is actually capped at 16 volts.
            // The function for converting volts to command data is Value = (desired volts / 0.2)

            // If valid value is sent, update MaxVoltage
            /*val = mtcu_buffer[2];
              if (val >= 30 && val <= 140)        // Allowable range
              {
                if (val <= (AbsMaxVoltage * 5)) // But constrain also to our board-specific AbsMaxVoltage
                {
                    MaxVoltage = ((float)val * 0.2);
                }
              }*/
            break;

        case 4:
            // Motor 2 Forward
            // setSpeed(2, getSpeedCommand_fromSerial(mtcu_buffer[2]));
            break;

        case 5:
            // Motor 2 Reverse
            // setSpeed(2, -getSpeedCommand_fromSerial(mtcu_buffer[2]));
            break;

            // cases  6-13 reserved for future compatibility with the equivalent Sabertooth commands

        case 14:
            // Serial1 Watchdog (disabled by default on each boot)
            // Values greater than 0 will enabled the watchdog. The value specifies what length of time the controller will wait for a new serial command, after which if it does not receive one it will
            // stop the motors as a safety precaution. This can help guard against for example the communication cable becoming disconnected.
            // The the value passed is 0 it will disable the watchdog, however the feature is disabled by default on each restart so you don't need to do anything if you don't want it.
            // Note also the serial watchdog has no effect when the Scout is running in RC mode.

            if (input_line[2] == 0)
            {
                // SerialWatchdog = false;
            }
            else
            {
                // The length of time for the watchdog to wait is set in 100mS increments, so for example a value of 10 would equate to a watchdog timeout of 1000mS (1 second).
                // Valid data is a number from 0 to 255 which corresponds to watchdog timeouts of 100ms (1/10 second) to 25500mS (25.5 seconds)
                // The function for converting watchdog time to command data is Value = desired time in mS / 100
                // SerialWatchdogTimeout_mS = mtcu_buffer[2] * 100;
                // SerialWatchdog = true;
            }
            break;

        case 15:
            // Change baud rate. If valid value passed, re-start the hardware serial port at the selected baud rate
            // Serial1.println(6);
            /*Serial1.end();
            delay(1);
            switch (mtcu_buffer[2])
            {
            case BAUD_CODE_2400:
              Serial1.begin(2400);
              break;
            case BAUD_CODE_9600:
              Serial1.begin(9600);
              break;
            case BAUD_CODE_19200:
              Serial1.begin(19200);
              break;
            case BAUD_CODE_38400:
              Serial1.begin(38400);
              break;
            case BAUD_CODE_115200:
              Serial1.begin(115200);
              break;
            case BAUD_CODE_57600:
              Serial1.begin(57600);
              break;
            }*/
            break;

        // cases 16-17 reserved for future compatibility with Sabertooth commands (ramping and deadband)
        // case  19 presently un-assigned
        case 19:
            // Throttle position
            if (input_line[2])
            {
                // Throttle = mtcu_buffer[2];
            }
            break;
        case 20:
            // Direct fan control
            // Set fan speed to value 0-255
            /*setFanSpeed(mtcu_buffer[2]);
              // If user sets a fan speed, we switch to ManualFanControl. They can revert to automatic control by issuing command 21
              ManualFanControl = true;*/
            break;

        case 21:
            // Set fan control to "Automatic"
            // Fan control is automatic by default, so this command doesn't need to be issued unless the user initiated manual fan control (command 20) and now wants to revert to automatic.
            // Value is ignored.
            // ManualFanControl = false;
            break;

        case 22:
            // Set maximum current
            // Used to set a maximum current PER MOTOR (not total device current). If current on either motor exceeds the maximum level, BOTH motors will be stopped.
            // Value is in amps and can be any value from 1 to 30. Default is 12 amps, if the user chooses to set a higher level it is highly recommended to use a cooling fan.
            // One may wonder why we don't permit milliamp-level adjustment the way we do with voltage. The reason is that current sensing on this device is crude and
            // setting a precision current limit isn't possible anyway.

            // If valid value sent, update MaxWatt

#if defined REV1_3 || defined REV1_2
#if defined HIGH_CURRENT
            if (input_line[2] > 0 && input_line[2] <= 60)
            {
                MaxCurrent = input_line[2];
            }
            else
            {
                MaxCurrent = 60;
            }
#elif defined LOW_CURRENT
            if (input_line[2] > 0 && input_line[2] <= 6)
            {
                MaxCurrent = input_line[2];
            }
            else
            {
                MaxCurrent = 6;
            }
#else
            if (input_line[2] > 0 && input_line[2] <= 20)
            {
                MaxCurrent = input_line[2];
            }
            else
            {
                MaxCurrent = 20;
            }
#endif
#elif defined REV1_1
            if (input_line[2] > 0 && input_line[2] <= 30)
            {
                MaxCurrent = input_line[2];
            }
            else
            {
                MaxCurrent = 30;
            }
#endif

            break;

        case 23:
            // Brake motors at stop
            // The default method of stopping the motors is simply to quit powering them. However this does nothing to keep them from free-wheeling in response to an external
            // force. If we actually want them to remain stopped, we need to "brake" them. This is done by shorting both motor leads together which creates resistance inside
            // the motor that can help keep it stationary. The motor of course can still be turned as the brake is not very strong. But it can make a difference in some applications.
            // For tracked vehicles we will typically want this enabled since turning at slow speed is accomplished by keeping one track stationary.
            // The BrakeAtStop setting defaults to false at bootup but can be set by the user via this serial command. Pass a value of 1 (true) in the data byte to enable, or 0 (false) to disable.
            /*if (mtcu_buffer[2])
            {
              BrakeAtStop = true;
              // If a motor is already stopped go ahead and apply the brakes now
              if (M1_Speed == 0)
                BrakeMotor1();
              if (M2_Speed == 0)
                BrakeMotor2();
            }
            else
            {
              BrakeAtStop = false;
            }*/
            break;

        case 24:
            // Drag Inner Track
            // This function was created to compensate for certain gearboxes such as the Taigen V2 Steel 3:1 and 4:1 gearboxes that have inadequate
            // internal friction to prevent freewheeling with any sort of external force. When used in a tank or similar vehicle that requires
            // differential motor speed in order to turn, the model may become difficult or impossible to steer.
            //
            // The effect is most pronounced on heavy wide-tracked models with metal upgrades, such as the King Tiger, Jagdtiger, Panther, Jagdpanther, etc....
            // In these cases reducing voltage to the inner track to steer does nothing, the outer (faster) track has enough traction with the ground to
            // keep driving the model straight forward and the inner track just freewheels to keep up, rather than "dragging" the model into a turn as it should.
            //
            // The DragInnerTrack setting defaults to false at bootup but can be set by the user via this serial command. Pass a value of 1 (true) in the data byte
            // to enable, or 0 (false) to disable. When enabled, the Scout will determine which motor is supposed to be turning slower than the other, and attempt
            // to prevent it from freewheeling beyond the desired speed by "dragging" it with brief, pulsed brake commands.
            // See the DragMotor() function on the MOTOR tab of this sketch for further comments.
            /*if (mtcu_buffer[2])
              DragInnerTrack = true;
            else
              DragInnerTrack = false;*/
            break;
        case 25:
            // Max speed forward set by user in op config
            if (input_line[2])
            {
                // MaxSpeedForward = mtcu_buffer[2];
            }
            break;
        case 26:
            // Max speed reverse set by user in op config
            if (input_line[2])
            {
                // MaxSpeedReverse = mtcu_buffer[2];
            }
            break;
        case 27:
            // Torque modifier for the current selected gear 0-255 = 0 - 100%
            if (input_line[2])
            {
                // GearTorque = mtcu_buffer[2];
            }
            break;
        case 46:
            // Throttle position
            if (input_line[2])
            {
                // Throttle = mtcu_buffer[2];
#ifdef uart_debug
                Serial.print(" t ");
                Serial.println(Throttle);
#endif
            }
            break;
        case 75:
            // Vehicle speed scaled by max speed values
            if (input_line[2])
            {
                // vehicleSpeed = mtcu_buffer[2];
#ifdef uart_debug
                Serial.print(" vs ");
                Serial.println(vehicleSpeed);
#endif
            }
            break;
        default:
            break;
        }
        signaltimeout = 0;
        if ((adjusted_input == 0) && !armed)
        {
            zero_input_count++;
        }
        else
        {
            zero_input_count = 0;
        }
    }
    else
    {
        //invalid_crc++;
    }
}
