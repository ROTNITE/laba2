// tests_lab2.cpp — автономные тесты для ЛР-2 (бесконечное поле)

#include "TicTacToe.hpp"   // здесь должны быть Position, PositionHash, TicTacToeGame, Cell и т.п.
// Если HashTable/HashMap в отдельном хедере — раскомментируй и поправь имя:
// #include "HashTable.hpp"

#include <iostream>
#include <cassert>

class tests_all {
public:
    static void RunAllTests() {
        std::cout << "=== Запуск тестов ЛР-2 ===\n\n";

        TestHashTable();
        TestBasicMoves();
        TestWinDetection();
        TestAIMove();
        TestBoundaries();

        std::cout << "\n=== Все 5/5 тестов пройдены успешно! ===\n";
        std::cout << "(Если бы какой-то тест упал, сработал бы assert)\n\n";
    }

private:
    static void TestHashTable() {
        std::cout << "Тест 1: Хеш-таблица... ";

        PositionHash ph;
        auto hashFunc = [&ph](const Position& p) { return ph(p); };
        HashTable<Position, int> ht(16, hashFunc);

        Position p1(5, 10);
        Position p2(-3, 7);
        Position p3(100, -50);

        ht.Add(p1, 1);
        ht.Add(p2, 2);
        ht.Add(p3, 3);

        assert(ht.GetCount() == 3);
        assert(ht.Get(p1) == 1);
        assert(ht.Get(p2) == 2);
        assert(ht.Get(p3) == 3);
        assert(ht.ContainsKey(p1));
        assert(!ht.ContainsKey(Position(999, 999)));

        ht.Remove(p2);
        assert(!ht.ContainsKey(p2));
        assert(ht.GetCount() == 2);

        std::cout << "OK\n";
    }

    static void TestBasicMoves() {
        std::cout << "Тест 2: Базовые ходы... ";

        TicTacToeGame game(5);

        assert(game.MakeMove(0, 0, X));
        assert(game.GetCell(0, 0) == X);
        assert(!game.MakeMove(0, 0, O)); // Занятая клетка
        assert(game.MakeMove(1, 1, O));
        assert(game.GetCell(1, 1) == O);

        // Отрицательные координаты
        assert(game.MakeMove(-5, -3, X));
        assert(game.GetCell(-5, -3) == X);

        std::cout << "OK\n";
    }

    static void TestWinDetection() {
        std::cout << "Тест 3: Определение победы... ";

        TicTacToeGame game(5);

        // Горизонтальная линия
        for (int i = 0; i < 5; ++i) {
            game.MakeMove(i, 0, X);
        }
        assert(game.CheckWin(X));
        assert(!game.CheckWin(O));

        game.Reset();

        // Вертикальная линия
        for (int i = 0; i < 5; ++i) {
            game.MakeMove(0, i, O);
        }
        assert(game.CheckWin(O));

        game.Reset();

        // Диагональная
        for (int i = 0; i < 5; ++i) {
            game.MakeMove(i, i, X);
        }
        assert(game.CheckWin(X));

        game.Reset();

        // Обратная диагональ
        for (int i = 0; i < 5; ++i) {
            game.MakeMove(i, 4 - i, O);
        }
        assert(game.CheckWin(O));

        std::cout << "OK\n";
    }

    static void TestAIMove() {
        std::cout << "Тест 4: AI находит ходы... ";

        TicTacToeGame game(5);

        // AI должен найти ход в центр на пустой доске
        Position aiMove = game.FindBestMove(X, 2);
        assert(aiMove.x == 0 && aiMove.y == 0);

        game.MakeMove(0, 0, X);
        game.MakeMove(1, 0, O);

        // AI должен ходить в свободную клетку
        aiMove = game.FindBestMove(X, 2);
        assert(game.GetCell(aiMove.x, aiMove.y) == EMPTY);

        std::cout << "OK\n";
    }

    static void TestBoundaries() {
        std::cout << "Тест 5: Граничные случаи... ";

        TicTacToeGame game(5);

        // Большие координаты
        assert(game.MakeMove(1000, 1000, X));
        assert(game.GetCell(1000, 1000) == X);

        // Отрицательные координаты
        assert(game.MakeMove(-1000, -1000, O));
        assert(game.GetCell(-1000, -1000) == O);

        // Смешанные
        assert(game.MakeMove(500, -500, X));
        assert(game.GetCell(500, -500) == X);

        std::cout << "OK\n";
    }
};

int main() {
    tests_all::RunAllTests();
    return 0;
}
