#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <clocale>
using namespace std;

namespace Faculty
{

    class GradeError : public exception
    {
    private:
        string message;
        int value;

    public:
        GradeError(int v) : value(v)
        {
            message = "оценка " + to_string(v) + " вне диапазона 0..10";
        }

        const char *what() const noexcept override
        {
            return message.c_str();
        }

        const char *TO_GET_Type() const
        {
            return "Faculty::GradeError";
        }
        int TO_GET_Value() const { return value; }
    };

    class Student
    {
    public:
        static const int SIZE = 5;

    private:
        string name;
        int course;
        int grades[SIZE];

    public:
        Student(const string &n, int c) : name(n), course(c)
        {
            for (int i = 0; i < SIZE; i++)
            {
                grades[i] = 0;
            }
        }

        void TO_SET_Grade(int i, int value)
        {
            if (i < 0 || i >= SIZE)
            {

                throw out_of_range("индекс " + to_string(i) + " вне диапазона 0.." + to_string(SIZE - 1));
            }
            if (value < 0 || value > 10)
            {
                throw GradeError(value);
            }
            grades[i] = value;
        }

        void TO_SHOW() const
        {
            cout << "Faculty::Student " << name << ", курс "
                 << course << ", оценки: ";
            for (int i = 0; i < SIZE; i++)
            {
                cout << grades[i] << " ";
            }
            cout << endl;
        }
    };

}

namespace Dormitory
{

    const int MAX_ROOM = 50;

    class RoomError : public exception
    {
    private:
        string message;
        int room;

    public:
        RoomError(int r) : room(r)
        {
            message = "комнаты " + to_string(r) + " нет (допустимо 1.." + to_string(MAX_ROOM) + ")";
        }

        const char *what() const noexcept override
        {
            return message.c_str();
        }

        const char *TO_GET_Type() const
        {
            return "Dormitory::RoomError";
        }
        int TO_GET_Room() const { return room; }
    };

    class Student
    {
    public:
        static const int SIZE = 5;

    private:
        string name;
        int room;

    public:
        Student(const string &n, int r) : name(n), room(r)
        {
            if (n.empty())
            {
                throw invalid_argument("имя студента не может "
                                       "быть пустым");
            }
            if (r < 1 || r > MAX_ROOM)
            {
                throw RoomError(r);
            }
        }

        void TO_MOVE(int newRoom)
        {
            if (newRoom < 1 || newRoom > MAX_ROOM)
            {
                throw RoomError(newRoom);
            }
            room = newRoom;
        }

        void TO_SHOW() const
        {
            cout << "Dormitory::Student " << name
                 << ", комната " << room << endl;
        }
    };

}

void report(const string &type, const exception &e)
{
    cout << "  Тип ошибки: " << type << endl;
    cout << "  Причина: " << e.what() << endl;
}

int main()
{
    system("chcp 65001");
    setlocale(LC_ALL, "ru_RU.UTF-8");

    cout << "=== 1. Пространства имён ===" << endl;

    Faculty::Student fs("Юрий", 2);
    Dormitory::Student ds("Юрий", 12);
    fs.TO_SHOW();
    ds.TO_SHOW();

    cout << endl
         << "=== 2. Стандартные исключения ===" << endl;
    try
    {
        fs.TO_SET_Grade(7, 5);
        cout << "  выполнено без ошибок" << endl;
    }
    catch (const out_of_range &e)
    {
        report("std::out_of_range", e);
    }

    try
    {
        Dormitory::Student bad("", 5);
        cout << "  выполнено без ошибок" << endl;
    }
    catch (const invalid_argument &e)
    {
        report("std::invalid_argument", e);
    }

    cout << endl
         << "=== 3. Пользовательские исключения ===" << endl;
    try
    {
        fs.TO_SET_Grade(2, 15);
    }
    catch (const Faculty::GradeError &e)
    {
        report(e.TO_GET_Type(), e);
        cout << "  Значение: " << e.TO_GET_Value() << endl;
    }

    try
    {
        ds.TO_MOVE(99);
    }
    catch (const Dormitory::RoomError &e)
    {
        report(e.TO_GET_Type(), e);
        cout << "  Значение: " << e.TO_GET_Room() << endl;
    }

    cout << endl
         << "=== 4. Раздельная обработка ===" << endl;
    for (int step = 1; step <= 4; step++)
    {
        cout << "Шаг " << step << ":" << endl;
        try
        {
            switch (step)
            {
            case 1:
                fs.TO_SET_Grade(9, 1);
                break;
            case 2:
                fs.TO_SET_Grade(1, -3);
                break;
            case 3:
                ds.TO_MOVE(0);
                break;
            case 4:
                ds.TO_MOVE(20);
                break;
            }
            cout << "  ошибок нет" << endl;
        }

        catch (const Faculty::GradeError &e)
        {
            cout << "  [catch Faculty]" << endl;
            report(e.TO_GET_Type(), e);
        }
        catch (const Dormitory::RoomError &e)
        {
            cout << "  [catch Dormitory]" << endl;
            report(e.TO_GET_Type(), e);
        }
        catch (const out_of_range &e)
        {
            cout << "  [catch std::out_of_range]" << endl;
            report("std::out_of_range", e);
        }
        catch (const exception &e)
        {
            cout << "  [catch std::exception]" << endl;
            report("std::exception", e);
        }
    }

    cout << endl
         << "Состояние после обработки:" << endl;
    fs.TO_SHOW();
    ds.TO_SHOW();

    return 0;
}