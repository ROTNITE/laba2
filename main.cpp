// main.cpp — ЛР-2, «Крестики-нолики на бесконечном поле»
// Стиль кода согласован с ЛР-3, но логика задачи И-13/И-13.1 сохранена.

#include "TicTacToe.hpp"

#include <iostream>
#include <chrono>
#include <fstream>
#include <limits>
#include <thread>
#include <random>
#include <string>

// Отрисовка "окна" бесконечного поля в стиле Board::print() из ЛР-3,
// НО с реальными координатами (minX..maxX, minY..maxY) на осях.
static void printBoardWindow(const TicTacToeGame& game,
                             int minX, int maxX,
                             int minY, int maxY)
{
    const int width  = maxX - minX + 1;
    const int height = maxY - minY + 1;

    // Заголовок по X: реальные координаты minX..maxX
    std::cout << "    ";            // небольшой отступ под ось Y
    for (int x = minX; x <= maxX; ++x) {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    // Строки поля по Y: реальные координаты minY..maxY
    for (int row = 0; row < height; ++row) {
        int y = minY + row;

        // Метка строки (ось Y слева)
        if (y >= 0) {
            std::cout << ' ';       // выравниваем знак для положительных
        }
        std::cout << y << " ";      // например " -5 " или "  0 "

        // Сами клетки
        for (int col = 0; col < width; ++col) {
            int x = minX + col;

            Cell cell = game.GetCell(x, y);
            char symbol =
                (cell == X) ? 'X' :
                (cell == O) ? 'O' : ' ';

            std::cout << symbol;
            if (col < width - 1) {
                std::cout << "|";
            }
        }
        std::cout << '\n';

        // Разделитель строк (как в Board::print из ЛР-3)
        if (row < height - 1) {
            std::cout << "   ";
            for (int col = 0; col < width; ++col) {
                std::cout << "-";
                if (col < width - 1) {
                    std::cout << "+";
                }
            }
            std::cout << '\n';
        }
    }
}

// === Режим: человек против компьютера ===
void playHumanVsAI() {
    std::cout << "\n=== Игра «Крестики-нолики» на бесконечном поле ===\n";
    std::cout << "Условие победы: 5 в ряд\n";

    TicTacToeGame game(5);

    // Выбор стороны
    std::cout << "\nЗа кого хотите играть?\n";
    std::cout << "1. X (вы ходите первым)\n";
    std::cout << "2. O (компьютер ходит первым)\n";
    std::cout << "Ваш выбор: ";

    int side = 0;
    while (!(std::cin >> side) || (side != 1 && side != 2)) {
        std::cout << "Ошибка. Введите 1 (X) или 2 (O): ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Cell humanCell = (side == 1) ? X : O;
    Cell aiCell    = (side == 1) ? O : X;

    bool humanTurn = (side == 1);   // если человек — X, он ходит первым

    int minX = -5, maxX = 5;
    int minY = -5, maxY = 5;

    while (true) {
        printBoardWindow(game, minX, maxX, minY, maxY);

        if (game.CheckWin(humanCell)) {
            std::cout << "\n*** Вы победили! ***\n";
            break;
        }
        if (game.CheckWin(aiCell)) {
            std::cout << "\n*** Компьютер победил! ***\n";
            break;
        }

        if (humanTurn) {
            std::cout << "\nВаш ход (" << (humanCell == X ? 'X' : 'O')
                      << "). Введите координаты (x y): ";
            int x, y;

            while (!(std::cin >> x >> y)) {
                std::cout << "Некорректный ввод! Введите два целых числа (x y): ";
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n');
            }

            if (!game.MakeMove(x, y, humanCell)) {
                std::cout << "Некорректный ход! Клетка уже занята. "
                             "Попробуйте снова.\n";
                continue;
            }

            minX = std::min(minX, x - 2);
            maxX = std::max(maxX, x + 2);
            minY = std::min(minY, y - 2);
            maxY = std::max(maxY, y + 2);
        } else {
            std::cout << "\nХод компьютера ("
                      << (aiCell == X ? 'X' : 'O') << ")...\n";

            auto start = std::chrono::high_resolution_clock::now();
            Position aiMove = game.FindBestMove(aiCell, 3);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start);

            game.MakeMove(aiMove.x, aiMove.y, aiCell);

            std::cout << "Компьютер сделал ход: ("
                      << aiMove.x << ", " << aiMove.y << ")\n";
            std::cout << "Время вычисления: " << duration.count() << " мс\n";
            std::cout << "Узлов оценено: "
                      << game.GetNodesEvaluated() << "\n";

            minX = std::min(minX, aiMove.x - 2);
            maxX = std::max(maxX, aiMove.x + 2);
            minY = std::min(minY, aiMove.y - 2);
            maxY = std::max(maxY, aiMove.y + 2);
        }

        humanTurn = !humanTurn;
    }

    printBoardWindow(game, minX, maxX, minY, maxY);
}

// === Режим: человек против человека ===
void playHumanVsHuman() {
    std::cout << "\n=== Игра «Человек против человека» ===\n";
    std::cout << "Условие победы: 5 в ряд\n";
    std::cout << "Игрок 1 — X, Игрок 2 — O\n\n";

    TicTacToeGame game(5);

    int minX = -5, maxX = 5;
    int minY = -5, maxY = 5;
    bool xTurn = true;

    while (true) {
        printBoardWindow(game, minX, maxX, minY, maxY);

        if (game.CheckWin(X)) {
            std::cout << "\n*** Победил игрок X! ***\n";
            break;
        }
        if (game.CheckWin(O)) {
            std::cout << "\n*** Победил игрок O! ***\n";
            break;
        }

        Cell current = xTurn ? X : O;
        std::cout << "\nХод игрока " << (current == X ? 'X' : 'O')
                  << ". Введите координаты (x y): ";

        int x, y;
        while (!(std::cin >> x >> y)) {
            std::cout << "Некорректный ввод! Введите два целых числа (x y): ";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (!game.MakeMove(x, y, current)) {
            std::cout << "Некорректный ход! Клетка уже занята. "
                         "Попробуйте снова.\n";
            continue;
        }

        minX = std::min(minX, x - 2);
        maxX = std::max(maxX, x + 2);
        minY = std::min(minY, y - 2);
        maxY = std::max(maxY, y + 2);

        xTurn = !xTurn;
    }

    printBoardWindow(game, minX, maxX, minY, maxY);
}

// === Сравнение глубины minimax (как было) ===
void compareAlgorithms() {
    std::cout << "\n=== Сравнение алгоритма minimax с разной глубиной ===\n\n";

    std::ofstream csv("comparison.csv");
    if (!csv.is_open()) {
        std::cout << "Не удалось открыть файл comparison.csv для записи.\n";
        return;
    }

    csv << "Глубина,Время(мс),Узлов оценено\n";

    for (int depth = 1; depth <= 4; ++depth) {
        TicTacToeGame game(5);

        // Тестовая позиция
        game.MakeMove(0, 0, X);
        game.MakeMove(1, 0, O);
        game.MakeMove(0, 1, X);
        game.MakeMove(1, 1, O);

        auto start = std::chrono::high_resolution_clock::now();
        Position move = game.FindBestMove(X, depth);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                end - start);
        long long nodes = game.GetNodesEvaluated();

        std::cout << "Глубина " << depth << ":\n";
        std::cout << "  Время: " << duration.count() << " мс\n";
        std::cout << "  Узлов оценено: " << nodes << "\n";
        std::cout << "  Лучший ход: (" << move.x << ", " << move.y << ")\n\n";

        csv << depth << "," << duration.count() << "," << nodes << "\n";
    }

    csv.close();
    std::cout << "Результаты сохранены в файл comparison.csv\n";
}

// === Демонстрационный режим: ИИ против ИИ с "рандомным началом" ===
void demoMode() {
    std::cout << "\n=== Демонстрационный режим ===\n";
    std::cout << "Компьютер играет сам с собой (X vs O).\n\n";

    std::cout << "Выберите скорость:\n";
    std::cout << "1. Быстро\n";
    std::cout << "2. Медленно (пауза между ходами)\n";
    std::cout << "3. Пошагово (по нажатию Enter)\n";

    int speedChoice;
    while (!(std::cin >> speedChoice) ||
           speedChoice < 1 || speedChoice > 3) {
        std::cout << "Некорректный выбор, введите 1, 2 или 3: ";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool slow = (speedChoice == 2);
    bool step = (speedChoice == 3);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    TicTacToeGame game(5);
    int minX = -5, maxX = 5;
    int minY = -5, maxY = 5;
    bool xTurn = true;

    // Генератор случайности для разнообразия начала партий
    std::mt19937 rng(std::random_device{}());
    int openingRandomMovesDone  = 0;
    int openingRandomMovesLimit = 2;   // 2 первых хода — случайные

    for (int moveIndex = 0; moveIndex < 40; ++moveIndex) {
        printBoardWindow(game, minX, maxX, minY, maxY);

        if (game.CheckWin(X)) {
            std::cout << "\n*** X победил! ***\n";
            break;
        }
        if (game.CheckWin(O)) {
            std::cout << "\n*** O победил! ***\n";
            break;
        }

        Cell player = xTurn ? X : O;
        std::cout << "\nХод " << (player == X ? "X" : "O") << "...\n";

        Position aiMove;

        if (openingRandomMovesDone < openingRandomMovesLimit) {
            // Случайный ход среди доступных — только в начале партии
            DynamicArray<Position> moves = game.GetPossibleMoves();
            if (!moves.empty()) {
                std::uniform_int_distribution<int> dist(
                    0,
                    static_cast<int>(moves.size()) - 1
                );
                aiMove = moves[static_cast<std::size_t>(dist(rng))];
                ++openingRandomMovesDone;

                std::cout << "Случайный начальный ход: ("
                          << aiMove.x << ", " << aiMove.y << ")\n";
            } else {
                break;
            }
        } else {
            // Нормальный minimax-ход
            aiMove = game.FindBestMove(player, 2);
            std::cout << "Ход по minimax: ("
                      << aiMove.x << ", " << aiMove.y << ")\n";
        }

        game.MakeMove(aiMove.x, aiMove.y, player);

        minX = std::min(minX, aiMove.x - 2);
        maxX = std::max(maxX, aiMove.x + 2);
        minY = std::min(minY, aiMove.y - 2);
        maxY = std::max(maxY, aiMove.y + 2);

        xTurn = !xTurn;

        // Задержка / пошаговый режим
        if (step) {
            std::cout << "Нажмите Enter, чтобы сделать следующий ход...";
            std::string dummy;
            std::getline(std::cin, dummy);
        } else if (slow) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(700));
        }
    }

    printBoardWindow(game, minX, maxX, minY, maxY);
}

void printMainMenu() {
    std::cout << "╔═══════════════════════════════════════════════╗\n";
    std::cout << "║  Крестики-нолики на бесконечном поле          ║\n";
    std::cout << "╚═══════════════════════════════════════════════╝\n\n";

    std::cout << "Главное меню:\n";
    std::cout << "1. Человек против ИИ\n";
    std::cout << "2. Демонстрация (AI vs AI)\n";
    std::cout << "3. Человек против человека\n";
    std::cout << "4. Сравнение глубины поиска minimax\n";
    std::cout << "0. Выход\n\n";
    std::cout << "Выберите опцию: ";
}

int main() {
    while (true) {
        printMainMenu();

        int choice;
        while (!(std::cin >> choice)) {
            std::cout << "Некорректный ввод, введите номер пункта меню: ";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1:
                playHumanVsAI();
                break;
            case 2:
                demoMode();
                break;
            case 3:
                playHumanVsHuman();
                break;
            case 4:
                compareAlgorithms();
                break;
            case 0:
                std::cout << "\nДо свидания!\n";
                return 0;
            default:
                std::cout << "\nНекорректный выбор!\n";
        }
    }
}
