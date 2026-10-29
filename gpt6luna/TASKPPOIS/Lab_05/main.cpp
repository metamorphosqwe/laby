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

class FirstBase {
protected:
    int number;

public:
    explicit FirstBase(int value) : number(value) {}

    void action() const {
        std::cout << "Функция первого класса, число: " << number << '\n';
    }

    virtual std::string kind() const {
        return "Первый базовый класс";
    }

    int getNumber() const {
        return number;
    }
};

class SecondBase {
protected:
    std::string text;

public:
    explicit SecondBase(const std::string& value) : text(value) {}

    void action() const {
        std::cout << "Функция второго класса, текст: " << text << '\n';
    }

    std::string getText() const {
        return text;
    }
};

// Combined наследует данные и функции сразу от двух классов.
class Combined : public FirstBase, public SecondBase {
private:
    int extraNumber;

public:
    Combined(int value, const std::string& textValue, int extra)
        : FirstBase(value), SecondBase(textValue), extraNumber(extra) {}

    // Указываем имя класса, потому что в обоих базовых классах есть action().
    void action() const {
        FirstBase::action();
        SecondBase::action();
        std::cout << "Дополнительное число: " << extraNumber << '\n';
    }

    std::string kind() const override {
        return "Производный класс Combined";
    }

    int getExtraNumber() const {
        return extraNumber;
    }
};

int main() {
    setRussianText();

    FirstBase first(42);
    SecondBase second("наследование");
    Combined combined(42, "наследование", 7);

    std::cout << "Вызов функций базовых классов:\n";
    first.action();
    std::cout << first.kind() << ", число: " << first.getNumber() << '\n';
    second.action();
    std::cout << "Текст: " << second.getText() << '\n';

    std::cout << "\nВызов функций производного класса:\n";
    combined.action();
    std::cout << "Тип объекта: " << combined.kind() << '\n';

    // Эти указатели указывают на один объект, но показывают его разные части.
    FirstBase* firstPointer = &combined;
    SecondBase* secondPointer = &combined;
    Combined* combinedPointer = &combined;

    std::cout << "\nВызов через указатели:\n";
    firstPointer->action();
    secondPointer->action();
    combinedPointer->action();

    // Благодаря virtual вызывается версия из Combined.
    std::cout << "\nВиртуальная функция: " << firstPointer->kind() << '\n';
    std::cout << "Значения полей: " << firstPointer->getNumber() << ", "
              << secondPointer->getText() << ", "
              << combinedPointer->getExtraNumber() << '\n';
    return 0;
}
