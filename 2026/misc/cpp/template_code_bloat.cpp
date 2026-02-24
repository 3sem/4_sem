#include <iostream>
#include <vector>

// Шаг 1: Объявляем множество уникальных типов
struct Type1 { int data; };
struct Type2 { int data; };
struct Type3 { int data; };
struct Type4 { int data; };
// ... и так далее до Type100
struct Type100 { int data; };

// Шаг 2: Создаем шаблон функции.
// Важно, чтобы компилятор не смог заинлайнить всё в main,
// поэтому тело функции вынесено (или там есть сложность).
template <typename T>
void process_data(const std::vector<T>& vec) {
    // возможнно __attribute__((noinline)) для GCC/Clang или #pragma optimize("", off) для MSVC
    int dummy = 0; // возможно volatile, чтобы компилятор не выкинул код
    for (const auto& item : vec) {
        dummy += item.data; // Какая-то операция, специфичная для типа T
    }
    std::cout << dummy; // Используем dummy
}

int main() {
    // Создаем вектора разных типов
    std::vector<Type1> v1;
    std::vector<Type2> v2;
    std::vector<Type3> v3;
    std::vector<Type4> v4;
    std::vector<Type4> v5;
    // ... инициализация до v100

    // Инстанцируем шаблон для КАЖДОГО типа
    process_data(v1);
    process_data(v2);
    process_data(v3);
    process_data(v4);
    process_data(v5);
    
    

    return 0;
}