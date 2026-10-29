 
 
 
 
 
 

#include <iostream>    
#include <string>      
#include <cstdlib>     
#include <clocale>     
using namespace std;


 
 
class Subject;

 
 
 
class Student {
public:
    static const int SIZE = 5;

private:
    int course;            
    int group;             
    int* grades;           
    int& rating;           
    static int alive;      
    static int defaultRating;    

public:
     
    Student() : rating(defaultRating) {
        course = 1;
        group = 0;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = 0;
        }
        alive++;
    }

     
    Student(int c, int g, const int arr[], int& r) : rating(r) {
        course = c;
        group = g;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = arr[i];
        }
        alive++;
    }

     
    Student(const Student& other) : rating(other.rating) {
        course = other.course;
        group = other.group;
        grades = new int[SIZE];
        for (int i = 0; i < SIZE; i++) {
            grades[i] = other.grades[i];
        }
        alive++;
    }

     
    ~Student() {
        delete[] grades;
        alive--;
    }

     
    Student& operator=(const Student& other) {
        if (this != &other) {
            delete[] grades;
            course = other.course;
            group = other.group;
            grades = new int[SIZE];
            for (int i = 0; i < SIZE; i++) {
                grades[i] = other.grades[i];
            }
             
        }
        return *this;
    }

     
     
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

     
     
     
     
     
     

     
    friend void TO_SHOW_PRIVATE(const Student& s);

     
     
    friend ostream& operator<<(ostream& os, const Student& s);

     
     
    friend bool TO_CHECK_PASS(const Student& s,
                              const Subject& sub);
    friend void TO_EXAM(Student& s, Subject& sub, int mark);

     
     
     
    friend class Deanery;
};

 
int Student::alive = 0;
int Student::defaultRating = 0;

 
 
 
class Subject {
private:
    string name;       
    int passMark;      
    int index;         
    int examCount;     

public:
    Subject(const string& n, int p, int i)
        : name(n), passMark(p), index(i), examCount(0) {}

     
    int TO_GET_ExamCount() const { return examCount; }

     
     
    friend bool TO_CHECK_PASS(const Student& s,
                              const Subject& sub);
    friend void TO_EXAM(Student& s, Subject& sub, int mark);
};

 
 
 
 

 
 
void TO_SHOW_PRIVATE(const Student& s) {
    cout << "[friend] course = " << s.course
        << ", group = " << s.group << endl;
    cout << "[friend] grades = [";
    for (int i = 0; i < Student::SIZE; i++) {
        cout << s.grades[i] << (i < Student::SIZE - 1 ? ", " : "");
    }
    cout << "], rating = " << s.rating << endl;
}

 
 
ostream& operator<<(ostream& os, const Student& s) {
    os << "Student{курс=" << s.course << ", группа=" << s.group
        << ", оценки=[";
    for (int i = 0; i < Student::SIZE; i++) {
        os << s.grades[i] << (i < Student::SIZE - 1 ? " " : "");
    }
    os << "], рейтинг=" << s.rating << "}";
    return os;
}

 
 
bool TO_CHECK_PASS(const Student& s, const Subject& sub) {
    int mark = s.grades[sub.index];
    bool ok = mark >= sub.passMark;
    cout << "  " << sub.name << " (оценка №" << sub.index + 1
        << "): " << mark << ", проходной " << sub.passMark
        << " -> " << (ok ? "сдан" : "не сдан") << endl;
    return ok;
}

 
 
void TO_EXAM(Student& s, Subject& sub, int mark) {
    s.grades[sub.index] = mark;
    sub.examCount++;
    cout << "  Экзамен «" << sub.name << "»: оценка " << mark
        << " (экзаменов: " << sub.examCount << ")" << endl;
}

 
 
 
class Deanery {
private:
    int processed;     
     
     

public:
    Deanery() : processed(0) {}

     
    void TO_PROMOTE(Student& s) {
        if (s.course < 5) {
            s.course++;
        }
        processed++;
        cout << "  Перевод: теперь курс " << s.course << endl;
    }

     
     
    void TO_SET_Rating(Student& s, int r) {
        s.rating = r;
        processed++;
        cout << "  Рейтинг установлен: " << r << endl;
    }

     
    void TO_RESET_Grades(Student& s) {
        for (int i = 0; i < Student::SIZE; i++) {
            s.grades[i] = 0;
        }
        processed++;
        cout << "  Оценки обнулены" << endl;
    }

     
    void TO_REPORT(const Student& s) const {
        cout << "  Отчёт: курс " << s.course << ", группа "
            << s.group << ", рейтинг " << s.rating
            << ", сумма оценок " << s.TO_GET_Sum() << endl;
    }

    int TO_GET_Processed() const { return processed; }
};

int main() {
    system("chcp 65001");                   
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int g[Student::SIZE] = {8, 9, 7, 10, 9};
    int groupRating = 70;
    Student s(2, 29, g, groupRating);

     
    cout << "=== 1. Дружественная функция ===" << endl;
    cout << "Открытый метод TO_SHOW: ";
    s.TO_SHOW();
    TO_SHOW_PRIVATE(s);
    cout << "operator<<: " << s << endl;

     
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
