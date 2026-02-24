#include <iostream>

template <typename T>
T sum_variadic(T t) {
    volatile T result = t; // volatile предотвращает оптимизацию
    return result;
}

// Рекурсивный шаблон для нескольких аргументов
template <typename T, typename... Args>
T sum_variadic(T first, Args... args) {
    volatile T result = first + sum_variadic(args...);
    return result;
}

// Функция, которая будет инстанцироваться с разным количеством аргументов
template <typename... Args>
auto calculate_variadic(Args... args) {
    return sum_variadic(args...);
}

int main() {
    volatile int result = 0; // чтобы компилятор не выкинул код
    
    // Тест 1: variadic templates - вызываем с 1..N аргументами
    result += sum_variadic(1);
    result += sum_variadic(1, 2);
    result += sum_variadic(1, 2, 3);
    result += sum_variadic(1, 2, 3, 4);
    // ... продолжаем до MAX_ARGS...
    
    
    return 0;
}