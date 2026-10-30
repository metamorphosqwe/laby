#include <iostream>
#include <string>
#include <cstdlib>
#include <clocale>
using namespace std;

class Person
{
private:
    string name;
    int age;

public:
    Person(const string &n, int a) : name(n), age(a)
    {
        cout << "  [Person] конструктор: " << name << endl;
    }

    virtual ~Person()
    {
        cout << "  [Person] деструктор: " << name << endl;
    }

    string TO_GET_Name() const { return name; }
    int TO_GET_Age() const { return age; }

    void TO_INTRODUCE() const
    {
        cout << "  Person::TO_INTRODUCE: " << name
             << ", возраст " << age << endl;
    }

    virtual void TO_DESCRIBE() const
    {
        cout << "  Person::TO_DESCRIBE: человек "
             << name << endl;
    }
};

class Worker
{
private:
    string company;
    int hours;

public:
    Worker(const string &c, int h) : company(c), hours(h)
    {
        cout << "  [Worker] конструктор: " << company << endl;
    }

    virtual ~Worker()
    {
        cout << "  [Worker] деструктор: " << company << endl;
    }

    string TO_GET_Company() const { return company; }
    int TO_GET_Hours() const { return hours; }
    void TO_SET_Hours(int h) { hours = h; }

    void TO_INTRODUCE() const
    {
        cout << "  Worker::TO_INTRODUCE: работает в " << company
             << ", " << hours << " ч/нед" << endl;
    }
};

class WorkingStudent : public Person, public Worker
{
private:
    int course;
    int group;

public:
    WorkingStudent(const string &n, int a, const string &c,
                   int h, int cr, int g)
        : Person(n, a), Worker(c, h), course(cr), group(g)
    {
        cout << "  [WorkingStudent] конструктор: курс "
             << course << endl;
    }

    ~WorkingStudent() override
    {
        cout << "  [WorkingStudent] деструктор: курс "
             << course << endl;
    }

    int TO_GET_Course() const { return course; }
    int TO_GET_Group() const { return group; }

    void TO_INTRODUCE() const
    {
        Person::TO_INTRODUCE();
        Worker::TO_INTRODUCE();
        cout << "  WorkingStudent::TO_INTRODUCE: курс "
             << course << ", группа " << group << endl;
    }

    void TO_DESCRIBE() const override
    {
        cout << "  WorkingStudent::TO_DESCRIBE: студент "
             << TO_GET_Name() << ", курс " << course
             << ", работает в " << TO_GET_Company() << endl;
    }
};

int main()
{
    system("chcp 65001");
    setlocale(LC_ALL, "ru_RU.UTF-8");

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

    cout << endl
         << "=== 2. Объект производного класса ===" << endl;
    WorkingStudent ws("Юрий", 19, "Фирма", 15, 2, 29);

    cout << "Имя: " << ws.TO_GET_Name()
         << ", возраст: " << ws.TO_GET_Age() << endl;
    cout << "Работа: " << ws.TO_GET_Company()
         << ", часов: " << ws.TO_GET_Hours() << endl;

    cout << "Курс: " << ws.TO_GET_Course()
         << ", группа: " << ws.TO_GET_Group() << endl;
    cout << "TO_INTRODUCE производного класса:" << endl;
    ws.TO_INTRODUCE();

    cout << "Явный вызов ws.Person::TO_INTRODUCE():" << endl;
    ws.Person::TO_INTRODUCE();
    cout << "Явный вызов ws.Worker::TO_INTRODUCE():" << endl;
    ws.Worker::TO_INTRODUCE();
    cout << "Виртуальный ws.TO_DESCRIBE():" << endl;
    ws.TO_DESCRIBE();

    cout << endl
         << "=== 3. Вызов через указатели ===" << endl;
    WorkingStudent *pd = &ws;
    Person *pp = &ws;
    Worker *pw = &ws;

    cout << "Через WorkingStudent*:" << endl;
    cout << "  " << pd->TO_GET_Name() << ", "
         << pd->TO_GET_Age() << ", "
         << pd->TO_GET_Company() << ", "
         << pd->TO_GET_Hours() << ", "
         << pd->TO_GET_Course() << ", "
         << pd->TO_GET_Group() << endl;
    pd->TO_INTRODUCE();

    cout << "Через Person*:" << endl;
    cout << "  " << pp->TO_GET_Name() << ", "
         << pp->TO_GET_Age() << endl;
    pp->TO_INTRODUCE();

    cout << "Через Worker*:" << endl;
    cout << "  " << pw->TO_GET_Company() << ", "
         << pw->TO_GET_Hours() << endl;
    pw->TO_INTRODUCE();

    cout << endl
         << "=== 4. Виртуальные функции ===" << endl;
    cout << "Прямой вызов ws.TO_DESCRIBE():" << endl;
    ws.TO_DESCRIBE();
    cout << "Через Person* на WorkingStudent (virtual):"
         << endl;
    pp->TO_DESCRIBE();
    cout << "Невиртуальный pp->TO_INTRODUCE() для сравнения:"
         << endl;
    pp->TO_INTRODUCE();

    cout << "Срезка: Person copy = ws; copy.TO_DESCRIBE():"
         << endl;
    {

        Person copy = ws;
        copy.TO_DESCRIBE();
    }

    cout << "Динамический объект через Person*:" << endl;
    Person *dp =
        new WorkingStudent("Анна", 21, "Кафе", 10, 3, 29);
    dp->TO_DESCRIBE();
    cout << "delete dp (виртуальный деструктор):" << endl;
    delete dp;

    cout << endl
         << "Выход из main:" << endl;
    return 0;
}
