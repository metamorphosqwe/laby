// ============================================================
// Лабораторная работа №7
// Тема: «Шаблоны»
// Предметная область: «Студент».
// Реализовано:
//   - шаблоны функций: TO_MAX, TO_SWAP, TO_SUM, TO_FIND;
//   - явная специализация шаблона функции (для const char*);
//   - шаблоны классов: Journal<T> (журнал) и Pair<K, V> (пара).
// Замечание: в учебном проекте всё лежит в одном файле. В
// большом проекте шаблоны помещают в заголовок .hpp, так как
// компилятору нужно видеть определение в момент инстанцирования.
// ============================================================

#include <iostream>    // cout, endl, ostream
#include <string>      // string
#include <cstring>     // strcmp
#include <stdexcept>   // out_of_range
#include <cstdlib>     // system()
#include <clocale>     // setlocale()
using namespace std;

// ==============================================================
// Пользовательский тип, с которым будут работать шаблоны.
// Чтобы Student можно было подставить в TO_MAX, в нём определён
// operator>; чтобы выводить через cout — operator<<. Для
// Journal<Student> нужен ещё конструктор по умолчанию
// (new T[n] создаёт элементы по умолчанию).
// ==============================================================
class Student {
private:
    string name;
    double average;   // средний балл

public:
    Student() : name(""), average(0) {}
    Student(const string& n, double a) : name(n), average(a) {}

    // Сравнение по среднему баллу.
    bool operator>(const Student& other) const {
        return average > other.average;
    }

    friend ostream& operator<<(ostream& os, const Student& s) {
        return os << s.name << " (" << s.average << ")";
    }
};

// ==============================================================
// ШАБЛОНЫ ФУНКЦИЙ
// template <typename T> — T — параметр-тип. Компилятор сам
// создаёт версию функции для каждого использованного типа
// (инстанцирование). Тип T выводится по аргументам вызова.
// ==============================================================

// Передача параметров ПО ЗНАЧЕНИЮ и возврат значения типа T.
// Тип T должен поддерживать оператор >.
template <typename T>
T TO_MAX(T a, T b) {
    return (a > b) ? a : b;
}

// ЯВНАЯ СПЕЦИАЛИЗАЦИЯ для const char*. Общий шаблон сравнил бы
// адреса указателей, а не строки, поэтому для C-строк нужна
// своя версия на основе strcmp.
template <>
const char* TO_MAX<const char*>(const char* a, const char* b) {
    return (strcmp(a, b) > 0) ? a : b;
}

// Передача параметров ПО ССЫЛКЕ: функция меняет аргументы
// вызывающего кода. Возвращаемого значения нет.
template <typename T>
void TO_SWAP(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

// Обработка массива: n элементов типа T, результат типа T.
// T() — «нулевое» значение типа (0, 0.0, пустая строка).
// Тип T должен поддерживать оператор +.
template <typename T>
T TO_SUM(const T arr[], int n) {
    T sum = T();
    for (int i = 0; i < n; i++) {
        sum = sum + arr[i];
    }
    return sum;
}

// Поиск значения: возвращает индекс или -1. Тип T должен
// поддерживать оператор ==.
template <typename T>
int TO_FIND(const T arr[], int n, const T& value) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == value) {
            return i;
        }
    }
    return -1;
}

// ==============================================================
// ШАБЛОН КЛАССА Journal<T> — журнал фиксированной вместимости,
// хранящий значения любого типа T в динамическом массиве.
// Реализованы правило трёх (копирование, присваивание,
// деструктор), как в лабораторной работе №3.
// ==============================================================
template <typename T>
class Journal {
private:
    T* data;          // динамический массив элементов типа T
    int capacity;     // вместимость
    int count;        // сколько элементов занято

public:
    Journal(int cap = 5) : capacity(cap), count(0) {
        data = new T[capacity];
    }

    Journal(const Journal& other)
        : capacity(other.capacity), count(other.count) {
        data = new T[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    ~Journal() {
        delete[] data;
    }

    Journal& operator=(const Journal& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            count = other.count;
            data = new T[capacity];
            for (int i = 0; i < count; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // Добавление: false, если журнал заполнен.
    bool TO_ADD(const T& value) {
        if (count >= capacity) {
            return false;
        }
        data[count++] = value;
        return true;
    }

    // Доступ к элементу по ссылке (можно читать и менять).
    T& TO_GET(int i) {
        if (i < 0 || i >= count) {
            throw out_of_range("индекс вне журнала");
        }
        return data[i];
    }

    int TO_GET_Count() const { return count; }

    // Методы можно определять и вне класса; тогда перед каждым
    // нужен заголовок шаблона и имя Journal<T>::
    T TO_MAX_ELEMENT() const;
    void TO_SHOW() const;
};

// Максимальный элемент: использует шаблон функции TO_MAX.
template <typename T>
T Journal<T>::TO_MAX_ELEMENT() const {
    if (count == 0) {
        throw out_of_range("журнал пуст");
    }
    T best = data[0];
    for (int i = 1; i < count; i++) {
        best = TO_MAX(best, data[i]);
    }
    return best;
}

template <typename T>
void Journal<T>::TO_SHOW() const {
    cout << "[";
    for (int i = 0; i < count; i++) {
        cout << data[i] << (i < count - 1 ? " | " : "");
    }
    cout << "] " << count << "/" << capacity << endl;
}

// ==============================================================
// ШАБЛОН КЛАССА с двумя параметрами-типами: Pair<K, V>
// (например, «имя — оценка»).
// ==============================================================
template <typename K, typename V>
class Pair {
private:
    K first;
    V second;

public:
    Pair(const K& f, const V& s) : first(f), second(s) {}

    K TO_GET_First() const { return first; }
    V TO_GET_Second() const { return second; }

    void TO_SHOW() const {
        cout << first << " -> " << second << endl;
    }
};

int main() {
    system("chcp 65001");                  // UTF-8 в консоли Windows
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // ----- 1. Шаблоны функций -----
    cout << "=== 1. Шаблоны функций ===" << endl;
    // Тип T выводится автоматически по аргументам.
    cout << "TO_MAX(5, 9) = " << TO_MAX(5, 9) << endl;
    cout << "TO_MAX(2.5, 1.5) = " << TO_MAX(2.5, 1.5) << endl;
    cout << "TO_MAX('a', 'z') = " << TO_MAX('a', 'z') << endl;
    cout << "TO_MAX(string, string) = "
        << TO_MAX(string("abc"), string("abd")) << endl;
    // Явное указание типа: аргумент 3 приводится к double.
    cout << "TO_MAX<double>(3, 4.5) = "
        << TO_MAX<double>(3, 4.5) << endl;
    // Специализация для const char*.
    cout << "TO_MAX(\"apple\", \"banana\") = "
        << TO_MAX("apple", "banana") << endl;
    // Пользовательский тип.
    Student s1("Юрий", 8.5);
    Student s2("Анна", 9.2);
    cout << "TO_MAX(s1, s2) = " << TO_MAX(s1, s2) << endl;

    // Передача по ссылке: значения меняются местами.
    int a = 1, b = 2;
    TO_SWAP(a, b);
    cout << "TO_SWAP(int): a = " << a << ", b = " << b << endl;
    string x = "первая", y = "вторая";
    TO_SWAP(x, y);
    cout << "TO_SWAP(string): x = " << x
        << ", y = " << y << endl;

    // Обработка массивов разных типов.
    int grades[] = {8, 9, 7, 10, 9};
    double marks[] = {7.5, 8.5, 9.0};
    string words[] = {"ша", "бло", "н"};
    cout << "TO_SUM(int[5]) = " << TO_SUM(grades, 5) << endl;
    cout << "TO_SUM(double[3]) = " << TO_SUM(marks, 3) << endl;
    cout << "TO_SUM(string[3]) = " << TO_SUM(words, 3) << endl;
    cout << "TO_FIND(grades, 7) = "
        << TO_FIND(grades, 5, 7) << endl;
    cout << "TO_FIND(grades, 4) = "
        << TO_FIND(grades, 5, 4) << endl;
    cout << "TO_FIND(words, \"бло\") = "
        << TO_FIND(words, 3, string("бло")) << endl;

    // ----- 2. Шаблонный класс Journal -----
    cout << endl
        << "=== 2. Шаблонный класс Journal ===" << endl;
    cout << "Journal<int>:" << endl;
    Journal<int> ji(5);
    ji.TO_ADD(8);
    ji.TO_ADD(9);
    ji.TO_ADD(7);
    ji.TO_ADD(10);
    ji.TO_SHOW();
    cout << "  максимум: " << ji.TO_MAX_ELEMENT() << endl;
    ji.TO_GET(2) = 6;                      // изменение через ссылку
    cout << "  после TO_GET(2) = 6: ";
    ji.TO_SHOW();
    ji.TO_ADD(5);
    cout << "  после TO_ADD(5): ";
    ji.TO_SHOW();
    cout << "  TO_ADD в полный журнал: "
        << (ji.TO_ADD(4) ? "добавлено" : "нет места") << endl;

    cout << "Journal<double>:" << endl;
    Journal<double> jd(3);
    jd.TO_ADD(7.5);
    jd.TO_ADD(9.25);
    jd.TO_SHOW();
    cout << "  максимум: " << jd.TO_MAX_ELEMENT() << endl;

    cout << "Journal<string>:" << endl;
    Journal<string> js(3);
    js.TO_ADD("Юрий");
    js.TO_ADD("Анна");
    js.TO_SHOW();

    cout << "Journal<Student>:" << endl;
    Journal<Student> jst(3);
    jst.TO_ADD(s1);
    jst.TO_ADD(s2);
    jst.TO_ADD(Student("Олег", 7.8));
    jst.TO_SHOW();
    cout << "  лучший: " << jst.TO_MAX_ELEMENT() << endl;

    cout << "Копирование Journal<int> (глубокое):" << endl;
    Journal<int> copy(ji);
    copy.TO_GET(0) = 1;
    cout << "  ji:   ";
    ji.TO_SHOW();
    cout << "  copy: ";
    copy.TO_SHOW();

    cout << "Ошибка доступа:" << endl;
    try {
        ji.TO_GET(10);
    }
    catch (const out_of_range& e) {
        cout << "  out_of_range: " << e.what() << endl;
    }

    // ----- 3. Шаблон с двумя параметрами -----
    cout << endl << "=== 3. Шаблонный класс Pair ===" << endl;
    Pair<string, int> p1("Юрий", 10);
    Pair<string, int> p2("Анна", 9);
    Pair<int, double> p3(29, 8.5);
    p1.TO_SHOW();
    p2.TO_SHOW();
    p3.TO_SHOW();
    // Шаблон функции работает и с объектами шаблона класса.
    TO_SWAP(p1, p2);
    cout << "После TO_SWAP(p1, p2):" << endl;
    p1.TO_SHOW();
    p2.TO_SHOW();

    return 0;
}
