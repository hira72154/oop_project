#include "../include/AttendanceSystem.h"
//  Person
Person::Person(string name, string id) : name(name), id(id) {}
Person::~Person() {}
//  Student
Student::Student(string name, string id, int rollNo,
                 string department, string semester,
                 string managedBy)
    : Person(name, id),
      rollNo(rollNo), department(department),
      semester(semester), managedBy(managedBy),
      attendancePercentage(0.0f), presentClasses(0)
{
    if (!Validator<int>::isPositive(rollNo))
        throw AttendanceException<int>("Roll number must be a positive number!", rollNo);
    if (name.empty())
        throw AttendanceException<string>("Student name cannot be empty!", name);
}

void Student::display() const
{
    cout << "\n  +--------------------------------------------------+\n";
    cout << "  |                STUDENT RECORD                    |\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |  Name        : " << name << "\n";
    cout << "  |  ID          : " << id << "\n";
    cout << "  |  Roll No     : " << rollNo << "\n";
    cout << "  |  Department  : " << department << "\n";
    cout << "  |  Semester    : " << semester << "\n";
    if (!managedBy.empty())
        cout << "  |  Teacher ID  : " << managedBy << "\n";
    cout << "  |  Attendance  : " << presentClasses << "/" << TOTAL_CLASSES << " classes\n";
    cout << fixed << setprecision(2);
    cout << "  |  Percentage  : " << attendancePercentage << "%\n";
    string status = (attendancePercentage >= MIN_ATTENDANCE) ? "ELIGIBLE" : "NOT ELIGIBLE";
    cout << "  |  Exam Status : " << status << "\n";
    cout << "  +--------------------------------------------------+\n";
}

void Student::markPresent()
{
    if (presentClasses < TOTAL_CLASSES)
    {
        presentClasses++;
        calculateAttendance();
    }
    else
    {
        cout << "  [!] All " << TOTAL_CLASSES << " classes already recorded.\n";
    }
}

void Student::markAbsent()
{
    calculateAttendance();
}

void Student::calculateAttendance()
{
    attendancePercentage = (static_cast<float>(presentClasses) / TOTAL_CLASSES) * 100.0f;
}

void Student::checkEligibility() const
{
    cout << "  Exam Status : ";
    if (attendancePercentage >= MIN_ATTENDANCE)
        cout << "ELIGIBLE\n";
    else
        cout << "NOT ELIGIBLE (Attendance below 75%)\n";
}

void Student::setAttendance(float p)
{
    if (!Validator<float>::inRange(p, 0.0f, 100.0f))
        throw AttendanceException<float>("Attendance must be between 0 and 100!", p);
    attendancePercentage = p;
}

void Student::setPresentClasses(int p)
{
    presentClasses = p;
    calculateAttendance();
}

string Student::toFileString() const
{
    stringstream ss;
    ss << "STUDENT," << name << "," << id << ","
       << rollNo << "," << department << ","
       << semester << ","
       << fixed << setprecision(2) << attendancePercentage << ","
       << TOTAL_CLASSES << "," << presentClasses << ","
       << managedBy;
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════
//  Teacher
// ═══════════════════════════════════════════════════════════════
Teacher::Teacher(string name, string id, string subject,
                 string designation, string username, string password)
    : Person(name, id), subject(subject),
      designation(designation), username(username), password(password)
{
    if (name.empty())
        throw AttendanceException<string>("Teacher name cannot be empty!", name);
}

void Teacher::display() const
{
    cout << "\n  +--------------------------------------------------+\n";
    cout << "  |                TEACHER RECORD                    |\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |  Name        : " << name << "\n";
    cout << "  |  ID          : " << id << "\n";
    cout << "  |  Subject     : " << subject << "\n";
    cout << "  |  Designation : " << designation << "\n";
    cout << "  |  Username    : " << username << "\n";
    cout << "  +--------------------------------------------------+\n";
}

string Teacher::toFileString() const
{
    stringstream ss;
    ss << "TEACHER," << name << "," << id << ","
       << subject << "," << designation << ","
       << username << "," << password;
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════
//  StudentTeacher
// ═══════════════════════════════════════════════════════════════
StudentTeacher::StudentTeacher(string name, string id, int rollNo,
                               string department, string semester,
                               string subject, string designation,
                               string extraRole)
    : Person(name, id),
      Student(name, id, rollNo, department, semester),
      Teacher(name, id, subject, designation),
      extraRole(extraRole) {}

void StudentTeacher::display() const
{
    cout << "\n  +--------------------------------------------------+\n";
    cout << "  |            STUDENT-TEACHER RECORD                |\n";
    cout << "  +--------------------------------------------------+\n";
    cout << "  |  Name       : " << name << "\n";
    cout << "  |  ID         : " << id << "\n";
    cout << "  |  Department : " << getDepartment() << "\n";
    cout << "  |  Semester   : " << getSemester() << "\n";
    cout << "  |  Subject    : " << getSubject() << "\n";
    cout << "  |  Extra Role : " << extraRole << "\n";
    cout << fixed << setprecision(2);
    cout << "  |  Attendance : " << getAttendance() << "%\n";
    cout << "  +--------------------------------------------------+\n";
}

// ═══════════════════════════════════════════════════════════════
//  AuthSystem
// ═══════════════════════════════════════════════════════════════
AuthSystem::AuthSystem()
{
    adminCredentials["admin"] = "admin123";
}

bool AuthSystem::authenticateAdmin(const string &username, const string &password) const
{
    auto it = adminCredentials.find(username);
    if (it == adminCredentials.end() || it->second != password)
        throw AttendanceException<string>("Invalid admin credentials!", username);
    return true;
}

bool AuthSystem::authenticateStudent(const string &studentId, const string &password) const
{
    auto it = studentCredentials.find(studentId);
    if (it == studentCredentials.end() || it->second != password)
        throw AttendanceException<string>("Invalid student credentials!", studentId);
    return true;
}

void AuthSystem::addStudentCredential(const string &id, const string &password)
{
    studentCredentials[id] = password;
}

void AuthSystem::removeStudentCredential(const string &id)
{
    studentCredentials.erase(id);
}

// ═══════════════════════════════════════════════════════════════
//  AttendanceManager
// ═══════════════════════════════════════════════════════════════

// FIX: Constructor now accepts an optional AuthSystem pointer.
AttendanceManager::AttendanceManager(string file, AuthSystem *authPtr)
    : filename(file), auth(authPtr) {}

AttendanceManager::~AttendanceManager()
{
    for (Student *s : students)
        delete s;
    for (Teacher *t : teachers)
        delete t;
}

int AttendanceManager::findStudentIndex(string id) const
{
    for (int i = 0; i < (int)students.size(); i++)
        if (students[i]->getId() == id)
            return i;
    return -1;
}

int AttendanceManager::findTeacherIndex(string id) const
{
    for (int i = 0; i < (int)teachers.size(); i++)
        if (teachers[i]->getId() == id)
            return i;
    return -1;
}

// FIX: addStudent() now automatically registers login credentials in
//      AuthSystem (default password = student ID) whenever auth is set.
//      This ensures the credential is created whether the call comes from
//      AdminMenu, TeacherMenu, or any future code path.
void AttendanceManager::addStudent(Student *s)
{
    if (findStudentIndex(s->getId()) != -1)
        throw AttendanceException<string>("A student with this ID already exists!", s->getId());

    students.push_back(s);

    // Automatically create login credentials (password defaults to student ID).
    if (auth != nullptr)
    {
        auth->addStudentCredential(s->getId(), s->getId());
        cout << "  [+] Student record added successfully.\n";
        cout << "  [+] Login credentials created (username: " << s->getId()
             << ", password: " << s->getId() << ").\n";
    }
    else
    {
        cout << "  [+] Student record added successfully.\n";
    }
}

// FIX: removeStudent() now automatically removes the corresponding login
//      credentials from AuthSystem whenever auth is set, keeping both
//      stores in sync.
void AttendanceManager::removeStudent(string id)
{
    int idx = findStudentIndex(id);
    if (idx == -1)
        throw AttendanceException<string>("No student found with ID:", id);

    delete students[idx];
    students.erase(students.begin() + idx);

    // Automatically remove login credentials.
    if (auth != nullptr)
    {
        auth->removeStudentCredential(id);
        cout << "  [-] Student record and credentials removed successfully.\n";
    }
    else
    {
        cout << "  [-] Student record removed successfully.\n";
    }
}

Student *AttendanceManager::findStudent(string id) const
{
    int idx = findStudentIndex(id);
    if (idx == -1)
        throw AttendanceException<string>("No student found with ID:", id);
    return students[idx];
}

void AttendanceManager::updateStudent(string id, string dept, string sem)
{
    Student *s = findStudent(id);
    (void)s;
    (void)dept;
    (void)sem;
    cout << "  [!] Use Remove + Add to update department/semester.\n";
}

void AttendanceManager::addTeacher(Teacher *t)
{
    if (findTeacherIndex(t->getId()) != -1)
        throw AttendanceException<string>("A teacher with this ID already exists!", t->getId());
    teachers.push_back(t);
    cout << "  [+] Teacher record added successfully.\n";
}

void AttendanceManager::removeTeacher(string id)
{
    int idx = findTeacherIndex(id);
    if (idx == -1)
        throw AttendanceException<string>("No teacher found with ID:", id);
    delete teachers[idx];
    teachers.erase(teachers.begin() + idx);
    cout << "  [-] Teacher record removed successfully.\n";
}

Teacher *AttendanceManager::findTeacher(string id) const
{
    int idx = findTeacherIndex(id);
    if (idx == -1)
        throw AttendanceException<string>("No teacher found with ID:", id);
    return teachers[idx];
}

void AttendanceManager::displayAllStudents() const
{
    if (students.empty())
    {
        cout << "  No student records found.\n";
        return;
    }
    cout << "\n  ===== STUDENT RECORDS (" << students.size() << ") =====\n";
    for (Student *s : students)
        s->display();
}

void AttendanceManager::displayAllTeachers() const
{
    if (teachers.empty())
    {
        cout << "  No teacher records found.\n";
        return;
    }
    cout << "\n  ===== TEACHER RECORDS (" << teachers.size() << ") =====\n";
    for (Teacher *t : teachers)
        t->display();
}

void AttendanceManager::displayAllPersons() const
{
    cout << "\n  ===== ALL RECORDS =====\n";
    if (students.empty() && teachers.empty())
    {
        cout << "  No records found.\n";
        return;
    }
    for (Student *s : students)
        s->display();
    for (Teacher *t : teachers)
        t->display();
}

void AttendanceManager::markAttendance(string id, bool present)
{
    Student *s = findStudent(id);
    if (present)
    {
        s->markPresent();
        cout << "  Marked PRESENT: " << s->getName() << "\n";
    }
    else
    {
        s->markAbsent();
        cout << "  Marked ABSENT: " << s->getName() << "\n";
    }
    if (s->getAttendance() < MIN_ATTENDANCE)
    {
        cout << "  [WARNING] " << s->getName()
             << "'s attendance is " << fixed << setprecision(1)
             << s->getAttendance() << "% -- below 75% threshold!\n";
    }
}

void AttendanceManager::showAttendanceReport() const
{
    cout << "\n  ===== ATTENDANCE REPORT =====\n";
    if (students.empty())
    {
        cout << "  No student records found.\n";
        return;
    }
    int eligible = 0, notEligible = 0;
    for (Student *s : students)
    {
        s->display();
        if (s->getAttendance() >= MIN_ATTENDANCE)
            eligible++;
        else
            notEligible++;
    }
    cout << "\n  Summary:\n";
    cout << "  Eligible     : " << eligible << "\n";
    cout << "  Not Eligible : " << notEligible << "\n";
}

void AttendanceManager::saveToFile()
{
    ofstream file(filename);
    if (!file.is_open())
        throw AttendanceException<string>("Unable to open file for writing!", filename);

    file << "TYPE,NAME,ID,FIELD1,FIELD2,FIELD3,FIELD4,FIELD5,FIELD6,FIELD7\n";
    for (Student *s : students)
        file << s->toFileString() << "\n";
    for (Teacher *t : teachers)
        file << t->toFileString() << "\n";
    file.close();

    cout << "  Data saved to " << filename << "\n";
    saveToHTML();
}

// FIX: loadFromFile() now re-registers credentials for every student that
//      is loaded from disk, so credentials survive a save-and-reload cycle.
void AttendanceManager::loadFromFile()
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "  No existing data file found. Starting fresh.\n";
        return;
    }

    // Clear old credentials for students we're about to replace.
    if (auth != nullptr)
    {
        for (Student *s : students)
            auth->removeStudentCredential(s->getId());
    }

    for (Student *s : students)
        delete s;
    for (Teacher *t : teachers)
        delete t;
    students.clear();
    teachers.clear();

    string line;
    bool firstLine = true;
    int count = 0;

    while (getline(file, line))
    {
        if (line.empty())
            continue;
        if (firstLine)
        {
            firstLine = false;
            continue;
        }

        stringstream ss(line);
        string type;
        getline(ss, type, ',');

        try
        {
            if (type == "STUDENT")
            {
                string name, id, dept, sem, managedBy;
                int roll, total, present;
                float attendance;

                getline(ss, name, ',');
                getline(ss, id, ',');
                ss >> roll;
                ss.ignore();
                getline(ss, dept, ',');
                getline(ss, sem, ',');
                ss >> attendance;
                ss.ignore();
                ss >> total;
                ss.ignore();
                ss >> present;
                ss.ignore();
                getline(ss, managedBy);

                Student *s = new Student(name, id, roll, dept, sem, managedBy);
                s->setPresentClasses(present);
                students.push_back(s);

                // Re-register credentials after loading from file.
                if (auth != nullptr)
                    auth->addStudentCredential(id, id);

                count++;
            }
            else if (type == "TEACHER")
            {
                string name, id, subject, designation, username, password;
                getline(ss, name, ',');
                getline(ss, id, ',');
                getline(ss, subject, ',');
                getline(ss, designation, ',');
                getline(ss, username, ',');
                getline(ss, password);
                teachers.push_back(new Teacher(name, id, subject, designation, username, password));
                count++;
            }
        }
        catch (...)
        {
        }
    }

    file.close();
    cout << "  Data loaded successfully. (" << count << " records)\n";
}

void AttendanceManager::saveToHTML()
{
    string htmlFile = "attendance_report.html";
    ofstream f(htmlFile);
    if (!f.is_open())
        return;

    int eligible = 0;
    float totalAtt = 0.0f;
    for (Student *s : students)
    {
        if (s->getAttendance() >= MIN_ATTENDANCE)
            eligible++;
        totalAtt += s->getAttendance();
    }

    time_t now = time(nullptr);
    string timeStr = string(ctime(&now));
    if (!timeStr.empty() && timeStr.back() == '\n')
        timeStr.pop_back();

    // ── Head ──────────────────────────────────────────────────────
    f << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    f << "  <meta charset=\"UTF-8\">\n";
    f << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    f << "  <title>Attendance Dashboard</title>\n";
    f << "  <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
    f << "  <link href=\"https://fonts.googleapis.com/css2?family=Syne:wght@400;600;700;800&family=DM+Sans:wght@300;400;500&display=swap\" rel=\"stylesheet\">\n";
    f << "  <style>\n";
    f << "    :root{--bg:#05080f;--card:#0c1018;--border:#161c28;--border2:#1e2738;\n";
    f << "          --blue:#3b82f6;--cyan:#22d3ee;--green:#34d399;--red:#f87171;\n";
    f << "          --amber:#fbbf24;--text:#94a3b8;--bright:#e2e8f0;--dim:#475569;}\n";
    f << "    *,*::before,*::after{box-sizing:border-box;margin:0;padding:0;}\n";
    f << "    body{font-family:'DM Sans',sans-serif;background:var(--bg);color:var(--text);min-height:100vh;}\n";
    f << "    .topbar{background:var(--card);border-bottom:1px solid var(--border);padding:0 40px;\n";
    f << "            display:flex;align-items:center;justify-content:space-between;height:64px;\n";
    f << "            position:sticky;top:0;z-index:100;}\n";
    f << "    .topbar-logo{font-family:'Syne',sans-serif;font-size:18px;font-weight:800;color:var(--bright);letter-spacing:-0.5px;}\n";
    f << "    .topbar-logo span{color:var(--blue);}\n";
    f << "    .topbar-meta{font-size:12px;color:var(--dim);}\n";
    f << "    .topbar-badge{background:#3b82f615;border:1px solid #3b82f640;color:var(--blue);padding:4px 12px;border-radius:20px;font-size:11px;font-weight:500;}\n";
    f << "    .layout{display:grid;grid-template-columns:240px 1fr;min-height:calc(100vh - 64px);}\n";
    f << "    .sidebar{background:var(--card);border-right:1px solid var(--border);padding:32px 0;}\n";
    f << "    .sidebar-section{padding:0 20px;margin-bottom:32px;}\n";
    f << "    .sidebar-label{font-size:10px;letter-spacing:2px;text-transform:uppercase;color:var(--dim);margin-bottom:12px;padding-left:12px;}\n";
    f << "    .nav-item{display:flex;align-items:center;gap:10px;padding:10px 12px;border-radius:8px;\n";
    f << "              cursor:pointer;font-size:14px;color:var(--text);transition:all .15s;\n";
    f << "              margin-bottom:2px;text-decoration:none;}\n";
    f << "    .nav-item:hover,.nav-item.active{background:#3b82f610;color:var(--bright);}\n";
    f << "    .nav-item.active{color:var(--blue);}\n";
    f << "    .nav-dot{width:8px;height:8px;border-radius:50%;background:var(--border2);flex-shrink:0;}\n";
    f << "    .nav-item.active .nav-dot{background:var(--blue);}\n";
    f << "    .main{padding:40px;}\n";
    f << "    .search-wrap{margin-bottom:32px;position:relative;}\n";
    f << "    .search-wrap input{width:100%;background:var(--card);border:1px solid var(--border2);\n";
    f << "                       border-radius:10px;padding:14px 18px 14px 46px;color:var(--bright);\n";
    f << "                       font-family:'DM Sans',sans-serif;font-size:14px;outline:none;transition:border-color .2s;}\n";
    f << "    .search-wrap input:focus{border-color:var(--blue);}\n";
    f << "    .search-wrap input::placeholder{color:var(--dim);}\n";
    f << "    .search-icon{position:absolute;left:16px;top:50%;transform:translateY(-50%);color:var(--dim);font-size:16px;}\n";
    f << "    .stats-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:16px;margin-bottom:32px;}\n";
    f << "    .stat-card{background:var(--card);border:1px solid var(--border);border-radius:12px;padding:24px 20px;position:relative;overflow:hidden;}\n";
    f << "    .stat-card::after{content:'';position:absolute;top:0;left:0;right:0;height:2px;}\n";
    f << "    .stat-card.blue::after{background:var(--blue);}\n";
    f << "    .stat-card.cyan::after{background:var(--cyan);}\n";
    f << "    .stat-card.green::after{background:var(--green);}\n";
    f << "    .stat-card.red::after{background:var(--red);}\n";
    f << "    .stat-num{font-family:'Syne',sans-serif;font-size:40px;font-weight:800;line-height:1;margin-bottom:6px;}\n";
    f << "    .stat-card.blue .stat-num{color:var(--blue);}\n";
    f << "    .stat-card.cyan .stat-num{color:var(--cyan);}\n";
    f << "    .stat-card.green .stat-num{color:var(--green);}\n";
    f << "    .stat-card.red .stat-num{color:var(--red);}\n";
    f << "    .stat-lbl{font-size:11px;letter-spacing:1.5px;text-transform:uppercase;color:var(--dim);}\n";
    f << "    .sec-title{font-family:'Syne',sans-serif;font-size:18px;font-weight:700;color:var(--bright);\n";
    f << "               margin-bottom:16px;display:flex;align-items:center;gap:12px;}\n";
    f << "    .sec-title::after{content:'';flex:1;height:1px;background:var(--border);}\n";
    f << "    .table-wrap{background:var(--card);border:1px solid var(--border);border-radius:12px;overflow:hidden;margin-bottom:32px;}\n";
    f << "    table{width:100%;border-collapse:collapse;}\n";
    f << "    th{background:#090e18;color:var(--dim);font-size:10px;letter-spacing:1.5px;text-transform:uppercase;\n";
    f << "       padding:14px 18px;text-align:left;font-weight:500;border-bottom:1px solid var(--border);}\n";
    f << "    td{padding:14px 18px;border-bottom:1px solid #0c1220;font-size:13px;color:var(--text);vertical-align:middle;}\n";
    f << "    tr:last-child td{border-bottom:none;}\n";
    f << "    tr:hover td{background:#0f1520;}\n";
    f << "    .empty-row td{text-align:center;color:var(--dim);padding:40px;}\n";
    f << "    .name-cell{display:flex;align-items:center;gap:10px;}\n";
    f << "    .avatar{width:32px;height:32px;border-radius:8px;display:flex;align-items:center;\n";
    f << "            justify-content:center;font-family:'Syne',sans-serif;font-size:12px;font-weight:700;flex-shrink:0;}\n";
    f << "    .name-bright{color:var(--bright);font-weight:500;}\n";
    f << "    .badge{display:inline-flex;align-items:center;padding:3px 10px;border-radius:20px;font-size:11px;font-weight:500;}\n";
    f << "    .badge-eligible{background:#34d39915;color:var(--green);border:1px solid #34d39935;}\n";
    f << "    .badge-not{background:#f8717115;color:var(--red);border:1px solid #f8717135;}\n";
    f << "    .bar-cell{display:flex;align-items:center;gap:10px;min-width:130px;}\n";
    f << "    .bar-track{flex:1;height:4px;background:var(--border2);border-radius:2px;overflow:hidden;}\n";
    f << "    .bar-fill{height:100%;border-radius:2px;}\n";
    f << "    .bar-pct{font-size:12px;font-weight:500;min-width:38px;text-align:right;}\n";
    f << "    .footer{text-align:center;padding:32px;color:var(--dim);font-size:11px;\n";
    f << "            letter-spacing:1px;border-top:1px solid var(--border);text-transform:uppercase;}\n";
    f << "  </style>\n</head>\n<body>\n";

    // ── Topbar ────────────────────────────────────────────────────
    f << "<div class=\"topbar\">\n";
    f << "  <div class=\"topbar-logo\">Attend<span>Ease</span></div>\n";
    f << "  <div class=\"topbar-meta\">Generated: " << timeStr << "</div>\n";
    f << "  <div class=\"topbar-badge\">Student Dashboard</div>\n";
    f << "</div>\n";
    f << "<div class=\"layout\">\n";

    // ── Sidebar ───────────────────────────────────────────────────
    f << "  <aside class=\"sidebar\">\n";
    f << "    <div class=\"sidebar-section\">\n";
    f << "      <div class=\"sidebar-label\">Navigation</div>\n";
    f << "      <a class=\"nav-item active\" href=\"#students\"><span class=\"nav-dot\"></span> Students</a>\n";
    f << "      <a class=\"nav-item\" href=\"#report\"><span class=\"nav-dot\"></span> Report</a>\n";
    f << "    </div>\n";
    f << "    <div class=\"sidebar-section\">\n";
    f << "      <div class=\"sidebar-label\">Summary</div>\n";
    f << "      <div style=\"padding:12px;background:#0f1520;border-radius:8px;font-size:13px;\">\n";
    f << "        <div style=\"display:flex;justify-content:space-between;margin-bottom:8px;\"><span>Total Students</span><span style=\"color:var(--blue);font-weight:600;\">" << students.size() << "</span></div>\n";
    f << "        <div style=\"display:flex;justify-content:space-between;margin-bottom:8px;\"><span>Total Teachers</span><span style=\"color:var(--cyan);font-weight:600;\">" << teachers.size() << "</span></div>\n";
    f << "        <div style=\"display:flex;justify-content:space-between;\"><span>Eligible</span><span style=\"color:var(--green);font-weight:600;\">" << eligible << "</span></div>\n";
    f << "      </div>\n    </div>\n  </aside>\n";

    // ── Main ──────────────────────────────────────────────────────
    f << "  <main class=\"main\">\n";
    f << "    <div class=\"search-wrap\">\n";
    f << "      <span class=\"search-icon\">&#9906;</span>\n";
    f << "      <input type=\"text\" id=\"searchInput\" placeholder=\"Search students by name, ID, department...\" oninput=\"filterStudents()\">\n";
    f << "    </div>\n";

    // Stats
    f << "    <div class=\"stats-grid\">\n";
    f << "      <div class=\"stat-card blue\"><div class=\"stat-num\">" << students.size() << "</div><div class=\"stat-lbl\">Total Students</div></div>\n";
    f << "      <div class=\"stat-card cyan\"><div class=\"stat-num\">" << teachers.size() << "</div><div class=\"stat-lbl\">Teachers</div></div>\n";
    f << "      <div class=\"stat-card green\"><div class=\"stat-num\">" << eligible << "</div><div class=\"stat-lbl\">Eligible</div></div>\n";
    f << "      <div class=\"stat-card red\"><div class=\"stat-num\">" << (students.size() - eligible) << "</div><div class=\"stat-lbl\">Not Eligible</div></div>\n";
    f << "    </div>\n";

    // Student Table
    f << "    <div id=\"students\">\n";
    f << "      <div class=\"sec-title\">Student Records</div>\n";
    f << "      <div class=\"table-wrap\"><table id=\"studentTable\">\n";
    f << "        <thead><tr>\n";
    f << "          <th>No.</th><th>Student</th><th>Roll</th><th>Department</th><th>Semester</th><th>Teacher ID</th><th>Attendance</th><th>Status</th>\n";
    f << "        </tr></thead>\n";
    f << "        <tbody id=\"studentBody\">\n";

    if (students.empty())
    {
        f << "          <tr class=\"empty-row\"><td colspan=\"8\">No student records found.</td></tr>\n";
    }
    else
    {
        string colors[] = {"#3b82f6", "#22d3ee", "#34d399", "#a78bfa", "#fb7185", "#fbbf24"};
        int i = 0;
        for (Student *s : students)
        {
            float att = s->getAttendance();
            string barColor = att >= 75.0f ? "#34d399" : att >= 50.0f ? "#fbbf24"
                                                                      : "#f87171";
            string pctColor = att >= 75.0f ? "var(--green)" : att >= 50.0f ? "var(--amber)"
                                                                           : "var(--red)";
            string badge = att >= 75.0f
                               ? "<span class='badge badge-eligible'>&#10003; Eligible</span>"
                               : "<span class='badge badge-not'>&#10007; Not Eligible</span>";
            string color = colors[i % 6];
            string initial = s->getName().empty() ? "?" : string(1, (char)toupper(s->getName()[0]));
            string tid = s->getManagedBy().empty() ? "&mdash;" : s->getManagedBy();

            f << "          <tr data-name=\"" << s->getName() << "\" data-id=\"" << s->getId() << "\" data-dept=\"" << s->getDepartment() << "\">\n";
            f << "            <td style=\"color:var(--dim)\">" << (i + 1) << "</td>\n";
            f << "            <td><div class=\"name-cell\">"
              << "<div class=\"avatar\" style=\"background:" << color << "20;color:" << color << "\">" << initial << "</div>"
              << "<div><div class=\"name-bright\">" << s->getName() << "</div>"
              << "<div style=\"font-size:11px;color:var(--dim)\">" << s->getId() << "</div></div>"
              << "</div></td>\n";
            f << "            <td>" << s->getRollNo() << "</td>\n";
            f << "            <td>" << s->getDepartment() << "</td>\n";
            f << "            <td>" << s->getSemester() << "</td>\n";
            f << "            <td style=\"color:var(--dim);font-size:12px\">" << tid << "</td>\n";
            f << "            <td><div class=\"bar-cell\">"
              << "<div class=\"bar-track\"><div class=\"bar-fill\" style=\"width:" << fixed << setprecision(1) << att << "%;background:" << barColor << "\"></div></div>"
              << "<span class=\"bar-pct\" style=\"color:" << pctColor << "\">" << att << "%</span>"
              << "</div></td>\n";
            f << "            <td>" << badge << "</td>\n";
            f << "          </tr>\n";
            i++;
        }
    }
    f << "        </tbody>\n      </table></div>\n    </div>\n";

    // Report Section
    f << "    <div id=\"report\">\n";
    f << "      <div class=\"sec-title\">Attendance Report</div>\n";
    f << "      <div class=\"table-wrap\"><table>\n";
    f << "        <thead><tr><th>Student</th><th>Present</th><th>Total</th><th>Percentage</th><th>Eligibility</th></tr></thead>\n";
    f << "        <tbody>\n";

    if (students.empty())
    {
        f << "          <tr class=\"empty-row\"><td colspan=\"5\">No records.</td></tr>\n";
    }
    else
    {
        for (Student *s : students)
        {
            float att = s->getAttendance();
            string pctColor = att >= 75.0f ? "var(--green)" : att >= 50.0f ? "var(--amber)"
                                                                           : "var(--red)";
            string badge = att >= 75.0f
                               ? "<span class='badge badge-eligible'>Eligible</span>"
                               : "<span class='badge badge-not'>Not Eligible</span>";
            f << "          <tr>\n";
            f << "            <td class=\"name-bright\">" << s->getName() << "</td>\n";
            f << "            <td>" << s->getPresentClasses() << "</td>\n";
            f << "            <td>" << TOTAL_CLASSES << "</td>\n";
            f << "            <td style=\"color:" << pctColor << ";font-weight:500\">" << fixed << setprecision(1) << att << "%</td>\n";
            f << "            <td>" << badge << "</td>\n";
            f << "          </tr>\n";
        }
    }
    f << "        </tbody>\n      </table></div>\n    </div>\n";

    // Footer + JS
    f << "    <div class=\"footer\">AttendEase &mdash; C++ OOP Project &mdash; Student Dashboard</div>\n";
    f << "  </main>\n</div>\n";
    f << "<script>\n";
    f << "function filterStudents(){\n";
    f << "  const q=document.getElementById('searchInput').value.toLowerCase();\n";
    f << "  document.querySelectorAll('#studentBody tr[data-name]').forEach(row=>{\n";
    f << "    const n=(row.dataset.name||'').toLowerCase();\n";
    f << "    const i=(row.dataset.id||'').toLowerCase();\n";
    f << "    const d=(row.dataset.dept||'').toLowerCase();\n";
    f << "    row.style.display=(n.includes(q)||i.includes(q)||d.includes(q))?'':'none';\n";
    f << "  });\n}\n";
    f << "</script>\n</body>\n</html>\n";

    f.close();
    cout << "  HTML dashboard saved as: " << htmlFile << "\n";
}

// ═══════════════════════════════════════════════════════════════
//  StudentMenu
// ═══════════════════════════════════════════════════════════════
StudentMenu::StudentMenu(AttendanceManager &mgr, string id)
    : manager(mgr), studentId(id) {}

void StudentMenu::viewMyRecord() const
{
    try
    {
        Student *s = manager.findStudent(studentId);
        s->display();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void StudentMenu::viewMyAttendance() const
{
    try
    {
        Student *s = manager.findStudent(studentId);
        cout << "\n  ===== MY ATTENDANCE =====\n";
        cout << "  Name       : " << s->getName() << "\n";
        cout << "  Classes    : " << s->getPresentClasses() << "/" << TOTAL_CLASSES << "\n";
        cout << fixed << setprecision(2);
        cout << "  Percentage : " << s->getAttendance() << "%\n";
        s->checkEligibility();
        if (s->getAttendance() < MIN_ATTENDANCE)
        {
            int needed = (int)((MIN_ATTENDANCE / 100.0f * TOTAL_CLASSES) - s->getPresentClasses()) + 1;
            if (needed > 0)
                cout << "  [!] You need " << needed << " more class(es) to reach 75%.\n";
        }
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void StudentMenu::run()
{
    bool running = true;
    while (running)
    {
        cout << "\n  ============================================\n";
        cout << "           STUDENT PORTAL                   \n";
        cout << "  ============================================\n";
        cout << "   1. View My Record\n";
        cout << "   2. View My Attendance\n";
        cout << "   0. Logout\n";
        cout << "  ============================================\n";

        int choice;
        cout << "  Choice: ";
        while (!(cin >> choice) || choice < 0 || choice > 2)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Invalid. Try again: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
        case 1:
            viewMyRecord();
            break;
        case 2:
            viewMyAttendance();
            break;
        case 0:
            running = false;
            cout << "  Logged out.\n";
            break;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//  TeacherMenu helpers
// ═══════════════════════════════════════════════════════════════
TeacherMenu::TeacherMenu(AttendanceManager &mgr, string tid)
    : manager(mgr), teacherId(tid) {}

int TeacherMenu::getInt(string prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= min && value <= max)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid. Enter " << min << "-" << max << ": ";
    }
}

string TeacherMenu::getString(string prompt)
{
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void TeacherMenu::menuAddStudent()
{
    cout << "\n  ----- ADD STUDENT -----\n";
    string name = getString("  Name       : ");
    string id = getString("  ID         : ");
    int roll = getInt("  Roll No    : ", 1, 99999);
    string dept = getString("  Department : ");
    string sem = getString("  Semester   : ");
    try
    {
        // FIX: credentials are now created inside addStudent() automatically;
        //      no manual auth call needed here.
        Student *s = new Student(name, id, roll, dept, sem, teacherId);
        manager.addStudent(s);
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
    catch (AttendanceException<int> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
}

void TeacherMenu::menuMarkAttendance()
{
    cout << "\n  ----- MARK ATTENDANCE -----\n";
    string id = getString("  Student ID : ");
    int ch = getInt("  1=Present  2=Absent\n  Choice     : ", 1, 2);
    try
    {
        manager.markAttendance(id, ch == 1);
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
}

void TeacherMenu::menuViewStudents() const
{
    manager.displayAllStudents();
}

void TeacherMenu::menuSearchStudent() const
{
    cout << "\n  ----- SEARCH STUDENT -----\n";
    string id;
    cout << "  Student ID : ";
    getline(cin, id);
    try
    {
        manager.findStudent(id)->display();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void TeacherMenu::menuRemoveStudent()
{
    cout << "\n  ----- REMOVE STUDENT -----\n";
    string id = getString("  Student ID : ");
    try
    {
        // FIX: credentials are now removed inside removeStudent() automatically.
        manager.removeStudent(id);
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void TeacherMenu::menuViewReport() const
{
    manager.showAttendanceReport();
}

void TeacherMenu::menuUpdateStudent()
{
    cout << "\n  ----- UPDATE STUDENT -----\n";
    string id = getString("  Student ID : ");
    try
    {
        Student *s = manager.findStudent(id);
        s->display();
        cout << "\n  What to update?\n";
        cout << "  1. Set Present Classes manually\n";
        cout << "  2. Back\n";
        int ch = getInt("  Choice: ", 1, 2);
        if (ch == 1)
        {
            int p = getInt("  Present Classes (0-" + to_string(TOTAL_CLASSES) + "): ", 0, TOTAL_CLASSES);
            s->setPresentClasses(p);
            cout << "  Updated.\n";
        }
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void TeacherMenu::run()
{
    bool running = true;
    while (running)
    {
        cout << "\n  ============================================\n";
        cout << "           TEACHER PORTAL                   \n";
        cout << "  ============================================\n";
        cout << "   1. Add Student\n";
        cout << "   2. Mark Attendance\n";
        cout << "   3. View All Students\n";
        cout << "   4. Search Student\n";
        cout << "   5. Remove Student\n";
        cout << "   6. Update Student Record\n";
        cout << "   7. View Attendance Report\n";
        cout << "   0. Logout\n";
        cout << "  ============================================\n";

        int choice = getInt("  Choice: ", 0, 7);
        switch (choice)
        {
        case 1:
            menuAddStudent();
            break;
        case 2:
            menuMarkAttendance();
            break;
        case 3:
            menuViewStudents();
            break;
        case 4:
            menuSearchStudent();
            break;
        case 5:
            menuRemoveStudent();
            break;
        case 6:
            menuUpdateStudent();
            break;
        case 7:
            menuViewReport();
            break;
        case 0:
            running = false;
            cout << "  Logged out.\n";
            break;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//  AdminMenu
// ═══════════════════════════════════════════════════════════════
AdminMenu::AdminMenu(AttendanceManager &mgr, AuthSystem &a)
    : manager(mgr), auth(a) {}

int AdminMenu::getInt(string prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= min && value <= max)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid. Enter " << min << "-" << max << ": ";
    }
}

string AdminMenu::getString(string prompt)
{
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void AdminMenu::menuAddTeacher()
{
    cout << "\n  ----- ADD TEACHER -----\n";
    string name = getString("  Name        : ");
    string id = getString("  ID          : ");
    string subj = getString("  Subject     : ");
    string desig = getString("  Designation : ");
    string uname = getString("  Username    : ");
    string pass = getString("  Password    : ");
    try
    {
        manager.addTeacher(new Teacher(name, id, subj, desig, uname, pass));
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
}

void AdminMenu::menuRemoveTeacher()
{
    cout << "\n  ----- REMOVE TEACHER -----\n";
    string id = getString("  Teacher ID : ");
    try
    {
        manager.removeTeacher(id);
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void AdminMenu::menuViewTeachers() const
{
    manager.displayAllTeachers();
}

void AdminMenu::menuViewAllRecords() const
{
    manager.displayAllPersons();
}

void AdminMenu::menuFileOps()
{
    cout << "\n  1. Save Data\n  2. Load Data\n";
    int ch = getInt("  Choice: ", 1, 2);
    try
    {
        if (ch == 1)
            manager.saveToFile();
        else
            manager.loadFromFile();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void AdminMenu::menuAddStudent()
{
    cout << "\n  ----- ADD STUDENT (Admin) -----\n";
    string name = getString("  Name       : ");
    string id = getString("  ID         : ");
    int roll = getInt("  Roll No    : ", 1, 99999);
    string dept = getString("  Department : ");
    string sem = getString("  Semester   : ");
    string tid = getString("  Teacher ID : ");
    try
    {
        // FIX: removed the manual auth.addStudentCredential() call that was
        //      here before. addStudent() now handles it internally, so doing
        //      it here as well would register the credential twice (harmless
        //      but redundant and inconsistent with the Teacher path).
        manager.addStudent(new Student(name, id, roll, dept, sem, tid));
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
    catch (AttendanceException<int> &e)
    {
        cout << "  Error: " << e.what() << " [" << e.data() << "]\n";
    }
}

void AdminMenu::menuRemoveStudent()
{
    cout << "\n  ----- REMOVE STUDENT (Admin) -----\n";
    string id = getString("  Student ID : ");
    try
    {
        // FIX: removed the manual auth.removeStudentCredential() call that
        //      was here before. removeStudent() handles it internally now.
        manager.removeStudent(id);
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  Error: " << e.what() << "\n";
    }
}

void AdminMenu::run()
{
    bool running = true;
    while (running)
    {
        cout << "\n  ============================================\n";
        cout << "           ADMIN PORTAL                     \n";
        cout << "  ============================================\n";
        cout << "   1. Add Teacher\n";
        cout << "   2. Remove Teacher\n";
        cout << "   3. View All Teachers\n";
        cout << "   4. View All Records\n";
        cout << "   5. Add Student\n";
        cout << "   6. Remove Student\n";
        cout << "   7. File Operations\n";
        cout << "   0. Logout\n";
        cout << "  ============================================\n";

        int choice = getInt("  Choice: ", 0, 7);
        switch (choice)
        {
        case 1:
            menuAddTeacher();
            break;
        case 2:
            menuRemoveTeacher();
            break;
        case 3:
            menuViewTeachers();
            break;
        case 4:
            menuViewAllRecords();
            break;
        case 5:
            menuAddStudent();
            break;
        case 6:
            menuRemoveStudent();
            break;
        case 7:
            menuFileOps();
            break;
        case 0:
            running = false;
            cout << "  Logged out.\n";
            break;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
//  MenuSystem (Login + Main)
// ═══════════════════════════════════════════════════════════════
MenuSystem::MenuSystem(AttendanceManager &mgr, AuthSystem &a)
    : manager(mgr), auth(a) {}

void MenuSystem::printLine() const
{
    cout << "  ============================================\n";
}

void MenuSystem::printHeader(string title) const
{
    printLine();
    cout << "  " << title << "\n";
    printLine();
}

void MenuSystem::pause() const
{
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int MenuSystem::getInt(string prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= min && value <= max)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid. Enter " << min << "-" << max << ": ";
    }
}

string MenuSystem::getString(string prompt)
{
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void MenuSystem::loginAsAdmin()
{
    printHeader("ADMIN LOGIN");
    string uname = getString("  Username : ");
    string pass = getString("  Password : ");
    try
    {
        auth.authenticateAdmin(uname, pass);
        cout << "  [+] Admin login successful!\n";
        pause();
        AdminMenu adminMenu(manager, auth);
        adminMenu.run();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  [!] " << e.what() << "\n";
    }
}

void MenuSystem::loginAsTeacher()
{
    printHeader("TEACHER LOGIN");
    string id = getString("  Teacher ID : ");
    string pass = getString("  Password   : ");

    try
    {
        Teacher *t = manager.findTeacher(id);
        if (t->getPassword() != pass)
            throw AttendanceException<string>("Incorrect password!", id);
        cout << "  [+] Welcome, " << t->getName() << "!\n";
        pause();
        TeacherMenu tMenu(manager, id);
        tMenu.run();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  [!] " << e.what() << "\n";
    }
}

void MenuSystem::loginAsStudent()
{
    printHeader("STUDENT LOGIN");
    string id = getString("  Student ID : ");
    string pass = getString("  Password   : ");

    try
    {
        auth.authenticateStudent(id, pass);
        Student *s = manager.findStudent(id);
        cout << "  [+] Welcome, " << s->getName() << "!\n";
        pause();
        StudentMenu sMenu(manager, id);
        sMenu.run();
    }
    catch (AttendanceException<string> &e)
    {
        cout << "  [!] " << e.what() << "\n";
    }
}

void MenuSystem::showMainMenu() const
{
    cout << "\n";
    cout << "  ============================================\n";
    cout << "      ATTENDANCE MANAGEMENT SYSTEM           \n";
    cout << "  ============================================\n";
    cout << "   1. Login as Admin\n";
    cout << "   2. Login as Teacher\n";
    cout << "   3. Login as Student\n";
    cout << "   0. Exit\n";
    cout << "  ============================================\n";
}

void MenuSystem::run()
{
    // FIX: wire the auth system into the manager right at startup so that
    //      every subsequent addStudent / removeStudent / loadFromFile call
    //      automatically keeps credentials in sync.
    manager.setAuthSystem(&auth);

    manager.loadFromFile();

    // Add default teacher if none exist (for demo)
    if (manager.getTeacherCount() == 0)
    {
        try
        {
            manager.addTeacher(new Teacher("Dr. Ahmad", "T001", "OOP", "Professor", "ahmad", "pass123"));
        }
        catch (...)
        {
        }
    }

    bool running = true;
    while (running)
    {
        showMainMenu();
        int choice = getInt("  Choice: ", 0, 3);
        switch (choice)
        {
        case 1:
            loginAsAdmin();
            break;
        case 2:
            loginAsTeacher();
            break;
        case 3:
            loginAsStudent();
            break;
        case 0:
            try
            {
                manager.saveToFile();
            }
            catch (...)
            {
            }
            cout << "\n  Goodbye! Data saved.\n\n";
            running = false;
            break;
        }
    }
}