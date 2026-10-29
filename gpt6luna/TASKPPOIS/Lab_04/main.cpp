#include <clocale>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void setRussianText() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
#else
    std::setlocale(LC_ALL, "");
#endif
}

class CourseRecord;

class StudentRecord {
private:
    std::string name;
    int score;

public:
    StudentRecord(const std::string& studentName, int studentScore)
        : name(studentName), score(studentScore) {}

    // Эти функции и класс могут читать закрытые поля студента.
    friend void showStudent(const StudentRecord& student);
    friend void compareScore(const StudentRecord& student,
                             const CourseRecord& course);
    friend class RecordInspector;
};

class CourseRecord {
private:
    std::string title;
    int passScore;

public:
    CourseRecord(const std::string& courseTitle, int requiredScore)
        : title(courseTitle), passScore(requiredScore) {}

    // Эта функция-друг имеет доступ также к закрытым полям курса.
    friend void compareScore(const StudentRecord& student,
                             const CourseRecord& course);
};

void showStudent(const StudentRecord& student) {
    std::cout << "Студент " << student.name
              << ", оценка " << student.score << '\n';
}

// Функция-друг двух классов сравнивает оценку с проходным баллом.
void compareScore(const StudentRecord& student, const CourseRecord& course) {
    std::cout << "Курс \"" << course.title << "\": ";
    if (student.score >= course.passScore) {
        std::cout << "зачёт";
    } else {
        std::cout << "незачёт";
    }
    std::cout << '\n';
}

class RecordInspector {
public:
    // Дружественный класс может менять закрытую оценку.
    void addBonus(StudentRecord& student, int bonus) {
        student.score += bonus;
        if (student.score > 100) {
            student.score = 100;
        }
    }

    void show(const StudentRecord& student) {
        std::cout << "После добавления баллов: " << student.score << '\n';
    }
};

int main() {
    setRussianText();

    StudentRecord student("Мария", 86);
    CourseRecord course("Алгоритмы", 60);

    showStudent(student);
    compareScore(student, course);

    RecordInspector inspector;
    inspector.addBonus(student, 8);
    inspector.show(student);
    compareScore(student, course);
    return 0;
}
