#ifndef ATTENDANCESYSTEM_H
#define ATTENDANCESYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <map>
#include <ctime>

using namespace std;

const int TOTAL_CLASSES = 5;
const float MIN_ATTENDANCE = 75.0f;

// ─── Exception Template ─────
template <typename T>
class AttendanceException
{
private:
    string message;
    T payload;

public:
    AttendanceException(string msg, T data) : message(msg), payload(data) {}
    string what() const { return message; }
    T data() const { return payload; }
};

// ─── Validator Template ────────────────────────────────────────────────────
template <typename T>
class Validator
{
public:
    static bool inRange(T value, T minVal, T maxVal)
    {
        return value >= minVal && value <= maxVal;
    }
    static bool isPositive(T value)
    {
        return value > 0;
    }
    static bool isNotEmpty(const string &s)
    {
        return !s.empty();
    }
};

// ─── Person (Abstract Base) ────────────────────────────────────────────────
class Person
{
protected:
    string name;
    string id;

public:
    Person(string name, string id);
    virtual void display() const = 0;
    virtual string getRole() const = 0;
    string getName() const { return name; }
    string getId() const { return id; }
    virtual ~Person();
};

// ─── Student ───────────────────────────────────────────────────────────────
class Student : virtual public Person
{
private:
    int rollNo;
    string department;
    string semester;
    string managedBy; // teacher ID who manages this student
    float attendancePercentage;
    int presentClasses;

public:
    Student(string name, string id, int rollNo,
            string department, string semester,
            string managedBy = "");

    void display() const override;
    void markPresent();
    void markAbsent();
    void calculateAttendance();
    void checkEligibility() const;

    float getAttendance() const { return attendancePercentage; }
    int getRollNo() const { return rollNo; }
    string getDepartment() const { return department; }
    string getSemester() const { return semester; }
    string getManagedBy() const { return managedBy; }
    int getPresentClasses() const { return presentClasses; }
    int getTotalClasses() const { return TOTAL_CLASSES; }
    string getRole() const override { return "Student"; }

    void setAttendance(float p);
    void setPresentClasses(int p);
    void setManagedBy(string teacherId) { managedBy = teacherId; }
    string toFileString() const;
};

// ─── Teacher ───────────────────────────────────────────────────────────────
class Teacher : virtual public Person
{
private:
    string subject;
    string designation;
    string username;
    string password;

public:
    Teacher(string name, string id, string subject,
            string designation, string username = "", string password = "");

    void display() const override;
    string getRole() const override { return "Teacher"; }
    string toFileString() const;

    string getSubject() const { return subject; }
    string getDesignation() const { return designation; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }

    void setUsername(string u) { username = u; }
    void setPassword(string p) { password = p; }
};

// ─── StudentTeacher (Multiple Inheritance) ─────────────────────────────────
class StudentTeacher : public Student, public Teacher
{
private:
    string extraRole;

public:
    StudentTeacher(string name, string id, int rollNo,
                   string department, string semester,
                   string subject, string designation,
                   string extraRole);

    void display() const override;
    string getRole() const override { return "StudentTeacher"; }
};

// ─── Auth System ───────────────────────────────────────────────────────────
class AuthSystem
{
private:
    map<string, string> adminCredentials;
    map<string, string> studentCredentials; // studentId -> password

public:
    AuthSystem();
    bool authenticateAdmin(const string &username, const string &password) const;
    bool authenticateStudent(const string &studentId, const string &password) const;
    void addStudentCredential(const string &id, const string &password);
    void removeStudentCredential(const string &id);

    // Default admin: admin/admin123
    // Default student password: student ID
};

// ─── AttendanceManager ─────────────────────────────────────────────────────
// FIX: Now holds a pointer to AuthSystem so that addStudent() and
//      removeStudent() always keep credentials in sync, regardless of
//      which menu (Admin or Teacher) triggers the operation.
class AttendanceManager
{
private:
    vector<Student *> students;
    vector<Teacher *> teachers;
    string filename;
    AuthSystem *auth; // ← NEW: optional link to auth system

    int findStudentIndex(string id) const;
    int findTeacherIndex(string id) const;

public:
    // FIX: auth pointer defaults to nullptr so existing call-sites that
    //      don't pass it still compile; when provided, credentials are
    //      managed automatically.
    AttendanceManager(string file = "attendance_data.csv", AuthSystem *auth = nullptr);
    ~AttendanceManager();

    // FIX: call setAuthSystem() once at startup to wire up auth.
    void setAuthSystem(AuthSystem *a) { auth = a; }

    // Student operations
    void addStudent(Student *s);   // ← now also registers credentials
    void removeStudent(string id); // ← now also removes credentials
    Student *findStudent(string id) const;
    void updateStudent(string id, string dept, string sem);

    // Teacher operations
    void addTeacher(Teacher *t);
    void removeTeacher(string id);
    Teacher *findTeacher(string id) const;

    // Display
    void displayAllStudents() const;
    void displayAllTeachers() const;
    void displayAllPersons() const;

    // Attendance
    void markAttendance(string id, bool present);
    void showAttendanceReport() const;

    // File I/O
    void saveToFile();
    void loadFromFile(); // ← now also re-registers credentials on load
    void saveToHTML();

    int getStudentCount() const { return (int)students.size(); }
    int getTeacherCount() const { return (int)teachers.size(); }

    const vector<Student *> &getStudents() const { return students; }
    const vector<Teacher *> &getTeachers() const { return teachers; }
};

// ─── Role-based Menus ──────────────────────────────────────────────────────
class StudentMenu
{
private:
    AttendanceManager &manager;
    string studentId;

    void viewMyRecord() const;
    void viewMyAttendance() const;

public:
    StudentMenu(AttendanceManager &mgr, string id);
    void run();
};

class TeacherMenu
{
private:
    AttendanceManager &manager;
    string teacherId;

    void menuAddStudent();
    void menuMarkAttendance();
    void menuViewStudents() const;
    void menuSearchStudent() const;
    void menuRemoveStudent();
    void menuViewReport() const;
    void menuUpdateStudent();

    int getInt(string prompt, int min, int max);
    string getString(string prompt);

public:
    TeacherMenu(AttendanceManager &mgr, string tid);
    void run();
};

class AdminMenu
{
private:
    AttendanceManager &manager;
    AuthSystem &auth;

    void menuAddTeacher();
    void menuRemoveTeacher();
    void menuViewTeachers() const;
    void menuViewAllRecords() const;
    void menuFileOps();
    void menuAddStudent();
    void menuRemoveStudent();

    int getInt(string prompt, int min, int max);
    string getString(string prompt);

public:
    AdminMenu(AttendanceManager &mgr, AuthSystem &auth);
    void run();
};

// ─── Main Menu / Login System ──────────────────────────────────────────────
class MenuSystem
{
private:
    AttendanceManager &manager;
    AuthSystem &auth;

    void printLine() const;
    void printHeader(string title) const;
    void pause() const;
    int getInt(string prompt, int min, int max);
    string getString(string prompt);

    void loginAsAdmin();
    void loginAsTeacher();
    void loginAsStudent();

public:
    MenuSystem(AttendanceManager &mgr, AuthSystem &auth);
    void run();
    void showMainMenu() const;
};

#endif