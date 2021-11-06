#pragma config(Motor,  port2,           catapult,      tmotorServoStandard, openLoop)
#pragma config(Motor,  port3,           leftMotor,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           rightMotor,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           handLeft,      tmotorServoStandard, openLoop, driveRight)
#pragma config(Motor,  port6,           handRight,     tmotorServoStandard, openLoop, driveLeft)
#pragma config(Motor,  port7,           armMotor,      tmotorServoStandard, openLoop)
#pragma config(Motor,  port8,           handMotor,     tmotorServoStandard, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/********************************

Joystick mapping:
	Ch3(Left Joystick up/down) - Left Wheel
	Ch2(Right Joystick up/down) - Right Wheel
Button mapping:
	Btn7L - Changes the Mode
	Btn7U - Catapult Launch
	Btn7D - Catapult Reset
	Btn7R - Automation
	Btn8L - Rotate Hand Left
	Btn8R - Rotate Hand Right

	Mode0:
		Btn5U - Raise Arm
		Btn5D - Lower Arm
		Btn6U - Open Hand
		Btn6D - Close Hand
	Mode1:
		Btn5D - Open Hand
		Btn6D - Close Hand
		Btn5U - Raise Arm
		Btn6U - Lower Arm

********************************/
bool automationRunning = false;

task automation() {
	automationRunning = true;
	// 1. Move forward for 5 seconds
	motor[leftMotor] = 100;
	motor[rightMotor] = 100;
	wait1Msec(5000)
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;

	// 2. Turn right for 2 seconds
	motor[leftMotor] = 50;
	motor[rightMotor] = -50;
	wait1Msec(2000)
	motor[leftMotor] = 0;
	motor[leftMotor] = 0;

	// 3. Lift arm
	while (sensorValue(armRaiseLimit) < 0) {
		motor[armMotor] = 100;
	}

	motor[armMotor] = 0;

	// 4. open hand
	while (motor[handLeft] < maxServoPos && motor[handRight] < MaxServoNeg) {
		// Increment left side of hand
		motor[handLeft] = motor[handLeft] + servoIncrement;
		// Decrease right side of hand
		motor[handRight] = motor[handRight] - servoIncrement;
	}

	// 5. rotate hand


	// 6. Move forward for 5 second	
	motor[leftMotor] = 100;
	motor[rightMotor] = 100;
	wait1Msec(5000)
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;

	// 7. close hand
	while (motor[handLeft] < maxServoNeg && motor[handRight] < MaxServoPos) {
		// Increment left side of hand
		motor[handLeft] = motor[handLeft] - servoIncrement;
		// Decrease right side of hand
		motor[handRight] = motor[handRight] + servoIncrement;
	}

	// 8. move backward for 5 second
	motor[leftMotor] = -100;
	motor[rightMotor] = -100;
	wait1Msec(5000)
	motor[leftMotor] = 0;
	motor[rightMotor] = 0;
	// 9. Turn left for 2 seconds
	motor[leftMotor] = -50;
	motor[rightMotor] = 50;
	wait1Msec(2000)
	motor[leftMotor] = 0;
	motor[leftMotor] = 0;
	// end

	automationRunning = false;
}

task main()
{
	/* 
	  	Holds the current mode
		What ever is defined here will be mode on power
	*/
	int controlMode = 0; // 0 or 1 

  	// Define the min and max a servo can rotate
	int maxServoPos = 127;
	int maxServoNeg = -127;

	// Servo rotation increment
	int servoIncrement = 10;

	// Joystick Threshold servo rotation
	int threshold = 15;

	// Main Event Loop
	while (true) {

		// Toggle modes
		if (vexRT[Btn7L] == 1) {
			// If mode is 0 switch to 1
			if (controlMode == 0) {
				controlMode = 1;
			} else {
				controlMode = 0;
			}
		}

		// Start / stop automation
		if (vexRT[Btn7R] == 1) {
			if (!automationRunning) {
				startTask(automation)
			} else {
				stopTask(automation)
			}
		}

		/** Shared Controls **/

		// Moves the left wheel motor with left joystick
		if (vexRT[Ch3] > threshold || vexRT[Ch3] < -threshold) {
			motor[leftMotor] = vexRT[Ch3];
		} else {
			motor[leftMotor] = 0;
		}

		// Moves the right wheel motor with right joystick
		if (vexRT[Ch2] > threshold || vexRT[Ch2] < -threshold) {
			motor[rightMotor] = vexRT[Ch2];
		} else {
			motor[rightMotor] = 0;
		}

		// 7U - Catapult Launch
		if (vexRT[Btn7U] == 1) {
			// Make sure the servo isnt at the max position already.
			if (motor[catapult] < maxServoPos) {
				// Increment left side of hand
				motor[catapult] = motor[catapult] + servoIncrement;
			}
		}

		// 7D - Catapult Reset
		if (vexRT[Btn7D] == 1) {
			// Make sure the servo isnt at the min position already.
			if (motor[catapult] > maxServoNeg ) {
				// Decrease left side of hand
				motor[catapult] = motor[catapult] - servoIncrement;
			}
		}

		// 8L - Rotate Hand Left
		if (vexRT[Btn8L] == 1) {
			// Rotate motor to rotate hand
			motor[handMotor] = 100;

		// 8R - Rotate Hand Right
		} else if (vexRT[Btn8R] == 1) {
			// Provide a negative value which rotates the motor the opposite direction and rotates the hand right
			motor[handMotor] = -100;
		} else {
			// Stop motor
			motor[handMotor] = 0;
		}

		/** End Shared Controls **/

		/** Mode Specific Controls **/
		
		// All of the logic to handle buttons while in mode 0
		if (controlMode == 0) {
            controlMode0()
		} else {
			controlMode1()
		}
		/** End Mode Specific Controls **/
	}
	
}

void controlMode0() {
    // 6U - Open Hand
    if (vexRT[Btn6U] == 1) {
        // Make sure the servo isnt at the max position already.
        if (motor[handLeft] < maxServoPos) {
            // Increment left side of hand
            motor[handLeft] = motor[handLeft] + servoIncrement;
            // Decrease right side of hand
            motor[handRight] = motor[handRight] - servoIncrement;
        }
    }

    // 6D - Close Hand
    if (vexRT[Btn6D] == 1) {
        // Make sure the servo isnt at the min position already.
        if (motor[handLeft] > maxServoNeg ) {
            // Decrease left side of hand
            motor[handLeft] = motor[handLeft] - servoIncrement;
            // Increase right side of hand
            motor[handRight] = motor[handRight] + servoIncrement;
        }
    }

    // 5U - Raise Arm
    if (vexRT[Btn5U] == 1) {
        // Rotate motor to raise arm
        motor[armMotor] = 100;

    // 5D - Lower Arm
    } else if (vexRT[Btn5D] == 1) {
        // Provide a negative value which rotates the motor the opposite direction and lowers the arm.
        motor[armMotor] = -100;
    } else {
        // Stop motor
        motor[armMotor] = 0;
    }
}

void controlMode1() {
    // 5D - Open Hand
    if (vexRT[Btn5D] == 1) {
        // Make sure the servo isnt at the max position already.
        if (motor[handLeft] < maxServoPos) {
            // Increment left side of hand
            motor[handLeft] = motor[handLeft] + servoIncrement;
            // Decrease right side of hand
            motor[handRight] = motor[handRight] - servoIncrement;
        }
    }

    // 6D - Close Hand
    if (vexRT[Btn6D] == 1) {
        // Make sure the servo isnt at the min position already.
        if (motor[handLeft] > maxServoNeg ) {
            // Decrease left side of hand
            motor[handLeft] = motor[handLeft] - servoIncrement;
            // Increase right side of hand
            motor[handRight] = motor[handRight] + servoIncrement;
        }
    }

    // 5U - Raise Arm
    if (vexRT[Btn5U] == 1) {
        // Rotate motor to raise arm
        motor[armMotor] = 100;

    // 6U - Lower Arm
    } else if (vexRT[Btn6U] == 1) {
        // Provide a negative value which rotates the motor the opposite direction and lowers the arm.
        motor[armMotor] = -100;
    } else {
        // Stop motor
        motor[armMotor] = 0;
    }
}
