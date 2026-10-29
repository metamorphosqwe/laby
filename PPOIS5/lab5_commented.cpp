// ============================================================
// Лабораторная работа №5
// Тема: «Иерархии классов. Наследование»
// Предметная область: «Студент». Иерархия из трёх классов:
//   Person (человек) и Worker (работник) — два базовых класса,
//   WorkingStudent (студент-совместитель) — производный класс,
//   наследующий ОБА базовых (множественное наследование).
// ============================================================

#include <iostream>   // cout, endl
#include <string>     // string
#include <cstdlib>    // system()
#include <clocale>    // setlocale()
using namespace std;

// ==============================================================
// Базовый класс 1: Person
// ==============================================================
class Person {
private:
    string name;      // имя
    int age;          // возраст

public:
    // Конструктор. Сообщение выводится, чтобы увидеть порядок
    // вызова конструкторов в иерархии.
    Person(const string& n, int a) : name(n), age(a) {
        cout << "  [Person] конструктор: " << name << endl;
    }

    // ВИРТУАЛЬНЫЙ деструктор. Если объект производного класса
    // удаляется через указатель на базовый (delete ptr), без
    // virtual вызвался бы только ~Person, а часть объекта,
    // принадлежащая производному классу, не была бы уничтожена.
    virtual ~Person() {
        cout << "  [Person] деструктор: " << name << endl;
    }

    string TO_GET_Name() const { return name; }
    int TO_GET_Age() const { return age; }

    // Метод с ОДИНАКОВЫМ ИМЕНЕМ в обоих базовых классах, но с
    // разным поведением. НЕвиртуальный: вызов определяется по
    // типу указателя/ссылки, а не по типу объекта.
    void TO_INTRODUCE() const {
        cout << "  Person::TO_INTRODUCE: " << name
            << ", возраст " << age << endl;
    }

    // ВИРТУАЛЬНЫЙ метод: вызов определяется по реальному типу
    // объекта (позднее связывание через таблицу виртуальных функций).
    virtual void TO_DESCRIBE() const {
        cout << "  Person::TO_DESCRIBE: человек "
            << name << endl;
    }
};

// ==============================================================
// Базовый класс 2: Worker
// ==============================================================
class Worker {
private:
    string company;   // место работы
    int hours;        // часов в неделю

public:
    Worker(const string& c, int h) : company(c), hours(h) {
        cout << "  [Worker] конструктор: " << company << endl;
    }

    // Виртуальный деструктор нужен и здесь: объект можно удалить
    // и через указатель Worker*.
    virtual ~Worker() {
        cout << "  [Worker] деструктор: " << company << endl;
    }

    string TO_GET_Company() const { return company; }
    int TO_GET_Hours() const { return hours; }
    void TO_SET_Hours(int h) { hours = h; }

    // То же имя, что и в Person, но другое поведение.
    void TO_INTRODUCE() const {
        cout << "  Worker::TO_INTRODUCE: работает в " << company
            << ", " << hours << " ч/нед" << endl;
    }
};

// ==============================================================
// Производный класс: WorkingStudent
// public-наследование от ОБОИХ базовых классов. Порядок
// конструирования базовых классов определяется порядком их
// перечисления здесь (Person, затем Worker), а не порядком
// в списке инициализации конструктора.
// ==============================================================
class WorkingStudent : public Person, public Worker {
private:
    int course;       // курс
    int group;        // группа

public:
    // Список инициализации вызывает конструкторы базовых классов
    // с нужными аргументами, затем инициализирует свои поля.
    WorkingStudent(const string& n, int a, const string& c,
                   int h, int cr, int g)
        : Person(n, a), Worker(c, h), course(cr), group(g) {
        cout << "  [WorkingStudent] конструктор: курс "
            << course << endl;
    }

    // Деструктор: уже виртуальный, так как базовые деструкторы
    // виртуальные. Порядок вызова обратный конструированию.
    ~WorkingStudent() override {
        cout << "  [WorkingStudent] деструктор: курс "
            << course << endl;
    }

    int TO_GET_Course() const { return course; }
    int TO_GET_Group() const { return group; }

    // Метод с тем же именем: СКРЫВАЕТ оба базовых TO_INTRODUCE.
    // Если его не определить, вызов ws.TO_INTRODUCE() станет
    // неоднозначным (ambiguous) и не скомпилируется. Здесь
    // варианты базовых классов вызываются явно через оператор
    // разрешения области видимости ::
    void TO_INTRODUCE() const {
        Person::TO_INTRODUCE();
        Worker::TO_INTRODUCE();
        cout << "  WorkingStudent::TO_INTRODUCE: курс "
            << course << ", группа " << group << endl;
    }

    // Переопределение виртуального метода Person::TO_DESCRIBE.
    // override просит компилятор проверить, что в базовом классе
    // есть такой virtual-метод с той же сигнатурой (при опечатке
    // будет ошибка компиляции, а не новый метод).
    void TO_DESCRIBE() const override {
        cout << "  WorkingStudent::TO_DESCRIBE: студент "
            << TO_GET_Name() << ", курс " << course
            << ", работает в " << TO_GET_Company() << endl;
    }
};

int main() {
    system("chcp 65001");                  // UTF-8 в консоли Windows
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // ----- 1. Объекты базовых классов -----
    cout << "=== 1. Объекты базовых классов ===" << endl;
    Person p("Иван", 20);
    Worker w("Типография", 12);
    cout << "Person: имя " << p.TO_GET_Name()
        << ", возраст " << p.TO_GET_Age() << endl;
    p.TO_INTRODUCE();
    p.TO_DESCRIBE();
    cout << "Worker: место работы " << w.TO_GET_Company()
        << ", часов " << w.TO_GET_Hours() << endl;
    w.TO_SET_Hours(16);
    cout << "После TO_SET_Hours(16): "
        << w.TO_GET_Hours() << endl;
    w.TO_INTRODUCE();

    // ----- 2. Объект производного класса -----
    cout << endl
        << "=== 2. Объект производного класса ===" << endl;
    WorkingStudent ws("Юрий", 19, "Фирма", 15, 2, 29);
    // Унаследованные public-члены обоих базовых классов.
    cout << "Имя: " << ws.TO_GET_Name()
        << ", возраст: " << ws.TO_GET_Age() << endl;
    cout << "Работа: " << ws.TO_GET_Company()
        << ", часов: " << ws.TO_GET_Hours() << endl;
    // Собственные члены.
    cout << "Курс: " << ws.TO_GET_Course()
        << ", группа: " << ws.TO_GET_Group() << endl;
    cout << "TO_INTRODUCE производного класса:" << endl;
    ws.TO_INTRODUCE();
    // Явный выбор базовой версии у объекта производного класса.
    cout << "Явный вызов ws.Person::TO_INTRODUCE():" << endl;
    ws.Person::TO_INTRODUCE();
    cout << "Явный вызов ws.Worker::TO_INTRODUCE():" << endl;
    ws.Worker::TO_INTRODUCE();
    cout << "Виртуальный ws.TO_DESCRIBE():" << endl;
    ws.TO_DESCRIBE();

    // ----- 3. Указатели и оператор -> -----
    cout << endl << "=== 3. Вызов через указатели ===" << endl;
    WorkingStudent* pd = &ws;    // указатель на производный класс
    Person* pp = &ws;            // указатель на 1-й базовый класс
    Worker* pw = &ws;            // указатель на 2-й базовый класс

    cout << "Через WorkingStudent*:" << endl;
    cout << "  " << pd->TO_GET_Name() << ", "
        << pd->TO_GET_Age() << ", "
        << pd->TO_GET_Company() << ", "
        << pd->TO_GET_Hours() << ", "
        << pd->TO_GET_Course() << ", "
        << pd->TO_GET_Group() << endl;
    pd->TO_INTRODUCE();

    // Через указатель базового класса видны ТОЛЬКО члены базового.
    cout << "Через Person*:" << endl;
    cout << "  " << pp->TO_GET_Name() << ", "
        << pp->TO_GET_Age() << endl;
    pp->TO_INTRODUCE();   // невиртуальный: версия Person

    cout << "Через Worker*:" << endl;
    cout << "  " << pw->TO_GET_Company() << ", "
        << pw->TO_GET_Hours() << endl;
    pw->TO_INTRODUCE();   // невиртуальный: версия Worker

    // ----- 4. Виртуальные функции -----
    cout << endl << "=== 4. Виртуальные функции ===" << endl;
    cout << "Прямой вызов ws.TO_DESCRIBE():" << endl;
    ws.TO_DESCRIBE();
    cout << "Через Person* на WorkingStudent (virtual):"
        << endl;
    pp->TO_DESCRIBE();    // вызовется версия WorkingStudent
    cout << "Невиртуальный pp->TO_INTRODUCE() для сравнения:"
        << endl;
    pp->TO_INTRODUCE();   // вызовется версия Person

    cout << "Срезка: Person copy = ws; copy.TO_DESCRIBE():"
        << endl;
    {
        // Копирование в объект базового типа «срезает» часть
        // производного класса: copy — настоящий Person, поэтому
        // виртуальность не помогает.
        Person copy = ws;
        copy.TO_DESCRIBE();
    }

    cout << "Динамический объект через Person*:" << endl;
    Person* dp =
        new WorkingStudent("Анна", 21, "Кафе", 10, 3, 29);
    dp->TO_DESCRIBE();
    cout << "delete dp (виртуальный деструктор):" << endl;
    delete dp;   // вызовутся ~WorkingStudent, ~Worker, ~Person

    cout << endl << "Выход из main:" << endl;
    return 0;
}   // деструкторы ws, w, p
