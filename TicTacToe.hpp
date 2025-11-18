// TicTacToe.hpp
#pragma once

#include "HashTable.hpp"
#include "DynamicArray.hpp"

#include <algorithm>
#include <limits>
#include <iostream>

struct Position {
    int x;
    int y;

    Position(int x_ = 0, int y_ = 0)
        : x(x_), y(y_) {}

    bool operator==(const Position& other) const noexcept {
        return x == other.x && y == other.y;
    }
};

// Хеш-функция для Position
struct PositionHash {
    std::size_t operator()(const Position& pos) const noexcept {
        // Cantor pairing + кодирование знака
        std::size_t a = pos.x >= 0
                        ? static_cast<std::size_t>(2 * pos.x)
                        : static_cast<std::size_t>(-2 * pos.x - 1);
        std::size_t b = pos.y >= 0
                        ? static_cast<std::size_t>(2 * pos.y)
                        : static_cast<std::size_t>(-2 * pos.y - 1);
        return (a + b) * (a + b + 1) / 2 + b;
    }
};

enum Cell {
    EMPTY = 0,
    X = 1,
    O = 2
};

class TicTacToeGame {
private:
    HashTable<Position, Cell>* board_;
    int winLength_;
    PositionHash posHash_;

    // Статистика для сравнения алгоритмов
    mutable long long nodesEvaluated_;

public:
    explicit TicTacToeGame(int winLen = 5)
        : board_(nullptr),
          winLength_(winLen),
          nodesEvaluated_(0) {

        auto hashFunc = [this](const Position& p) { return posHash_(p); };
        board_ = new HashTable<Position, Cell>(1024, hashFunc);
    }

    ~TicTacToeGame() {
        delete board_;
    }

    void Reset() {
        delete board_;
        auto hashFunc = [this](const Position& p) { return posHash_(p); };
        board_ = new HashTable<Position, Cell>(1024, hashFunc);
        nodesEvaluated_ = 0;
    }

    [[nodiscard]] Cell GetCell(int x, int y) const {
        Position pos(x, y);
        if (board_->ContainsKey(pos)) {
            return board_->Get(pos);
        }
        return EMPTY;
    }

    bool MakeMove(int x, int y, Cell player) {
        Position pos(x, y);
        if (board_->ContainsKey(pos)) {
            return false;
        }
        board_->Add(pos, player);
        return true;
    }

    [[nodiscard]] bool CheckWin(Cell player) const {
        auto keys = board_->GetKeys();

        for (const auto& pos : keys) {
            if (board_->Get(pos) != player) {
                continue;
            }

            // 4 направления: горизонталь, вертикаль, две диагонали
            int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

            for (int d = 0; d < 4; ++d) {
                int count = 1;

                // Проверяем в обе стороны
                for (int dir = -1; dir <= 1; dir += 2) {
                    int nx = pos.x + directions[d][0] * dir;
                    int ny = pos.y + directions[d][1] * dir;

                    while (GetCell(nx, ny) == player) {
                        ++count;
                        nx += directions[d][0] * dir;
                        ny += directions[d][1] * dir;
                    }
                }

                if (count >= winLength_) {
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] DynamicArray<Position> GetPossibleMoves() const {
        auto keys = board_->GetKeys();
        DynamicArray<Position> candidates;

        if (keys.empty()) {
            candidates.push_back(Position(0, 0));
            return candidates;
        }

        // Ходы вокруг уже занятых клеток
        for (const auto& pos : keys) {
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }
                    Position newPos(pos.x + dx, pos.y + dy);
                    if (!board_->ContainsKey(newPos)) {
                        candidates.push_back(newPos);
                    }
                }
            }
        }

        if (candidates.empty()) {
            return candidates;
        }

        // Удаляем дубликаты: sort + unique
        std::sort(
            candidates.begin(),
            candidates.end(),
            [](const Position& a, const Position& b) {
                return (a.x < b.x) || (a.x == b.x && a.y < b.y);
            }
        );

        auto uniqueEnd = std::unique(
            candidates.begin(),
            candidates.end(),
            [](const Position& a, const Position& b) {
                return a.x == b.x && a.y == b.y;
            }
        );

        std::size_t newSize =
            static_cast<std::size_t>(uniqueEnd - candidates.begin());
        while (candidates.size() > newSize) {
            candidates.pop_back();
        }

        return candidates;
    }

    [[nodiscard]] int EvaluatePosition(Cell player) const {
        ++nodesEvaluated_;

        if (CheckWin(X)) {
            return (player == X) ? 10000 : -10000;
        }
        if (CheckWin(O)) {
            return (player == O) ? 10000 : -10000;
        }

        int score = 0;
        auto keys = board_->GetKeys();

        // Оцениваем потенциальные линии
        int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

        for (const auto& pos : keys) {
            Cell cell = board_->Get(pos);
            if (cell == EMPTY) {
                continue;
            }

            for (int d = 0; d < 4; ++d) {
                int count = 1;
                int empty = 0;

                for (int dir = -1; dir <= 1; dir += 2) {
                    int nx = pos.x + directions[d][0] * dir;
                    int ny = pos.y + directions[d][1] * dir;

                    for (int step = 0; step < winLength_ - 1; ++step) {
                        Cell c = GetCell(nx, ny);
                        if (c == cell) {
                            ++count;
                        } else if (c == EMPTY) {
                            ++empty;
                        } else {
                            break;
                        }
                        nx += directions[d][0] * dir;
                        ny += directions[d][1] * dir;
                    }
                }

                if (count + empty >= winLength_) {
                    int lineScore = count * count * 10;
                    score += (cell == player) ? lineScore : -lineScore;
                }
            }
        }

        return score;
    }

    [[nodiscard]] Position FindBestMove(Cell player, int depth = 3) {
        nodesEvaluated_ = 0;
        int bestScore = std::numeric_limits<int>::min();
        Position bestMove{0, 0};

        DynamicArray<Position> moves = GetPossibleMoves();

        // Ограничиваем количество ходов для производительности
        std::size_t movesCount = moves.size();
        if (movesCount > 20) {
            movesCount = 20;
        }

        for (std::size_t i = 0; i < movesCount; ++i) {
            const Position& move = moves[i];

            board_->Add(move, player);
            int score = Minimax(
                depth - 1,
                /*isMaximizing=*/false,
                player,
                std::numeric_limits<int>::min(),
                std::numeric_limits<int>::max()
            );
            board_->Remove(move);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }

        return bestMove;
    }

    [[nodiscard]] long long GetNodesEvaluated() const {
        return nodesEvaluated_;
    }

    void Display(int minX, int maxX, int minY, int maxY) const {
        std::cout << "\n   ";
        for (int x = minX; x <= maxX; ++x) {
            std::cout << ' ' << (x % 10);
        }
        std::cout << '\n';

        for (int y = minY; y <= maxY; ++y) {
            std::cout << (y >= 0 ? ' ' : '-') << y << " |";
            for (int x = minX; x <= maxX; ++x) {
                Cell cell = GetCell(x, y);
                char symbol = (cell == X) ? 'X'
                                          : ((cell == O) ? 'O' : '.');
                std::cout << symbol << '|';
            }
            std::cout << '\n';
        }
    }

private:
    int Minimax(int depth, bool isMaximizing, Cell player,
                int alpha, int beta) const {
        ++nodesEvaluated_;

        if (depth == 0 || CheckWin(X) || CheckWin(O)) {
            return EvaluatePosition(player);
        }

        DynamicArray<Position> moves = GetPossibleMoves();
        if (moves.empty()) {
            return 0;
        }

        std::size_t movesCount = moves.size();
        if (movesCount > 15) {
            movesCount = 15;
        }

        Cell currentPlayer = isMaximizing ? player : (player == X ? O : X);

        if (isMaximizing) {
            int maxScore = std::numeric_limits<int>::min();
            for (std::size_t i = 0; i < movesCount; ++i) {
                const Position& move = moves[i];

                board_->Add(move, currentPlayer);
                int score = Minimax(depth - 1, false, player, alpha, beta);
                board_->Remove(move);

                if (score > maxScore) {
                    maxScore = score;
                }
                if (score > alpha) {
                    alpha = score;
                }
                if (beta <= alpha) {
                    break;
                }
            }
            return maxScore;
        } else {
            int minScore = std::numeric_limits<int>::max();
            for (std::size_t i = 0; i < movesCount; ++i) {
                const Position& move = moves[i];

                board_->Add(move, currentPlayer);
                int score = Minimax(depth - 1, true, player, alpha, beta);
                board_->Remove(move);

                if (score < minScore) {
                    minScore = score;
                }
                if (score < beta) {
                    beta = score;
                }
                if (beta <= alpha) {
                    break;
                }
            }
            return minScore;
        }
    }
};
