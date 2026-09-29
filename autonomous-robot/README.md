# Autonomous Robot – UCD Robotic Design Project (2024)

A team of three of us designed, built and programmed an autonomous robot, which then competed against other teams' robots in a competition sponsored by Infineon. The robot runs on an Arduino with a motor-driver shield, a 16x2 LCD button shield, a BNO055 9-axis IMU, an optical wheel encoder, an IR distance sensor and two servos.

This folder contains the C++ (Arduino) sketches we wrote, from the early sensor labs to the final competition program.

## Structure
```
autonomous-robot/
├── competition/
│   └── CompetitionRun/            Final match program: holds a heading, then turns onto a new heading
├── heading-control/
│   ├── ControlLoop_v1/            First heading controller (error fed straight to steering)
│   ├── ControlLoop_v2/            Simplified loop that holds a single heading
│   └── ControlLoop_v3/            Tuned proportional controller (Kp = 0.9)
├── sensor-labs/
│   ├── WheelOdometry/             Counts encoder holes to measure distance travelled
│   ├── DistanceSensorCalibration/ Maps IR sensor voltage to distance (about 8–30 cm)
│   └── WallReverseChallenge/      Drives to within 10 cm of a wall, then reverses 20 cm
└── servo-tests/
    ├── ServoManualControl/        Adjust servo angle and speed live using the LCD buttons
    └── DriveAndServo/             Drives the motors while running the continuous servo
```

## How it works

### Heading control (IMU and proportional feedback)
The BNO055 gives the robot's compass heading. `signed_angle_fromX_toY()` finds the shortest signed angle between the current and target heading, wrapping the result into −180°…+180° so the robot always turns the short way round:

```cpp
int signed_angle_fromX_toY(int x, int y){
    int clockwise_angle = y - x;
    if (clockwise_angle > 180)       return clockwise_angle - 360;
    else if (clockwise_angle < -180) return clockwise_angle + 360;
    else                             return clockwise_angle;
}
```

The error is multiplied by a gain `Kp` and passed to `steer()`, which slows one wheel relative to the other to turn back onto course. Versions v1 → v3 show the controller being developed and tuned.

### Wheel odometry
An optical sensor looks through an 8-hole gear. The code detects each **rising edge** of the sensor voltage (the previous sample is below the threshold and the new one is above it) to count holes. Distance is then worked out as:

```
distance = (holes / 8) × gear ratio (5/3) × wheel circumference (8.2 cm)
```

### Distance sensing
We logged IR sensor readings at known distances and used `map()` to convert readings into an approximate distance between about 8 and 30 cm. The wall challenge combines this with odometry: drive until the wall is within 10 cm, stop, then reverse exactly 20 cm.

### Match start and shutdown
The competition sketch uses the course-provided `arm_and_start()` routine. It syncs a countdown with the other robots and arms on a button press. An interrupt driven by a 31 Hz signal on pin 11 then cuts the motors when the match time runs out.

## Libraries needed
Install these through **Sketch → Include Library → Manage Libraries** in the Arduino IDE:
- `Arduino_NineAxesMotion` (BNO055 IMU)
- `EnableInterrupt`
- `LCD16x2` (DFRobot LCD shield)
- `Servo` and `Wire` (built in)

## Credits
Team 6: Abigail, Eoin and Paddy. The project was supervised by Assoc. Prof. Paul Cuffe. The start-up and shutdown routine (`arm_and_start` and `CheckCycle`) and the base sketch layout were provided by the module.
