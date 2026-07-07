#pragma once

#include <array>
#include <string>
#include <vector>

class AITetrisGame
{
public:
    struct AIConfig
    {
        int lineClearWeight = 5000;
        int aggregateHeightWeight = 3;
        int holesWeight = 100;
        int bumpinessWeight = 10;
        int maxHeightWeight = 8;
    };

    static constexpr int kBoardWidth = 10;
    static constexpr int kBoardHeight = 24;
    using Board = std::array<std::array<int, kBoardWidth>, kBoardHeight>;

    struct Move
    {
        int rotation = 0;
        int x = 0;
        int landingY = 0;
        int score = -1000000;
    };

    AITetrisGame();
    void reset();
    bool step();
    int getScore() const;
    int getLines() const;
    bool isGameOver() const;
    const Board& getBoard() const;
    int getCurrentPiece() const;
    int getNextPiece() const;

    static Move chooseBestMoveForBoard(const Board& board, int pieceType, int startingRotation, int startX, int startY, const AIConfig& config);

private:
    Board board_{};
    std::array<std::array<std::array<int, 2>, 4>, 7> baseShapes_{};
    std::vector<int> bag_;
    int bagIndex_ = 0;
    int currentPiece_ = 0;
    int nextPiece_ = 0;
    int score_ = 0;
    int lines_ = 0;
    bool gameOver_ = false;

    int nextPieceType();
    std::vector<std::array<std::array<int, 2>, 4>> rotationsForPiece(int pieceType) const;
    bool canPlace(const Board& board, int pieceType, int rotation, int x, int y) const;
    bool canPlace(int pieceType, int rotation, int x, int y) const;
    static int clearLinesInBoard(Board& board);
    Move chooseBestMove(int pieceType);
    int evaluatePlacement(const Board& board, int pieceType, int rotation, int x, int y, int linesCleared) const;
};
