#include <clocale>
#include <iostream>
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

class Profile {
private:
    std::string name;
    std::string label;
    int score;

public:
    explicit Profile(const std::string& profileName)
        : name(profileName), label("нет подписи"), score(0) {}

    // Эта функция вызывается, если передано целое число.
    void set(int newScore) {
        score = newScore;
    }

    // Эта функция вызывается, если передано дробное число.
    void set(double newScore) {
        score = static_cast<int>(newScore);
    }

    // Здесь два параметра: число и подпись.
    void set(int newScore, const std::string& newLabel) {
        score = newScore;
        label = newLabel;
    }

    // Для обычной или неизменяемой строки сохраняем её копию.
    void set(const std::string& newLabel) {
        label = newLabel;
    }

    // Для изменяемой строки выбирается эта версия: она меняет исходную строку.
    void set(std::string& newLabel) {
        label = newLabel;
        newLabel += " (прочитано)";
    }

    int get() { return score; }
    int get() const { return score; }

    void show() const {
        std::cout << name << ": оценка " << score
                  << ", подпись: " << label << '\n';
    }
};

int main() {
    setRussianText();

    Profile profile("Иван");
    std::string mutableLabel = "текущая";
    const std::string fixedLabel = "итог";

    profile.set(82);                 // Версия для int.
    profile.set(88.5);               // Версия для double.
    profile.set(91, fixedLabel);     // Версия с двумя параметрами.
    profile.set(mutableLabel);       // Версия для изменяемой строки.
    profile.set(fixedLabel);         // Версия для const-строки.

    std::cout << "Изменённая строка: " << mutableLabel << '\n';
    std::cout << "Обычный вызов get(): " << profile.get() << '\n';

    const Profile& readOnlyProfile = profile;
    std::cout << "Вызов get() для const-объекта: "
              << readOnlyProfile.get() << '\n';
    profile.show();

    // Нельзя перегрузить функции, отличающиеся только типом результата:
    // int result();
    // double result();
    return 0;
}
