#include <clocale>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void setRussianText() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
#else
    std::setlocale(LC_ALL, "");
#endif
}

struct Student {
    int id;
    std::string name;
    double average;
};

// Записываем одного студента в поток.
std::ostream& operator<<(std::ostream& output, const Student& student) {
    output << student.id << ' ' << student.name << ' ' << student.average;
    return output;
}

// Считываем одного студента из потока.
std::istream& operator>>(std::istream& input, Student& student) {
    input >> student.id >> student.name >> student.average;
    return input;
}

int main() {
    setRussianText();

    // get() считывает один символ, а getline() — всю строку.
    std::istringstream characters("Example");
    char firstCharacter;
    characters.get(firstCharacter);
    std::cout << "Первый символ через get(): ";
    std::cout.put(firstCharacter);
    std::cout << '\n';

    std::istringstream lines("Строка для getline");
    std::string line;
    std::getline(lines, line);
    std::cout << "Строка через getline(): " << line << '\n';

    Student students[3] = {
        {101, "Anna", 91.5},
        {102, "Ivan", 84.25},
        {103, "Maria", 97.0}
    };

    // ofstream создаёт файл и записывает в него список студентов.
    std::ofstream outputFile("students.txt");
    if (!outputFile) {
        std::cout << "Не удалось создать файл students.txt\n";
        return 1;
    }

    outputFile << 3 << '\n';
    for (int i = 0; i < 3; i++) {
        outputFile << students[i] << '\n';
    }
    outputFile.close();

    // ifstream читает студентов из сохранённого файла.
    std::ifstream inputFile("students.txt");
    int count;
    if (!(inputFile >> count) || count != 3) {
        std::cout << "Не удалось прочитать файл students.txt\n";
        return 1;
    }

    Student loadedStudents[3];
    for (int i = 0; i < count; i++) {
        if (!(inputFile >> loadedStudents[i])) {
            std::cout << "Ошибка при чтении студента\n";
            return 1;
        }
    }
    inputFile.close();

    // fstream открывает тот же файл для чтения и записи.
    std::fstream file("students.txt", std::ios::in | std::ios::out);
    if (!file) {
        std::cout << "Не удалось открыть файл через fstream\n";
        return 1;
    }
    file.seekp(0);
    file << 3;
    file.close();

    // Манипуляторы делают вывод таблицы ровным и показывают два знака после точки.
    std::cout << "\nЗагруженные из файла студенты:\n";
    for (int i = 0; i < count; i++) {
        std::cout << std::setw(5) << loadedStudents[i].id << "  "
                  << std::setw(8) << loadedStudents[i].name << "  "
                  << std::fixed << std::setprecision(2)
                  << loadedStudents[i].average << std::endl;
    }

    return 0;
}
