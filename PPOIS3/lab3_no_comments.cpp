#include <iostream>
#include <cstdlib>
#include <clocale>
using namespace std;

class Student
{
public:
    static const int SIZE = 5;

private:
    int course;
    int group;

    int *grades;

    int &rating;

    static int alive;

    static int defaultRating;

public:
    Student() : rating(defaultRating)
    {
        course = 1;
        group = 0;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = 0;
        }
        alive++;
        cout << "  [Конструктор по умолчанию] объектов: "
             << alive << endl;
    }

    Student(int c, int g, const int arr[], int &r) : rating(r)
    {
        course = c;
        group = g;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = arr[i];
        }
        alive++;
        cout << "  [Конструктор с параметрами] объектов: "
             << alive << endl;
    }

    Student(const Student &other) : rating(other.rating)
    {
        course = other.course;
        group = other.group;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = other.grades[i];
        }
        alive++;
        cout << "  [Конструктор копирования] объектов: "
             << alive << endl;
    }

    ~Student()
    {
        delete[] grades;
        alive--;
        cout << "  [Деструктор] объектов: " << alive << endl;
    }

    Student &operator=(const Student &other)
    {

        if (this != &other)
        {
            delete[] grades;
            course = other.course;
            group = other.group;
            grades = new int[SIZE];
            for (int i = 0; i < SIZE; i++)
            {
                grades[i] = other.grades[i];
            }
        }
        cout << "  [Оператор =]" << endl;
        return *this;
    }

    void TO_SHOW() const
    {
        cout << "Курс: " << course;
        cout << " | Группа: " << group;
        cout << " | Оценки: ";
        for (int i = 0; i < SIZE; i++)
        {
            cout << grades[i] << " ";
        }
        cout << "| Рейтинг: " << rating << endl;
    }

    int TO_GET_Course() const { return course; }
    int TO_GET_Group() const { return group; }

    int TO_GET_Sum() const
    {
        int sum = 0;
        for (int i = 0; i < SIZE; i++)
        {
            sum += grades[i];
        }
        return sum;
    }

    int TO_GET_Rating() const { return rating; }

    void TO_SET_Rating(int r) { rating = r; }

    int *TO_GET_Array() { return grades; }

    int (&TO_GET_ArrayRef()) [SIZE]
    {
        return reinterpret_cast<int (&)[SIZE]>(*grades);
    }

    int &TO_GET_Element(int i)
    {

        static int stub = 0;
        if (i < 0 || i >= SIZE)
        {
            stub = 0;
            return stub;
        }
        return grades[i];
    }

    static int TO_GET_Alive() { return alive; }
};

int Student::alive = 0;
int Student::defaultRating = 0;

void changeByValue(Student s)
{
    s.TO_GET_Element(0) = 1;
    cout << "  внутри функции (копия): ";
    s.TO_SHOW();
}

void changeByRef(Student &s)
{
    s.TO_GET_Element(0) = 1;
    cout << "  внутри функции (оригинал): ";
    s.TO_SHOW();
}

Student &bestByRef(Student &a, Student &b)
{
    return (a.TO_GET_Sum() >= b.TO_GET_Sum()) ? a : b;
}

Student bestByValue(Student &a, Student &b)
{
    return (a.TO_GET_Sum() >= b.TO_GET_Sum()) ? a : b;
}

int main()
{
    system("chcp 65001");
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int g[Student::SIZE] = {8, 9, 7, 10, 9};
    int groupRating = 70;

    cout << "=== 1. Динамический массив ===" << endl;
    {

        Student s1;
        Student s2(2, 29, g, groupRating);
        Student s3(s2);
        cout << "s1: ";
        s1.TO_SHOW();
        cout << "s2: ";
        s2.TO_SHOW();
        cout << "s3: ";
        s3.TO_SHOW();

        cout << endl
             << "Изменяем оценку №1 у s3 (копия):" << endl;
        s3.TO_GET_Element(0) = 4;
        cout << "s2: ";
        s2.TO_SHOW();
        cout << "s3: ";
        s3.TO_SHOW();

        cout << endl
             << "Присваивание s1 = s2:" << endl;
        s1 = s2;
        cout << "s1: ";
        s1.TO_SHOW();
        s1.TO_GET_Element(4) = 5;
        cout << "s1 после изменения: ";
        s1.TO_SHOW();
        cout << "s2 не изменился:    ";
        s2.TO_SHOW();

        cout << endl
             << "Самоприсваивание s1 = s1:" << endl;
        s1 = s1;
        cout << "s1: ";
        s1.TO_SHOW();
        cout << endl
             << "Выход из блока (освобождение памяти):"
             << endl;
    }
    cout << "Живых объектов: " << Student::TO_GET_Alive() << endl;

    cout << endl
         << "=== 2. Ссылочная переменная ===" << endl;
    Student a(2, 29, g, groupRating);
    Student b(a);
    cout << "groupRating = " << groupRating << endl;
    cout << "a: ";
    a.TO_SHOW();

    groupRating = 85;
    cout << "groupRating = 85" << endl;
    cout << "a: ";
    a.TO_SHOW();
    cout << "b: ";
    b.TO_SHOW();

    a.TO_SET_Rating(90);
    cout << "a.TO_SET_Rating(90), groupRating = "
         << groupRating << endl;
    cout << "b: ";
    b.TO_SHOW();

    int otherRating = 50;
    int c[Student::SIZE] = {5, 5, 5, 5, 5};
    Student d(1, 15, c, otherRating);
    cout << endl
         << "Присваивание a = d (ссылка не перепривязывается):"
         << endl;
    a = d;
    cout << "a: ";
    a.TO_SHOW();
    cout << "groupRating = " << groupRating
         << ", otherRating = " << otherRating << endl;

    cout << endl
         << "=== 3. Передача и возврат ===" << endl;
    cout << "Передача по значению:" << endl;
    changeByValue(b);
    cout << "  после вызова: ";
    b.TO_SHOW();
    cout << "Передача по ссылке:" << endl;
    changeByRef(b);
    cout << "  после вызова: ";
    b.TO_SHOW();

    cout << endl
         << "Возврат по ссылке:" << endl;
    Student &best = bestByRef(a, b);
    best.TO_GET_Element(1) = 10;
    cout << "a: ";
    a.TO_SHOW();
    cout << "b: ";
    b.TO_SHOW();
    cout << "Возврат по значению:" << endl;
    Student copy = bestByValue(a, b);
    copy.TO_GET_Element(1) = 0;
    cout << "copy: ";
    copy.TO_SHOW();
    cout << "b:    ";
    b.TO_SHOW();

    cout << endl
         << "=== 4. Способы возврата массива ==="
         << endl;

    int *p = b.TO_GET_Array();
    p[2] = 6;
    cout << "1) указатель, p[2] = 6: ";
    b.TO_SHOW();
    cout << "   sizeof(p) = " << sizeof(p) << endl;

    int (&r)[Student::SIZE] = b.TO_GET_ArrayRef();
    cout << "2) ссылка на массив: ";
    for (int x : r)
        cout << x << " ";
    cout << endl;
    r[3] = 7;
    cout << "   после r[3] = 7: ";
    b.TO_SHOW();
    cout << "   sizeof(r) = " << sizeof(r)
         << " (" << Student::SIZE << " элементов)" << endl;

    int &e = b.TO_GET_Element(4);
    e = 9;
    cout << "3) ссылка на элемент, e = 9: ";
    b.TO_SHOW();
    b.TO_GET_Element(0)++;
    cout << "   TO_GET_Element(0)++: ";
    b.TO_SHOW();

    cout << endl
         << "Выход из main (освобождение памяти):" << endl;

    system("pause");
    return 0;
}
