#include <clocale>
#include <exception>
#include <iostream>
#include <stdexcept>

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

namespace FirstSpace {

// Собственное исключение для неверного числа.
class WrongNumber : public std::exception {
public:
    const char* what() const noexcept override {
        return "Число должно быть больше нуля";
    }
};

class Calculator {
public:
    int calculate(int number) const {
        if (number <= 0) {
            throw WrongNumber();
        }
        return number * 2;
    }
};

}

namespace SecondSpace {

class Calculator {
public:
    double calculate(double first, double second) const {
        if (second == 0) {
            throw std::domain_error("На ноль делить нельзя");
        }
        return first / second;
    }
};

}

int main() {
    setRussianText();

    FirstSpace::Calculator firstCalculator;
    SecondSpace::Calculator secondCalculator;

    try {
        std::cout << "Умножение: "
                  << firstCalculator.calculate(12) << '\n';
        firstCalculator.calculate(0);
    } catch (const FirstSpace::WrongNumber& error) {
        std::cout << "Поймали свою ошибку: " << error.what() << '\n';
    }

    try {
        std::cout << "Деление: "
                  << secondCalculator.calculate(7.5, 2.5) << '\n';
        secondCalculator.calculate(7.5, 0);
    } catch (const std::domain_error& error) {
        std::cout << "Поймали стандартную ошибку: " << error.what() << '\n';
    }

    return 0;
}
