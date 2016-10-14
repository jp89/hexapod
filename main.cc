#include <memory>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <string>
#include <servo_controller.hpp>

using namespace  std;

static const uint8_t SERVO_MIN = 120;  // Min pulse length out of 4096
static const uint16_t SERVO_MAX = 500;  // Max pulse length out of 4096
static const vector<uint8_t> ADDR = { 0x40 };
static const uint8_t PWM_FREQ = 50;


static const string welcome_message[] = {
"    ____    ____                                                       ___                                    ",
"    `MM'    `MM'                                                       `MM                                    ",
"     MM      MM                                                         MM                                    ",
"     MM      MM   ____   ____   ___    ___    __ ____     _____     ____MM       ____    ___ __/       ____   ",
"     MM      MM  6MMMMb  `MM(   )P'  6MMMMb   `M6MMMMb   6MMMMMb   6MMMMMM       `MM(    )M' `MM      6MMMMb  ",
"     MMMMMMMMMM 6M'  `Mb  `MM` ,P   8M'  `Mb   MM'  `Mb 6M'   `Mb 6M'  `MM        `Mb    d'   MM     6M'  `Mb ",
"     MM      MM MM    MM   `MM,P        ,oMM   MM    MM MM     MM MM    MM         YM.  ,P    MM     MM    MM ",
"     MM      MM MMMMMMMM    `MM.    ,6MM9'MM   MM    MM MM     MM MM    MM          MM  M     MM     MM    MM ",
"     MM      MM MM          d`MM.   MM'   MM   MM    MM MM     MM MM    MM          `Mbd'     MM     MM    MM ",
"     MM      MM YM    d9   d' `MM.  MM.  ,MM   MM.  ,M9 YM.   ,M9 YM.  ,MM           YMP      MM 68b YM.  ,M9 ",
"    _MM_    _MM_ YMMMM9  _d_  _)MM_ `YMMM9'Yb. MMYMMM9   YMMMMM9   YMMMMMM_           M      _MM_Y89  YMMMM9  ",
"                                               MM                                                             ",
"                                               MM                                                             ",
"                                              _MM_                                                            "};


void showItemChoice(void)
{
    cout << "Menu: " << endl;
    cout << "[1]. Test servo. " << endl;
    cout << "[q]. Quit." << endl;
}

int main (void) {
    AdafruitServoController& controller = AdafruitServoController::getInstance();
    controller.Init(ADDR, PWM_FREQ, SERVO_MIN, SERVO_MAX);
//  for (const auto line : welcome_message)
//  {
//      cout << line << endl;
//  }
//  cout << "Testing program." << endl;
//  char usersChoice = '0';
//  while (usersChoice != 'q') {
//    cin >> usersChoice;

//    switch (usersChoice) {
//    case '1': {
//            unsigned newPulse = 1;
//            while (newPulse != 0)
//            {
//                cout << "Enter servo id: " << endl;
//                unsigned id = 0;
//                cin >> id;
//                cout << "Enter new pulse: " << endl;
//                cin >> newPulse;
//                controller.setServo(id, newPulse);
//            }
//        }
//        break;
//    default:
//        break;
//    }
//  }
    sleep (3);

    vector<uint16_t> angles = { 45, 135, 90 };
    for (const auto& angle : angles) {
        for (int i = 0; i < 4; i++) {
            controller.setServo(i, angle, 2);
        }
        sleep(3);
    }
  return 0;
}
