// ============================================================
// Лабораторная работа №6
// Тема: «Пространства имён. Исключения»
// Предметная область: «Студент». Два пространства имён:
//   Faculty    — студент факультета (оценки),
//   Dormitory  — студент-жилец общежития (комната).
// В каждом определён класс с ОДИНАКОВЫМ именем Student.
// ============================================================

#include <iostream>    // cout, endl
#include <string>      // string, to_string
#include <exception>   // exception
#include <stdexcept>   // out_of_range, invalid_argument
#include <cstdlib>     // system()
#include <clocale>     // setlocale()
using namespace std;

const int SIZE = 5;    // число оценок

// ==============================================================
// Пространство имён Faculty
// ==============================================================
namespace Faculty {

    // ПОЛЬЗОВАТЕЛЬСКОЕ исключение: класс, наследуемый от
    // std::exception. Его можно перехватить и как GradeError,
    // и как std::exception.
    class GradeError : public exception {
    private:
        string message;   // текст причины
        int value;        // недопустимое значение оценки

    public:
        GradeError(int v) : value(v) {
            message = "оценка " + to_string(v)
                + " вне диапазона 0..10";
        }

        // Переопределяем what(): возвращает причину ошибки.
        // noexcept — метод сам не должен выбрасывать исключений.
        const char* what() const noexcept override {
            return message.c_str();
        }

        // Собственный метод: тип ошибки с указанием пространства.
        const char* TO_GET_Type() const {
            return "Faculty::GradeError";
        }
        int TO_GET_Value() const { return value; }
    };

    // Класс Student пространства Faculty.
    class Student {
    private:
        string name;
        int course;
        int grades[SIZE];

    public:
        Student(const string& n, int c) : name(n), course(c) {
            for (int i = 0; i < SIZE; i++) {
                grades[i] = 0;
            }
        }

        // Метод, который может выбросить два вида исключений.
        void TO_SET_Grade(int i, int value) {
            if (i < 0 || i >= SIZE) {
                // СТАНДАРТНОЕ исключение: сообщение задаём сами.
                throw out_of_range("индекс " + to_string(i)
                    + " вне диапазона 0.."
                    + to_string(SIZE - 1));
            }
            if (value < 0 || value > 10) {
                throw GradeError(value);   // пользовательское
            }
            grades[i] = value;   // изменение только после проверок
        }

        void TO_SHOW() const {
            cout << "Faculty::Student " << name << ", курс "
                << course << ", оценки: ";
            for (int i = 0; i < SIZE; i++) {
                cout << grades[i] << " ";
            }
            cout << endl;
        }
    };

}   // namespace Faculty

// ==============================================================
// Пространство имён Dormitory
// ==============================================================
namespace Dormitory {

    const int MAX_ROOM = 50;   // номера комнат: 1..MAX_ROOM

    // Пользовательское исключение второго пространства имён.
    class RoomError : public exception {
    private:
        string message;
        int room;

    public:
        RoomError(int r) : room(r) {
            message = "комнаты " + to_string(r)
                + " нет (допустимо 1.."
                + to_string(MAX_ROOM) + ")";
        }

        const char* what() const noexcept override {
            return message.c_str();
        }

        const char* TO_GET_Type() const {
            return "Dormitory::RoomError";
        }
        int TO_GET_Room() const { return room; }
    };

    // Класс с тем же именем Student, но в другом пространстве:
    // конфликта имён нет, это два разных типа.
    class Student {
    private:
        string name;
        int room;

    public:
        // Исключение в конструкторе: объект не будет создан.
        Student(const string& n, int r) : name(n), room(r) {
            if (n.empty()) {
                throw invalid_argument("имя студента не может "
                    "быть пустым");
            }
            if (r < 1 || r > MAX_ROOM) {
                throw RoomError(r);
            }
        }

        // Переселение: комната меняется только если номер верный.
        void TO_MOVE(int newRoom) {
            if (newRoom < 1 || newRoom > MAX_ROOM) {
                throw RoomError(newRoom);
            }
            room = newRoom;
        }

        void TO_SHOW() const {
            cout << "Dormitory::Student " << name
                << ", комната " << room << endl;
        }
    };

}   // namespace Dormitory

// Общая функция вывода: тип ошибки и причина. Параметр —
// ссылка на базовый std::exception, what() вызывается
// полиморфно (виртуальный метод).
void report(const string& type, const exception& e) {
    cout << "  Тип ошибки: " << type << endl;
    cout << "  Причина: " << e.what() << endl;
}

int main() {
    system("chcp 65001");                  // UTF-8 в консоли Windows
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // ----- 1. Два класса Student в разных пространствах -----
    cout << "=== 1. Пространства имён ===" << endl;
    // Полное (квалифицированное) имя снимает неоднозначность.
    Faculty::Student fs("Юрий", 2);
    Dormitory::Student ds("Юрий", 12);
    fs.TO_SHOW();
    ds.TO_SHOW();

    // ----- 2. Стандартные исключения -----
    cout << endl << "=== 2. Стандартные исключения ===" << endl;
    try {
        fs.TO_SET_Grade(7, 5);             // неверный индекс
        cout << "  выполнено без ошибок" << endl;
    }
    catch (const out_of_range& e) {        // по const-ссылке
        report("std::out_of_range", e);
    }

    try {
        Dormitory::Student bad("", 5);     // пустое имя
        cout << "  выполнено без ошибок" << endl;
    }
    catch (const invalid_argument& e) {
        report("std::invalid_argument", e);
    }

    // ----- 3. Пользовательские исключения -----
    cout << endl
        << "=== 3. Пользовательские исключения ===" << endl;
    try {
        fs.TO_SET_Grade(2, 15);            // неверная оценка
    }
    catch (const Faculty::GradeError& e) {
        report(e.TO_GET_Type(), e);
        cout << "  Значение: " << e.TO_GET_Value() << endl;
    }

    try {
        ds.TO_MOVE(99);                    // неверная комната
    }
    catch (const Dormitory::RoomError& e) {
        report(e.TO_GET_Type(), e);
        cout << "  Значение: " << e.TO_GET_Room() << endl;
    }

    // ----- 4. Раздельная обработка в одном try -----
    cout << endl << "=== 4. Раздельная обработка ===" << endl;
    for (int step = 1; step <= 4; step++) {
        cout << "Шаг " << step << ":" << endl;
        try {
            switch (step) {
            case 1: fs.TO_SET_Grade(9, 1); break;    // out_of_range
            case 2: fs.TO_SET_Grade(1, -3); break;   // GradeError
            case 3: ds.TO_MOVE(0); break;            // RoomError
            case 4: ds.TO_MOVE(20); break;           // без ошибки
            }
            cout << "  ошибок нет" << endl;
        }
        // Отдельный catch для каждого пространства имён.
        // Порядок важен: производные типы — раньше базового
        // exception, иначе его catch перехватил бы всё.
        catch (const Faculty::GradeError& e) {
            cout << "  [catch Faculty]" << endl;
            report(e.TO_GET_Type(), e);
        }
        catch (const Dormitory::RoomError& e) {
            cout << "  [catch Dormitory]" << endl;
            report(e.TO_GET_Type(), e);
        }
        catch (const out_of_range& e) {
            cout << "  [catch std::out_of_range]" << endl;
            report("std::out_of_range", e);
        }
        catch (const exception& e) {       // все остальные стандартные
            cout << "  [catch std::exception]" << endl;
            report("std::exception", e);
        }
    }

    // После ошибок данные остались корректными.
    cout << endl << "Состояние после обработки:" << endl;
    fs.TO_SHOW();
    ds.TO_SHOW();

    return 0;
}
