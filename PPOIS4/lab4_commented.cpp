// ============================================================
// Лабораторная работа №4
// Тема: «Функции-друзья классов»
// Основа: класс Student из лабораторной работы №3
//         (динамический int[], ссылочное поле rating).
// ============================================================

#include <iostream>   // cout, endl, ostream
#include <string>     // string (название предмета)
#include <cstdlib>    // system()
#include <clocale>    // setlocale()
using namespace std;

const int SIZE = 5;   // размер массива оценок

// Предварительное объявление класса: Student упоминает Subject
// в объявлении друга, а полное описание Subject появится ниже.
class Subject;

// ==============================================================
// Класс Student
// ==============================================================
class Student {
private:
    int course;           // курс
    int group;            // группа
    int* grades;          // динамический массив оценок
    int& rating;          // ссылка на внешнюю переменную (рейтинг)
    static int alive;     // счётчик существующих объектов
    static int defaultRating;   // «заглушка» для конструктора по умолчанию

public:
    // Конструктор по умолчанию.
    Student() : rating(defaultRating) {
        course = 1;
        group = 0;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = 0;
        }
        alive++;
    }

    // Конструктор с параметрами (r передаётся по ссылке).
    Student(int c, int g, const int arr[], int& r) : rating(r) {
        course = c;
        group = g;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = arr[i];
        }
        alive++;
    }

    // Конструктор копирования: глубокое копирование массива.
    Student(const Student& other) : rating(other.rating) {
        course = other.course;
        group = other.group;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = other.grades[i];
        }
        alive++;
    }

    // Деструктор: освобождаем массив, выделенный через new[].
    ~Student() {
        delete[] grades;
        alive--;
    }

    // Оператор присваивания (правило трёх).
    Student& operator=(const Student& other) {
        if (this != &other) {
            delete[] grades;
            course = other.course;
            group = other.group;
            grades = new int[SIZE];
            for (int i = 0; i < SIZE; i++) {
                grades[i] = other.grades[i];
            }
            // rating не присваиваем: ссылку нельзя перепривязать.
        }
        return *this;
    }

    // Публичный метод вывода: работает только через открытый
    // интерфейс класса (внутри класса доступ есть ко всему).
    void TO_SHOW() const {
        cout << "Курс: " << course;
        cout << " | Группа: " << group;
        cout << " | Оценки: ";
        for (int i = 0; i < SIZE; i++) {
            cout << grades[i] << " ";
        }
        cout << "| Рейтинг: " << rating << endl;
    }

    int TO_GET_Course() const { return course; }

    int TO_GET_Sum() const {
        int sum = 0;
        for (int i = 0; i < SIZE; i++) {
            sum += grades[i];
        }
        return sum;
    }

    static int TO_GET_Alive() { return alive; }

    // ----------------------------------------------------------
    // ОБЪЯВЛЕНИЯ ДРУЖБЫ
    // Слово friend даёт функции или классу доступ к закрытым
    // (private) членам. Сами друзья членами класса НЕ являются,
    // поэтому место объявления (private/public) значения не имеет.
    // ----------------------------------------------------------

    // 1. Дружественная функция, работающая с одним классом.
    friend void TO_SHOW_PRIVATE(const Student& s);

    // Перегрузка оператора << как дружественной функции: слева
    // стоит ostream, поэтому оператор не может быть методом Student.
    friend ostream& operator<<(ostream& os, const Student& s);

    // 2. Дружественные функции, работающие сразу с двумя классами.
    // Они объявлены другом и здесь, и в классе Subject.
    friend bool TO_CHECK_PASS(const Student& s,
                              const Subject& sub);
    friend void TO_EXAM(Student& s, Subject& sub, int mark);

    // 3. Дружественный КЛАСС: все методы Deanery получают доступ
    // ко всем членам Student. Дружба односторонняя: Student к
    // закрытым членам Deanery доступа не получает.
    friend class Deanery;
};

// Определение статических полей (один раз, вне класса).
int Student::alive = 0;
int Student::defaultRating = 0;

// ==============================================================
// Класс Subject (учебный предмет) — второй класс для пункта 2
// ==============================================================
class Subject {
private:
    string name;      // название предмета
    int passMark;     // проходной балл
    int index;        // номер оценки в массиве оценок студента
    int examCount;    // сколько раз выставлялась оценка

public:
    Subject(const string& n, int p, int i)
        : name(n), passMark(p), index(i), examCount(0) {}

    // Открытый метод: без него examCount снаружи не прочитать.
    int TO_GET_ExamCount() const { return examCount; }

    // Те же функции объявлены другом и в этом классе, поэтому
    // получают доступ к закрытым полям ОБОИХ классов.
    friend bool TO_CHECK_PASS(const Student& s,
                              const Subject& sub);
    friend void TO_EXAM(Student& s, Subject& sub, int mark);
};

// ==============================================================
// Реализация дружественных функций (не члены классов:
// в определении слово friend и Student:: не пишутся)
// ==============================================================

// Прямой доступ к закрытым полям Student, включая массив и
// ссылку. Через открытый интерфейс так сделать нельзя.
void TO_SHOW_PRIVATE(const Student& s) {
    cout << "[friend] course = " << s.course
        << ", group = " << s.group << endl;
    cout << "[friend] grades = [";
    for (int i = 0; i < SIZE; i++) {
        cout << s.grades[i] << (i < SIZE - 1 ? ", " : "");
    }
    cout << "], rating = " << s.rating << endl;
}

// Оператор вывода: возвращаем ostream&, чтобы можно было
// писать цепочкой: cout << a << b.
ostream& operator<<(ostream& os, const Student& s) {
    os << "Student{курс=" << s.course << ", группа=" << s.group
        << ", оценки=[";
    for (int i = 0; i < SIZE; i++) {
        os << s.grades[i] << (i < SIZE - 1 ? " " : "");
    }
    os << "], рейтинг=" << s.rating << "}";
    return os;
}

// Читает закрытые поля двух классов сразу:
// grades у Student, name/passMark/index у Subject.
bool TO_CHECK_PASS(const Student& s, const Subject& sub) {
    int mark = s.grades[sub.index];
    bool ok = mark >= sub.passMark;
    cout << "  " << sub.name << " (оценка №" << sub.index + 1
        << "): " << mark << ", проходной " << sub.passMark
        << " -> " << (ok ? "сдан" : "не сдан") << endl;
    return ok;
}

// Изменяет закрытые поля двух классов: оценку у Student
// и счётчик examCount у Subject.
void TO_EXAM(Student& s, Subject& sub, int mark) {
    s.grades[sub.index] = mark;
    sub.examCount++;
    cout << "  Экзамен «" << sub.name << "»: оценка " << mark
        << " (экзаменов: " << sub.examCount << ")" << endl;
}

// ==============================================================
// Класс Deanery (деканат) — друг класса Student (пункт 3)
// ==============================================================
class Deanery {
private:
    int processed;    // сколько операций выполнил деканат
    // Student НЕ является другом Deanery, поэтому processed
    // из методов Student недоступен (дружба не симметрична).

public:
    Deanery() : processed(0) {}

    // Перевод на следующий курс: меняет закрытое поле course.
    void TO_PROMOTE(Student& s) {
        if (s.course < 5) {
            s.course++;
        }
        processed++;
        cout << "  Перевод: теперь курс " << s.course << endl;
    }

    // Запись в закрытую ссылку rating: меняется ВНЕШНЯЯ
    // переменная, к которой привязан студент.
    void TO_SET_Rating(Student& s, int r) {
        s.rating = r;
        processed++;
        cout << "  Рейтинг установлен: " << r << endl;
    }

    // Обнуление закрытого динамического массива оценок.
    void TO_RESET_Grades(Student& s) {
        for (int i = 0; i < SIZE; i++) {
            s.grades[i] = 0;
        }
        processed++;
        cout << "  Оценки обнулены" << endl;
    }

    // Чтение всех закрытых полей (сумма — через открытый метод).
    void TO_REPORT(const Student& s) const {
        cout << "  Отчёт: курс " << s.course << ", группа "
            << s.group << ", рейтинг " << s.rating
            << ", сумма оценок " << s.TO_GET_Sum() << endl;
    }

    int TO_GET_Processed() const { return processed; }
};

int main() {
    system("chcp 65001");                  // UTF-8 в консоли Windows
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int g[SIZE] = {8, 9, 7, 10, 9};
    int groupRating = 70;
    Student s(2, 29, g, groupRating);

    // ----- 1. Дружественная функция -----
    cout << "=== 1. Дружественная функция ===" << endl;
    cout << "Открытый метод TO_SHOW: ";
    s.TO_SHOW();
    TO_SHOW_PRIVATE(s);
    cout << "operator<<: " << s << endl;

    // ----- 2. Функция-друг двух классов -----
    cout << endl
        << "=== 2. Дружественная функция двух классов ==="
        << endl;
    Subject math("Математика", 6, 0);
    Subject phys("Физика", 8, 3);
    Subject chem("Химия", 8, 2);

    cout << "Проверка сдачи:" << endl;
    TO_CHECK_PASS(s, math);
    TO_CHECK_PASS(s, phys);
    TO_CHECK_PASS(s, chem);

    cout << "Пересдача химии:" << endl;
    TO_EXAM(s, chem, 9);
    TO_CHECK_PASS(s, chem);
    cout << "s: " << s << endl;
    cout << "Экзаменов по химии: "
        << chem.TO_GET_ExamCount() << endl;

    // ----- 3. Дружественный класс -----
    cout << endl << "=== 3. Дружественный класс ===" << endl;
    Deanery dean;
    cout << "До: " << s << endl;
    dean.TO_PROMOTE(s);
    dean.TO_SET_Rating(s, 95);
    cout << "groupRating = " << groupRating << endl;
    dean.TO_REPORT(s);
    dean.TO_RESET_Grades(s);
    cout << "После: " << s << endl;
    cout << "Операций деканата: "
        << dean.TO_GET_Processed() << endl;

    return 0;
}
