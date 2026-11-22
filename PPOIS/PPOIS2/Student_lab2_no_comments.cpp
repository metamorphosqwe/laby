 























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
    int grades[SIZE];  

     
     
     
    static int Round(double value)
    {
        return (int)(value + 0.5);
    }

public:
     
     
     

     
    Student()
    {
        course = 1;
        group = 0;
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = 0;
        }
    }

     
    Student(int c, int g, const int arr[])
    {
        course = c;
        group = g;
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = arr[i];
        }
    }

     
    Student(const Student &other)
    {
        course = other.course;
        group = other.group;
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = other.grades[i];
        }
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
        cout << endl;
    }

     
     
     

    int TO_GET_Course() const { return course; }
    int TO_GET_Group() const { return group; }
    void TO_SET_Course(int c) { course = c; }
    void TO_SET_Group(int g) { group = g; }

     
    int TO_GET_Sum() const
    {
        int sum = 0;
        for (int i = 0; i < SIZE; i++)
        {
            sum += grades[i];
        }
        return sum;
    }

     
     
     
     
     
     
     

     
    void TO_SET_Grade(int i, int value)
    {
        if (i >= 0 && i < SIZE)
        {
            grades[i] = value;
        }
    }

     
    void TO_SET_Grade(int value)
    {
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = value;
        }
    }

     
     
     
     
     
     
     
     
     
    void TO_SET_Grade(int i, double value)
    {
        if (i >= 0 && i < SIZE)
        {
            grades[i] = Round(value);
        }
    }

     
     
     
     
     
     
     
     
    void TO_SET_Grade(double value)
    {
        int rounded = Round(value);
        for (int i = 0; i < SIZE; i++)
        {
            grades[i] = rounded;
        }
    }

     
     
     

     
     
     
     
     
     
     
    int TO_GET_Grade(int i) const
    {
        if (i < 0 || i >= SIZE)
            return -1;
        return grades[i];
    }

     
     
     
     
     
     
     
     
     
    int &TO_GET_Grade(int i)
    {
        static int stub = 0;
        if (i < 0 || i >= SIZE)
        {
            stub = 0;
            return stub;
        }
        return grades[i];
    }

     
     
     
     
     
     
     
    int TO_COMPARE(Student &other) const
    {
        cout << "[TO_COMPARE(Student&)]";
        if (TO_GET_Sum() > other.TO_GET_Sum())
            return 1;
        if (TO_GET_Sum() < other.TO_GET_Sum())
            return -1;
        return 0;
    }

     
     
     
    int TO_COMPARE(const Student &other) const
    {
        cout << "[TO_COMPARE(const Student&)]";
        if (TO_GET_Sum() > other.TO_GET_Sum())
            return 1;
        if (TO_GET_Sum() < other.TO_GET_Sum())
            return -1;
        return 0;
    }

     
     
     
     

     
     
     
     
     
     
     
     
     
     
     
     
     
     
};

int main()
{
     
    system("chcp 65001");
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int g[Student::SIZE] = {8, 9, 7, 10, 9};
    Student s(2, 29, g);
    cout << "=== Исходный объект ===" << endl;
    s.TO_SHOW();

     
    cout << endl;
    cout << "=== 1. Разное количество параметров ===" << endl;

     
    s.TO_SET_Grade(0, 6);
    cout << "TO_SET_Grade(0, 6):   ";
    s.TO_SHOW();

     
    s.TO_SET_Grade(5);
    cout << "TO_SET_Grade(5):      ";
    s.TO_SHOW();

     
    cout << endl
         << "=== 2. Разные типы параметров ===" << endl;

     
    s.TO_SET_Grade(1, 7);
    cout << "TO_SET_Grade(1, 7):   ";
    s.TO_SHOW();

     
     
    s.TO_SET_Grade(2, 7.6);
    cout << "TO_SET_Grade(2, 7.6): ";
    s.TO_SHOW();

     
    cout << endl
         << "=== 3. Число и тип параметров ===" << endl;

     
     
    s.TO_SET_Grade(9.4);
    cout << "TO_SET_Grade(9.4):    ";
    s.TO_SHOW();

     
    cout << endl
         << "=== 4. Перегрузка с const ===" << endl;

     
    int h[Student::SIZE] = {6, 7, 8, 9, 10};
    const Student cs(3, 15, h);  
    Student s2(2, 29, g);        

     
    cout << "const-объект, оценка №2: ";
    cout << cs.TO_GET_Grade(1) << endl;
     

     
     
    s2.TO_GET_Grade(1) = 10;
    cout << "s2 после TO_GET_Grade(1) = 10: ";
    s2.TO_SHOW();

     
    int r;

     
    r = s2.TO_COMPARE(s);
    cout << " результат: " << r << endl;

     
    r = s2.TO_COMPARE(cs);
    cout << " результат: " << r << endl;

     
     
    r = s2.TO_COMPARE(Student(1, 1, g));
    cout << " результат: " << r << endl;

     
     
     
    system("pause");
    return 0;
}
