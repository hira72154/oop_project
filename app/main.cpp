#include "AttendanceSystem.h"

int main()
{
    AttendanceManager manager("attendance_data.csv");
    AuthSystem auth;

    MenuSystem menu(manager, auth);
    menu.run();

    return 0;
}