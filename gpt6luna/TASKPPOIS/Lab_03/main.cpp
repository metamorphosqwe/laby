#include <clocale>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

// Включаем UTF-8, чтобы русский текст правильно отображался в Windows.
void setRussianText() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
#else
    std::setlocale(LC_ALL, "");
#endif
}

class IntegerArray {
private:
    static const int ARRAY_SIZE = 4;
    static const int FIXED_SIZE = 5;

    int& externalNumber;
    int* numbers;
    int fixedNumbers[FIXED_SIZE];

    // У объекта по умолчанию тоже должна быть переменная для ссылки.
    static int& defaultNumber() {
        static int number = 0;
        return number;
    }

public:
    // Конструктор по умолчанию.
    IntegerArray() : externalNumber(defaultNumber()), numbers(new int[ARRAY_SIZE]{}) {
        for (int i = 0; i < FIXED_SIZE; i++) {
            fixedNumbers[i] = 0;
        }
    }

    // Конструктор с параметром: ссылка будет связана с переданной переменной.
    explicit IntegerArray(int& number)
        : externalNumber(number), numbers(new int[ARRAY_SIZE]{}) {
        for (int i = 0; i < FIXED_SIZE; i++) {
            fixedNumbers[i] = 0;
        }
    }

    // Создаём отдельную копию динамического и обычного массивов.
    IntegerArray(const IntegerArray& other)
        : externalNumber(other.externalNumber), numbers(new int[ARRAY_SIZE]) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            numbers[i] = other.numbers[i];
        }
        for (int i = 0; i < FIXED_SIZE; i++) {
            fixedNumbers[i] = other.fixedNumbers[i];
        }
    }

    // Освобождаем память, которую запросили через new[].
    ~IntegerArray() {
        delete[] numbers;
    }

    // Присваиваем значения массивов, не меняя переменную, на которую ссылается объект.
    IntegerArray& operator=(const IntegerArray& other) {
        if (this != &other) {
            int* newNumbers = new int[ARRAY_SIZE];
            for (int i = 0; i < ARRAY_SIZE; i++) {
                newNumbers[i] = other.numbers[i];
            }

            delete[] numbers;
            numbers = newNumbers;

            for (int i = 0; i < FIXED_SIZE; i++) {
                fixedNumbers[i] = other.fixedNumbers[i];
            }
        }
        return *this;
    }

    int size() const {
        return ARRAY_SIZE;
    }

    // Возвращаем указатель на первый элемент динамического массива.
    int* getArray() {
        return numbers;
    }

    // Возвращаем ссылку на массив из пяти элементов.
    int (&getFixedArray())[FIXED_SIZE] {
        return fixedNumbers;
    }

    // Возвращаем ссылку на один элемент, чтобы его можно было изменить.
    int& getElement(int index) {
        if (index < 0 || index >= ARRAY_SIZE) {
            throw std::out_of_range("Такого элемента нет");
        }
        return numbers[index];
    }

    int& getExternalNumber() {
        return externalNumber;
    }
};

int main() {
    setRussianText();

    int outsideNumber = 17;
    IntegerArray numbers(outsideNumber);

    // Заполняем массив через указатель.
    int* array = numbers.getArray();
    for (int i = 0; i < numbers.size(); i++) {
        array[i] = (i + 1) * 10;
    }

    // Копия хранит свои значения отдельно от исходного объекта.
    IntegerArray copy(numbers);
    copy.getElement(0) = 99;

    int anotherNumber = 12;
    IntegerArray assigned(anotherNumber);
    assigned = numbers;
    assigned = assigned; // Проверяем присваивание объекта самому себе.

    // Меняем обычный массив через ссылку на него.
    numbers.getFixedArray()[0] = 100;

    std::cout << "Массив: ";
    for (int i = 0; i < numbers.size(); i++) {
        std::cout << numbers.getArray()[i] << ' ';
    }

    std::cout << "\nПервый элемент копии: " << copy.getElement(0);
    std::cout << "\nЧисло по ссылке: " << numbers.getExternalNumber();
    std::cout << "\nМассив по ссылке: ";
    for (int value : numbers.getFixedArray()) {
        std::cout << value << ' ';
    }

    // Изменение ссылки меняет исходную переменную.
    numbers.getExternalNumber() = 25;
    std::cout << "\nВнешнее число после изменения: " << outsideNumber;

    // Проверяем, что программа сообщает об обращении за границу массива.
    try {
        std::cout << "\nЭлемент за границей: " << numbers.getElement(numbers.size());
    } catch (const std::out_of_range& error) {
        std::cout << '\n' << error.what();
    }

    std::cout << '\n';
    return 0;
}
