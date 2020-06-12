## Big integer

* Класс длинное знаковое число.
* Реализация содержится в классе `big_integer` и находится в файле `big_integer.h`. Класс содержит:
	* Конструктор по умолчанию, инициализирующий число нулем.
	* Конструктор копирования, после которого исходный объект и копию можно изменять независимо.
	* Конструктор от int.
	* Explicit конструктор от `std::string`.
 	* Оператор присваивания.
	* Операторы сравнения.
	* Арифметические операции: сложение, вычитание, изменение знака (унарный минус), унарный плюс.
	* Умножение работающее за время `O(nm)`, где `n`,`m` — длины множителей в битах.
	* Деление и остаток от деления работающие за время `O(nm)`, где `n` — длина делителя в битах, а `m` — длина частного в битах.
	* Префиксный/постфиксный инкремент/декремент.
	* Битовые операции: и, или, исключающее или, не.
	* Битовые сдвиги.
* Существует глобальная функция `std::string to_string(big_integer const&)` возвращающая строковое представление числа.
* Реализация функций класса расположена в файле `big_integer.cpp`.
* Не используются сторонние библиотеками длинных чисел.
* Разряды числа представляются 32-битными числами.
* Реализация использует small-object и copy-on-write оптимизации.
* Если в `big_integer` хранится число -2^32 ≤ a ≤ 2^32 - 1, `big_integer` не выделяет динамическую память.
* Числа a < -2^32 и a > 2^32 - 1 выделяют не больше одного блока динамической памяти на каждый экземпляр `big_integer`.
* `big_integer_testing.cpp` — набор модульных тестов для проверки корректности программы. Тесты реализованы с помощью библиотеки gtest (Google Test).
* gtest/ — библиотека [gtest](https://github.com/google/googletest).
