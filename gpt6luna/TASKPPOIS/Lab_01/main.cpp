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

class Student {
public:
    static const int SUBJECTS = 5;

private:
    std::string name;
    int course;
    int grades[SUBJECTS];

public:
    // Конструктор по умолчанию.
    Student() : name("Без имени"), course(1), grades{} {}

    // Конструктор с данными студента.
    Student(const std::string& studentName, int studentCourse,
            const int studentGrades[SUBJECTS])
        : name(studentName), course(studentCourse), grades{} {
        for (int i = 0; i < SUBJECTS; i++) {
            grades[i] = studentGrades[i];
        }
    }

    // Конструктор копирует поля и каждую оценку.
    Student(const Student& other)
        : name(other.name), course(other.course), grades{} {
        for (int i = 0; i < SUBJECTS; i++) {
            grades[i] = other.grades[i];
        }
    }

    // Оператор присваивания копирует все данные студента.
    Student& operator=(const Student& other) {
        if (this != &other) {
            name = other.name;
            course = other.course;
            for (int i = 0; i < SUBJECTS; i++) {
                grades[i] = other.grades[i];
            }
        }
        return *this;
    }

    std::string TO_GET_Name() const { return name; }
    int TO_GET_Course() const { return course; }
    int TO_GET_Grade(int index) const { return grades[index]; }

    void TO_SET_Name(const std::string& studentName) { name = studentName; }
    void TO_SET_Course(int studentCourse) { course = studentCourse; }
    void TO_SET_Grade(int index, int grade) { grades[index] = grade; }

    void TO_SHOW() const {
        std::cout << "Студент: " << name << ", курс: " << course
                  << ", оценки:";
        for (int i = 0; i < SUBJECTS; i++) {
            std::cout << ' ' << grades[i];
        }
        std::cout << '\n';
    }

    // + возвращает студента с оценками, усреднёнными для двух студентов.
    Student operator+(const Student& other) const {
        Student result(*this);
        result.name = name + " и " + other.name;
        for (int i = 0; i < SUBJECTS; i++) {
            result.grades[i] = (grades[i] + other.grades[i]) / 2;
        }
        return result;
    }

    // ++ переводит студента на следующий курс.
    Student& operator++() {
        course++;
        return *this;
    }
};

int main() {
    setRussianText();

    int grades[Student::SUBJECTS]{91, 85, 100, 78, 94};
    Student first;
    Student second("Анна", 2, grades);
    Student copy(second);

    first.TO_SET_Name("Иван");
    first.TO_SET_Course(1);
    first.TO_SET_Grade(0, 80);

    std::cout << "Первый студент:\n";
    first.TO_SHOW();
    std::cout << "Второй студент:\n";
    second.TO_SHOW();
    std::cout << "Копия второго студента:\n";
    copy.TO_SHOW();

    first = second;
    std::cout << "После присваивания:\n";
    first.TO_SHOW();

    std::cout << "Результат операции +:\n";
    (second + copy).TO_SHOW();

    ++second;
    std::cout << "После операции ++:\n";
    second.TO_SHOW();

    std::cout << "Имя через TO_GET: " << second.TO_GET_Name()
              << ", первая оценка: " << second.TO_GET_Grade(0) << '\n';
    return 0;
}
