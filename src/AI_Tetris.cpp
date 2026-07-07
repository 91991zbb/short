#include "AITetris.h"

#include <algorithm>
#include <cstdlib>
#include <random>
#include <stdexcept>

namespace
{
    std::array<std::array<int, 2>, 4> rotateCells(const std::array<std::array<int, 2>, 4>& cells)
    {
        std::array<std::array<int, 2>, 4> rotated{};
        for (int i = 0; i < 4; ++i)
        {
            rotated[i][0] = -cells[i][1];
            rotated[i][1] = cells[i][0];
        }
        return rotated;
    }
}

AITetrisGame::AITetrisGame()
{
    reset();
}

void AITetrisGame::reset()
{
    board_ = {};
    bag_.clear();
    bagIndex_ = 0;
    score_ = 0;
    lines_ = 0;
    gameOver_ = false;

    baseShapes_ = {{{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
                    {{{1, 0}, {2, 0}, {0, 1}, {1, 1}}},
                    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
                    {{{1, 0}, {0, 1}, {1, 1}, {2, 1}}},
                    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}},
                    {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
                    {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}}};

    currentPiece_ = nextPieceType();
    nextPiece_ = nextPieceType();
}

int AITetrisGame::nextPieceType()
{
    if (bag_.empty())
    {
        bag_ = {0, 1, 2, 3, 4, 5, 6};
        std::shuffle(bag_.begin(), bag_.end(), std::mt19937{std::random_device{}()});
    }

    int pieceType = bag_.back();
    bag_.pop_back();
    return pieceType;
}

std::vector<std::array<std::array<int, 2>, 4>> AITetrisGame::rotationsForPiece(int pieceType) const
{
    std::vector<std::array<std::array<int, 2>, 4>> rotations;
    std::array<std::array<int, 2>, 4> cells{};
    for (int i = 0; i < 4; ++i)
    {
        cells[i] = baseShapes_[pieceType][i];
    }

    for (int i = 0; i < 4; ++i)
    {
        rotations.push_back(cells);
        cells = rotateCells(cells);
    }

    if (pieceType == 6)
    {
        rotations = {cells, cells, cells, cells};
    }
    return rotations;
}

bool AITetrisGame::canPlace(const Board& board, int pieceType, int rotation, int x, int y) const
{
    const auto rotations = rotationsForPiece(pieceType);
    const auto& cells = rotations[rotation % 4];

    for (const auto& cell : cells)
    {
        const int px = x + cell[0];
        const int py = y + cell[1];
        if (px < 0 || px >= kBoardWidth || py >= kBoardHeight)
        {
            return false;
        }
        if (py >= 0 && board[py][px] != 0)
        {
            return false;
        }
    }
    return true;
}

bool AITetrisGame::canPlace(int pieceType, int rotation, int x, int y) const
{
    return canPlace(board_, pieceType, rotation, x, y);
}

int AITetrisGame::clearLinesInBoard(Board& board)
{
    int cleared = 0;
    int writeRow = kBoardHeight - 1;

    for (int readRow = kBoardHeight - 1; readRow >= 0; --readRow)
    {
        bool full = true;
        for (int col = 0; col < kBoardWidth; ++col)
        {
            if (board[readRow][col] == 0)
            {
                full = false;
                break;
            }
        }

        if (!full)
        {
            board[writeRow] = board[readRow];
            --writeRow;
        }
        else
        {
            ++cleared;
        }
    }

    for (int row = writeRow; row >= 0; --row)
    {
        board[row] = {};
    }

    return cleared;
}

AITetrisGame::Move AITetrisGame::chooseBestMove(int pieceType)
{
    AIConfig config;
    return chooseBestMoveForBoard(board_, pieceType, 0, 0, 0, config);
}

AITetrisGame::Move AITetrisGame::chooseBestMoveForBoard(const Board& board, int pieceType, int startingRotation, int startX, int startY, const AIConfig& config)
{
    (void)startingRotation;
    (void)startX;
    (void)startY;

    Move best;
    const AITetrisGame tempGame;
    const auto rotations = tempGame.rotationsForPiece(pieceType);

    for (int rotation = 0; rotation < 4; ++rotation)
    {
        for (int x = 0; x < kBoardWidth; ++x)
        {
            int y = 0;
            while (true)
            {
                if (!tempGame.canPlace(board, pieceType, rotation, x, y + 1))
                {
                    break;
                }
                ++y;
            }

            if (!tempGame.canPlace(board, pieceType, rotation, x, y))
            {
                continue;
            }

            Board simulation = board;
            const auto& cells = rotations[rotation % 4];
            for (const auto& cell : cells)
            {
                const int px = x + cell[0];
                const int py = y + cell[1];
                if (py >= 0)
                {
                    simulation[py][px] = pieceType + 1;
                }
            }

            const int cleared = tempGame.clearLinesInBoard(simulation);
            const int score = tempGame.evaluatePlacement(simulation, pieceType, rotation, x, y, cleared);
            if (score > best.score)
            {
                best.rotation = rotation;
                best.x = x;
                best.landingY = y;
                best.score = score;
            }
        }
    }
    return best;
}

int AITetrisGame::evaluatePlacement(const Board& board, int pieceType, int rotation, int x, int y, int linesCleared) const
{
    (void)pieceType;
    (void)rotation;
    (void)x;
    (void)y;

    std::array<int, kBoardWidth> heights{};
    for (int col = 0; col < kBoardWidth; ++col)
    {
        for (int row = 0; row < kBoardHeight; ++row)
        {
            if (board[row][col] != 0)
            {
                heights[col] = kBoardHeight - row;
                break;
            }
        }
    }

    int aggregateHeight = 0;
    int maxHeight = 0;
    int holes = 0;
    int bumpiness = 0;
    for (int col = 0; col < kBoardWidth; ++col)
    {
        aggregateHeight += heights[col];
        maxHeight = std::max(maxHeight, heights[col]);

        bool seenBlock = false;
        for (int row = 0; row < kBoardHeight; ++row)
        {
            if (board[row][col] != 0)
            {
                seenBlock = true;
                continue;
            }
            if (seenBlock)
            {
                ++holes;
            }
        }
    }

    for (int col = 0; col < kBoardWidth - 1; ++col)
    {
        bumpiness += std::abs(heights[col] - heights[col + 1]);
    }

    return linesCleared * 5000 - aggregateHeight * 3 - holes * 100 - bumpiness * 10 - maxHeight * 8;
}

bool AITetrisGame::step()
{
    if (gameOver_)
    {
        return false;
    }

    currentPiece_ = nextPiece_;
    nextPiece_ = nextPieceType();

    const Move move = chooseBestMove(currentPiece_);
    if (move.score < -1000000)
    {
        gameOver_ = true;
        return false;
    }

    const auto rotations = rotationsForPiece(currentPiece_);
    const auto& cells = rotations[move.rotation % 4];
    for (const auto& cell : cells)
    {
        const int px = move.x + cell[0];
        const int py = move.landingY + cell[1];
        if (py >= 0)
        {
            board_[py][px] = currentPiece_ + 1;
        }
    }

    const int cleared = clearLinesInBoard(board_);
    score_ += cleared * 100;
    lines_ += cleared;

    if (!canPlace(nextPiece_, 0, 3, 0))
    {
        gameOver_ = true;
    }

    return true;
}

int AITetrisGame::getScore() const
{
    return score_;
}

int AITetrisGame::getLines() const
{
    return lines_;
}

bool AITetrisGame::isGameOver() const
{
    return gameOver_;
}

const AITetrisGame::Board& AITetrisGame::getBoard() const
{
    return board_;
}

int AITetrisGame::getCurrentPiece() const
{
    return currentPiece_;
}

int AITetrisGame::getNextPiece() const
{
    return nextPiece_;
}
