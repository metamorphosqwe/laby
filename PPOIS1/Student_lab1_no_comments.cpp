 












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

     
     
     
    int TO_GET_Grade(int i) const
    {
        if (i < 0 || i >= SIZE)
            return -1;
        return grades[i];
    }

     
     
     

     
    void TO_SET_Course(int c) { course = c; }

     
    void TO_SET_Group(int g) { group = g; }

     
     
    void TO_SET_Grade(int i, int value)
    {
        if (i >= 0 && i < SIZE)
        {
            grades[i] = value;
        }
    }

     
     
     

     
     
     
     
     
     
    Student &operator=(const Student &other)
    {
        if (this != &other)
        {
            course = other.course;
            group = other.group;
            for (int i = 0; i < SIZE; i++)
            {
                grades[i] = other.grades[i];
            }
        }
        return *this;
    }

     
     
     
     
     
     
     
     
    Student operator+(const Student &other) const
    {
        Student result;
        if (course > other.course)
        {
            result.course = course;
        }
        else
        {
            result.course = other.course;
        }
        result.group = group;
        for (int i = 0; i < SIZE; i++)
        {
            result.grades[i] = grades[i] + other.grades[i];
        }
        return result;
    }

     
     
     
     
     
    Student &operator++()
    {
        course++;
        return *this;
    }
};

int main()
{
     
    system("chcp 65001");
    setlocale(LC_ALL, "ru_RU.UTF-8");

     
    cout << "=== Создание объектов ===" << endl;

     
    Student s1;
    cout << "Объект 1 (по умолчанию): ";
    s1.TO_SHOW();

     
    int g[Student::SIZE] = {8, 9, 7, 10, 9};
    Student s2(2, 29, g);
    cout << "Объект 2 (с параметрами): ";
    s2.TO_SHOW();

     
    Student s3(s2);
    cout << "Объект 3 (копия объекта 2): ";
    s3.TO_SHOW();

     
    cout << endl
         << "=== Методы TO_SET ===" << endl;

     
    s1.TO_SET_Course(1);
    s1.TO_SET_Group(15);
    int newGrades[Student::SIZE] = {6, 7, 5, 8, 6};
    for (int i = 0; i < Student::SIZE; i++)
    {
        s1.TO_SET_Grade(i, newGrades[i]);
    }
    cout << "Объект 1 после TO_SET: ";
    s1.TO_SHOW();

     
     
    s3.TO_SET_Grade(4, 5);
    cout << "Объект 3 после TO_SET_Grade(4, 5): ";
    s3.TO_SHOW();
    cout << "Объект 2 (не изменился): ";
    s2.TO_SHOW();

     
    cout << endl
         << "=== Методы TO_GET ===" << endl;
    cout << "Курс объекта 2: " << s2.TO_GET_Course() << endl;
    cout << "Группа объекта 2: " << s2.TO_GET_Group() << endl;
     
    cout << "Оценка №4 объекта 2: ";
    cout << s2.TO_GET_Grade(3) << endl;

     
    cout << endl
         << "=== Оператор = ===" << endl;
    s1 = s2;
    cout << "Объект 1 после s1 = s2: ";
    s1.TO_SHOW();

     
    cout << endl
         << "=== Бинарный оператор + ===" << endl;
     
    Student s4 = s2 + s3;
    cout << "Результат s2 + s3: ";
    s4.TO_SHOW();

     
    cout << endl
         << "=== Унарный оператор ++ ===" << endl;
    cout << "Объект 3 до ++: ";
    s3.TO_SHOW();
    ++s3;
    cout << "Объект 3 после ++: ";
    s3.TO_SHOW();

    system("pause");
    return 0;
}
