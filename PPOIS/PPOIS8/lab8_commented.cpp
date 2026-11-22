// Лабораторная работа №8. Потоки ввода-вывода (iostream).
// Предметная область: класс Student (из предыдущих лабораторных).
// Версия с комментариями. Версия для отчёта получается удалением
// строк, целиком состоящих из комментариев (вывод идентичен).

#include <cstdio>   // std::remove: удалить временные файлы
#include <fstream>  // ifstream, ofstream, fstream
#include <iomanip>  // манипуляторы с параметрами: setw и др.
#include <iostream> // cin, cout, ostream, istream
#include <sstream>  // istringstream, ostringstream
#include <string>

// Класс Student: поля закрыты, ввод-вывод идёт через
// дружественные операторы << и >>, которые умеют работать
// с ЛЮБЫМ потоком (консоль, файл, строка), так как принимают
// базовые классы std::ostream и std::istream.
class Student {
    std::string name;
    std::string group;
    int year;
    double avg;

public:
    Student() : name(""), group(""), year(0), avg(0.0) {}
    Student(const std::string& n, const std::string& g,
            int y, double a)
        : name(n), group(g), year(y), avg(a) {}

    // const-методы доступа: нужны для вывода в таблицу
    const std::string& getName() const { return name; }
    const std::string& getGroup() const { return group; }
    int getYear() const { return year; }
    double getAvg() const { return avg; }

    // friend: операторам нужен доступ к закрытым полям.
    // Поток передаётся и возвращается по ссылке, потому что
    // потоки нельзя копировать; возврат ссылки позволяет
    // писать цепочки: out << a << b.
    friend std::ostream& operator<<(std::ostream& out,
                                    const Student& s);
    friend std::istream& operator>>(std::istream& in,
                                    Student& s);
};

// Формат одной записи в файле (одна строка):
// ФИО|группа|курс|средний балл
// Разделитель '|' выбран потому, что в ФИО есть пробелы,
// а '|' в данных не встречается.
std::ostream& operator<<(std::ostream& out, const Student& s) {
    // запоминаем флаги и точность, чтобы не испортить поток
    std::ios_base::fmtflags f = out.flags();
    std::streamsize p = out.precision();
    out << s.name << '|' << s.group << '|' << s.year << '|'
        << std::fixed << std::setprecision(2) << s.avg;
    out.flags(f);
    out.precision(p);
    return out;
}

std::istream& operator>>(std::istream& in, Student& s) {
    std::string n, g, y, a;
    // getline с третьим параметром читает до разделителя
    if (std::getline(in, n, '|') && std::getline(in, g, '|') &&
        std::getline(in, y, '|') && std::getline(in, a)) {
        s.name = n;
        s.group = g;
        s.year = std::stoi(y);
        s.avg = std::stod(a);
    }
    // при неудаче поток сам перейдёт в состояние fail,
    // и условие цикла while (in >> s) завершится
    return in;
}

// ---------- 1. Чтение: get(), getline() ----------
void demoInput() {
    std::cout << "=== 1. Чтение: get() и getline() ==="
              << std::endl;
    // Вместо клавиатуры используем istringstream: тот же
    // интерфейс istream, но вывод программы повторяем.
    // С cin методы работают точно так же.
    std::istringstream in("Hello\nIvanov Ivan\n42\nAI-29\nxyz");

    char c = in.get(); // get() без параметров: один символ
    std::cout << "get(): первый символ = '" << c << "'"
              << std::endl;

    char buf[32];
    // get(буфер, размер): читает до '\n', сам '\n' НЕ извлекает
    in.get(buf, 32);
    std::cout << "get(buf, 32): остаток слова = \"" << buf
              << "\"" << std::endl;
    std::cout << "peek(): следующий символ - перевод строки? "
              << std::boolalpha << (in.peek() == '\n')
              << std::endl;
    in.get(); // убираем оставшийся '\n'

    std::string line;
    // getline() читает всю строку и ИЗВЛЕКАЕТ '\n'
    std::getline(in, line);
    std::cout << "getline(): \"" << line << "\"" << std::endl;

    int number;
    in >> number; // оператор >> оставляет '\n' в потоке
    std::cout << "operator>>: число = " << number << std::endl;

    std::string bad;
    std::getline(in, bad);
    std::cout << "getline без ignore(): \"" << bad
              << "\" (пустая строка!)" << std::endl;

    std::getline(in, line);
    std::cout << "getline следующий: \"" << line << "\""
              << std::endl;

    // демонстрация правильного решения: cin.ignore()
    std::istringstream in2("7\nPetrov Petr\n");
    int k;
    in2 >> k;
    in2.ignore(); // выбрасываем '\n' после числа
    std::string name2;
    std::getline(in2, name2);
    std::cout << "С ignore(): k = " << k << ", строка = \""
              << name2 << "\"" << std::endl;
    std::cout << std::endl;
}

// ---------- 2. Вывод: put() ----------
void demoPut() {
    std::cout << "=== 2. Вывод: put() ===" << std::endl;
    std::cout.put('A');
    std::cout.put('B').put('C').put('\n'); // put возвращает ostream&
    const char* word = "Stream";
    for (int i = 0; word[i] != '\0'; ++i) {
        std::cout.put(word[i]);
        if (word[i + 1] != '\0') {
            std::cout.put('-');
        }
    }
    std::cout.put('\n');
    // put() работает и с другими потоками, например со строковым
    std::ostringstream os;
    for (char ch = 'a'; ch <= 'e'; ++ch) {
        os.put(ch);
    }
    std::cout << "ostringstream.put(): " << os.str()
              << std::endl;
    std::cout << std::endl;
}

// ---------- 3. Файловый ввод-вывод ----------
// Единая проверка открытия файла (по совету из задания)
bool openedOk(const std::ios& s, const std::string& fname) {
    if (!s) {
        std::cout << "Ошибка открытия файла " << fname
                  << std::endl;
        return false;
    }
    return true;
}

void demoFiles() {
    std::cout << "=== 3. Файлы: ofstream, ifstream, fstream ==="
              << std::endl;
    const std::string fname = "demo.txt";

    // ofstream: только запись; режим по умолчанию - обрезание
    std::ofstream out(fname);
    if (!openedOk(out, fname)) {
        return;
    }
    out << "Первая строка" << std::endl;
    out << "Число: " << 123 << std::endl;
    out << "Вещественное: " << 3.5 << std::endl;
    out.close(); // закрываем явно, чтобы данные записались
    std::cout << "ofstream: файл " << fname << " записан"
              << std::endl;

    // ifstream: только чтение, построчно через getline
    std::ifstream in(fname);
    if (!openedOk(in, fname)) {
        return;
    }
    std::string line;
    int n = 0;
    while (std::getline(in, line)) {
        std::cout << "ifstream, строка " << ++n << ": " << line
                  << std::endl;
    }
    in.close();

    // fstream: чтение и запись одним объектом.
    // ios::app - дописывать в конец файла.
    std::fstream io(fname, std::ios::in | std::ios::out |
                               std::ios::app);
    if (!openedOk(io, fname)) {
        return;
    }
    io << "Добавлено через fstream" << std::endl;
    // после записи возвращаем позицию чтения в начало
    io.clear();
    io.seekg(0, std::ios::beg);
    n = 0;
    while (std::getline(io, line)) {
        ++n;
    }
    std::cout << "fstream: после дозаписи строк в файле = " << n
              << std::endl;
    // посимвольное чтение get() из файла: считаем символы
    io.clear();
    io.seekg(0, std::ios::beg);
    int chars = 0;
    char ch;
    while (io.get(ch)) {
        ++chars;
    }
    std::cout << "fstream.get(): символов (байт) = " << chars
              << std::endl;
    io.close();

    // Попытка открыть несуществующий файл
    std::ifstream missing("no_such_file.txt");
    if (!openedOk(missing, "no_such_file.txt")) {
        std::cout << "good()=" << missing.good() << ", fail()="
                  << missing.fail() << std::endl;
    }
    std::remove(fname.c_str());
    std::cout << std::endl;
}

// ---------- 4. Манипуляторы ----------
void demoManip() {
    std::cout << "=== 4. Манипуляторы потока ===" << std::endl;
    double pi = 3.14159265358979;

    std::cout << "По умолчанию:      " << pi << std::endl;
    std::cout << "setprecision(3):   " << std::setprecision(3)
              << pi << std::endl;
    std::cout << "fixed + prec(3):   " << std::fixed << pi
              << std::endl;
    std::cout << "fixed + prec(8):   " << std::setprecision(8)
              << pi << std::endl;
    std::cout << "scientific(3):     " << std::scientific
              << std::setprecision(3) << pi << std::endl;
    // возврат к общему формату и точности 6
    std::cout.unsetf(std::ios_base::floatfield);
    std::cout << std::setprecision(6);

    std::cout << "setw(10), right:  [" << std::setw(10) << 42
              << "]" << std::endl;
    std::cout << "setw(10), left:   [" << std::left
              << std::setw(10) << 42 << "]" << std::endl;
    std::cout << "setfill('*'):     [" << std::setfill('*')
              << std::right << std::setw(10) << 42 << "]"
              << std::endl;
    std::cout << std::setfill(' ');

    std::cout << "hex/oct/dec:       " << std::hex << 255 << " "
              << std::oct << 255 << " " << std::dec << 255
              << std::endl;
    std::cout << "showbase+hex:      " << std::showbase
              << std::hex << 255 << std::dec
              << std::noshowbase << std::endl;
    std::cout << "showpos:           " << std::showpos << 7
              << " " << -7 << std::noshowpos << std::endl;
    std::cout << "boolalpha:         " << std::boolalpha
              << true << " " << false << std::noboolalpha
              << " (" << true << ")" << std::endl;
    std::cout << "uppercase+hex:     " << std::uppercase
              << std::hex << 255 << std::dec
              << std::nouppercase << std::endl;
    // setw действует только на ближайший вывод
    std::cout << "setw один раз:    [" << std::setw(6) << 1
              << "][" << 2 << "]" << std::endl;
    std::cout << "flush и endl:      " << std::flush << "flush "
              << "(без перевода) " << std::endl;
    std::cout << std::endl;
}

// ---------- 5. Сохранение и загрузка массива объектов ----------
void printTable(const Student* a, int n) {
    // setw для выравнивания столбцов (имена латиницей, чтобы
    // ширина считалась по символам, а не по байтам UTF-8)
    std::cout << std::left << std::setw(4) << "No"
              << std::setw(18) << "Name" << std::setw(8)
              << "Group" << std::setw(6) << "Year"
              << std::right << std::setw(6) << "Avg"
              << std::endl;
    std::cout << std::setfill('-') << std::setw(42) << ""
              << std::setfill(' ') << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << std::left << std::setw(4) << i + 1
                  << std::setw(18) << a[i].getName()
                  << std::setw(8) << a[i].getGroup()
                  << std::setw(6) << a[i].getYear()
                  << std::right << std::setw(6) << std::fixed
                  << std::setprecision(2) << a[i].getAvg()
                  << std::endl;
    }
    std::cout.unsetf(std::ios_base::floatfield);
    std::cout << std::setprecision(6) << std::left;
}

// Сохранение: возвращает true при успехе
bool saveStudents(const std::string& fname, const Student* a,
                  int n) {
    std::ofstream out(fname);
    if (!openedOk(out, fname)) {
        return false;
    }
    for (int i = 0; i < n; ++i) {
        out << a[i] << std::endl;
    }
    return true;
}

// Загрузка: читает до конца файла, возвращает число записей
int loadStudents(const std::string& fname, Student* a,
                 int maxN) {
    std::ifstream in(fname);
    if (!openedOk(in, fname)) {
        return 0;
    }
    int n = 0;
    Student s;
    while (n < maxN && in >> s) {
        a[n++] = s;
    }
    return n;
}

void demoObjects() {
    std::cout << "=== 5. Сохранение и загрузка массива ==="
              << std::endl;
    const int N = 4;
    Student src[N] = {
        Student("Ivanov Ivan", "AI-29", 2, 8.75),
        Student("Petrova Anna", "AI-29", 2, 9.5),
        Student("Sidorov Oleg", "AI-28", 3, 7.13),
        Student("Kozlov Maksim", "AI-30", 1, 6.0)};

    std::cout << "Исходный массив:" << std::endl;
    printTable(src, N);

    const std::string fname = "students.txt";
    if (!saveStudents(fname, src, N)) {
        return;
    }
    std::cout << "Сохранено записей: " << N << std::endl;

    std::cout << "Содержимое файла " << fname << ":"
              << std::endl;
    std::ifstream raw(fname);
    std::string line;
    while (std::getline(raw, line)) {
        std::cout << "  " << line << std::endl;
    }
    raw.close();

    Student dst[10];
    int m = loadStudents(fname, dst, 10);
    std::cout << "Загружено записей: " << m << std::endl;
    printTable(dst, m);

    bool same = (m == N);
    for (int i = 0; same && i < N; ++i) {
        same = src[i].getName() == dst[i].getName() &&
               src[i].getGroup() == dst[i].getGroup() &&
               src[i].getYear() == dst[i].getYear() &&
               src[i].getAvg() == dst[i].getAvg();
    }
    std::cout << "Массивы совпадают: " << std::boolalpha << same
              << std::endl;

    // операторы << и >> работают и с консолью, и со строкой
    std::istringstream one("Test Student|AI-29|2|9.10");
    Student t;
    one >> t;
    std::cout << "Из строки через >>: " << t << std::endl;

    loadStudents("no_such_file.txt", dst, 10);
    std::remove(fname.c_str());
}

int main() {
    demoInput();
    demoPut();
    demoFiles();
    demoManip();
    demoObjects();
    return 0;
}
