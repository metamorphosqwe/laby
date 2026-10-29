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

// Функция складывает два значения одинакового типа.
template <typename T>
T add(T first, T second) {
    return first + second;
}

// Шаблонный класс хранит одно значение любого типа.
template <typename T>
class Box {
private:
    T value;

public:
    explicit Box(T startValue) : value(startValue) {}

    T get() const {
        return value;
    }

    void set(T newValue) {
        value = newValue;
    }
};

int main() {
    setRussianText();

    Box<int> integerBox(12);
    std::cout << "Числа: " << add(12, 30) << '\n';
    integerBox.set(add(12, 30));
    std::cout << "Значение в Box<int>: " << integerBox.get() << '\n';

    Box<double> doubleBox(3.14);
    std::cout << "Дробные числа: " << add(3.14, 2.0) << '\n';
    doubleBox.set(add(3.14, 2.0));
    std::cout << "Значение в Box<double>: " << doubleBox.get() << '\n';

    Box<std::string> textBox("Привет");
    std::cout << "Строки: " << add(std::string("Привет "),
                                    std::string("мир")) << '\n';
    textBox.set(add(std::string("Привет "), std::string("мир")));
    std::cout << "Значение в Box<string>: " << textBox.get() << '\n';
    return 0;
}
