#include "AITetris.h"

#include <SFML/Graphics.hpp>
#include <array>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace
{
    sf::Color pieceColor(int pieceType)
    {
        switch (pieceType)
        {
        case 0: return sf::Color(0x4D, 0xD3, 0xFF);
        case 1: return sf::Color(0x2E, 0xCC, 0x71);
        case 2: return sf::Color(0xE6, 0x74, 0x51);
        case 3: return sf::Color(0x9B, 0x59, 0xB6);
        case 4: return sf::Color(0xF1, 0xC4, 0x0F);
        case 5: return sf::Color(0x1F, 0x77, 0xB4);
        case 6: return sf::Color(0xFF, 0x66, 0x99);
        default: return sf::Color::White;
        }
    }
}

int main(int argc, char** argv)
{
    const bool headless = argc > 1 && std::string(argv[1]) == "--headless";
    const bool batch = argc > 1 && std::string(argv[1]) == "--batch";
    const int games = argc > 2 ? std::stoi(argv[2]) : 100;
    AITetrisGame game;

    if (headless || batch)
    {
        std::vector<int> linesList;
        std::vector<int> scoreList;
        linesList.reserve(games);
        scoreList.reserve(games);

        for (int gameIndex = 0; gameIndex < games; ++gameIndex)
        {
            AITetrisGame trial;
            while (!trial.isGameOver())
            {
                trial.step();
            }
            linesList.push_back(trial.getLines());
            scoreList.push_back(trial.getScore());
        }

        const int totalLines = std::accumulate(linesList.begin(), linesList.end(), 0);
        const int totalScore = std::accumulate(scoreList.begin(), scoreList.end(), 0);
        const double avgLines = games > 0 ? static_cast<double>(totalLines) / games : 0.0;
        const double avgScore = games > 0 ? static_cast<double>(totalScore) / games : 0.0;

        std::cout << "AI batch run complete: games=" << games
                  << ", avgLines=" << avgLines
                  << ", avgScore=" << avgScore
                  << ", bestLines=" << *std::max_element(linesList.begin(), linesList.end())
                  << ", bestScore=" << *std::max_element(scoreList.begin(), scoreList.end())
                  << std::endl;
        return 0;
    }

    sf::RenderWindow window(sf::VideoMode({900, 900}), "Tetris AI - PD");
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        if (clock.getElapsedTime().asSeconds() >= 0.25f && !game.isGameOver())
        {
            game.step();
            clock.restart();
        }

        window.clear(sf::Color(18, 18, 18));

        const auto& board = game.getBoard();
        for (int row = 0; row < AITetrisGame::kBoardHeight; ++row)
        {
            for (int col = 0; col < AITetrisGame::kBoardWidth; ++col)
            {
                const int cell = board[row][col];
                sf::RectangleShape tile(sf::Vector2f(32.0f, 32.0f));
                tile.setPosition({100.0f + col * 34.0f, 80.0f + row * 34.0f});
                tile.setFillColor(cell == 0 ? sf::Color(40, 40, 40) : pieceColor(cell - 1));
                tile.setOutlineThickness(1.0f);
                tile.setOutlineColor(sf::Color(60, 60, 60));
                window.draw(tile);
            }
        }

        sf::Font font;
        bool fontLoaded = font.openFromFile("data/Fonts/sansation.ttf");
        if (!fontLoaded)
        {
            fontLoaded = font.openFromFile("C:/Windows/Fonts/simhei.ttf");
        }
        if (!fontLoaded)
        {
            std::cerr << "Failed to load font" << std::endl;
            return 1;
        }
        sf::Text info(font, "PD AI", 24);
        info.setPosition({620.0f, 80.0f});
        window.draw(info);

        sf::Text scoreText(font, "Lines: " + std::to_string(game.getLines()), 24);
        scoreText.setPosition({620.0f, 140.0f});
        window.draw(scoreText);

        sf::Text scoreText2(font, "Score: " + std::to_string(game.getScore()), 24);
        scoreText2.setPosition({620.0f, 180.0f});
        window.draw(scoreText2);

        window.display();
    }

    return 0;
}
