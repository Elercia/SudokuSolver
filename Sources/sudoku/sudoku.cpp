#include "sudoku.hpp"

#include <bit>
#include <iostream>
#include <string>

#ifdef APP_DEBUG
#define Assert(x)                                                 \
    do                                                            \
    {                                                             \
        if (!(x))                                                 \
        {                                                         \
            __debugbreak();                                       \
            std::cout << "Assertion failed :" << #x << std::endl; \
            exit(-255);                                           \
        }                                                         \
    } while (false)
#else
#define Assert(X)
#endif

constexpr u16 AllPossibleValues = 511;

bool HasPossibleValue(u16 possibleValues, u8 number)
{
    return (possibleValues & (1 << (number - 1))) != 0;
}

void RemovePossibleValue(u16& possibleValues, u8 number)
{
    possibleValues = possibleValues & ~(1 << (number - 1));
}

Sudoku::Sudoku(std::initializer_list<u8> values)
{
    memset(m_grid, INVALID_CELL_VALUE, sizeof(u8) * SUDOKU_GRID_SIZE);
    memset(m_possibleValues, INVALID_CELL_VALUE, sizeof(u8) * SUDOKU_GRID_SIZE);

    if (values.size() == 9 * 9)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                m_grid[i][j] = *(values.begin() + i * 9 + j);
            }
        }
    }

    m_cellsWithOnlyOnePossibleValue = 0;
    m_cellToGuessCount = 0;
}

void Sudoku::Print() const
{
    std::string s;
    s.resize(9 * 9 + 9 * 9 + 9);  // enough space for the numbers, spaces and "\n"

    for (int i = 0; i < 9; i++)
    {
        std::sprintf(&s[i * 9 * 2 + 1], "%d %d %d %d %d %d %d %d %d\n", m_grid[i][0], m_grid[i][1], m_grid[i][2],
                     m_grid[i][3], m_grid[i][4], m_grid[i][5], m_grid[i][6], m_grid[i][7], m_grid[i][8]);
    }

    std::cout << s << "\n";
}

bool Sudoku::Solve()
{
    // First, set all the possible values for each cells
    InitializePossibleValues();

    // First step, put the right number on each position where there is only one possibility
    do
    {
        PutValueWherePossible();
    } while (m_cellsWithOnlyOnePossibleValue != 0);
    return IsFinito();
}

void Sudoku::InitializePossibleValues()
{
    u16 possibleValuePerSection[3][3];
    u16 possibleValuePerLine[9];
    u16 possibleValuePerCol[9];

    for (int i = 0; i < 9; i++)
    {
        *((u16*) possibleValuePerSection + i) = AllPossibleValues;
        possibleValuePerLine[i] = AllPossibleValues;
        possibleValuePerCol[i] = AllPossibleValues;
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            u8 value = m_grid[i][j];
            if (value != INVALID_CELL_VALUE)
            {
                RemovePossibleValue(possibleValuePerLine[i], value);
                RemovePossibleValue(possibleValuePerCol[j], value);

                const int sectionLine = i / 3;
                const int sectionCol = j / 3;
                RemovePossibleValue(possibleValuePerSection[sectionLine][sectionCol], value);
            }
        }
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (m_grid[i][j] != INVALID_CELL_VALUE)
            {
                continue;
            }

            m_cellToGuessCount++;

            m_possibleValues[i][j] =
                possibleValuePerSection[i / 3][j / 3] & possibleValuePerLine[i] & possibleValuePerCol[j];

            if (std::popcount(m_possibleValues[i][j]) == 1)
                m_cellsWithOnlyOnePossibleValue++;
        }
    }
}

void Sudoku::PutValueWherePossible()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (m_grid[i][j] != INVALID_CELL_VALUE)
                continue;

            if (std::popcount(m_possibleValues[i][j]) == 1)
            {
                u8 number = (u8) std::countr_zero(m_possibleValues[i][j]) + 1;

                m_possibleValues[i][j] = 0;
                m_grid[i][j] = number;

                Assert(m_cellToGuessCount >= 1);
                Assert(m_cellsWithOnlyOnePossibleValue >= 1);

                m_cellToGuessCount--;
                m_cellsWithOnlyOnePossibleValue--;

                RemovePossibleValuesForLine(i, number);
                RemovePossibleValuesForColumn(j, number);
                RemovePossibleValuesForSection(i / 3, j / 3, number);
            }
        }
    }
}

void Sudoku::RemovePossibleValuesForColumn(int col, u8 number)
{
    for (int line = 0; line < 9; line++)
    {
        auto& possibleValues = m_possibleValues[line][col];

        if (HasPossibleValue(possibleValues, number) && std::popcount(possibleValues) == 2)
            m_cellsWithOnlyOnePossibleValue++;

        RemovePossibleValue(possibleValues, number);

        Assert(possibleValues != 0 || m_grid[line][col] != INVALID_CELL_VALUE);
    }
}

void Sudoku::RemovePossibleValuesForLine(int line, u8 number)
{
    for (int col = 0; col < 9; col++)
    {
        auto& possibleValues = m_possibleValues[line][col];
        if (HasPossibleValue(possibleValues, number) && std::popcount(possibleValues) == 2)
            m_cellsWithOnlyOnePossibleValue++;

        RemovePossibleValue(possibleValues, number);

        Assert(possibleValues != 0 || m_grid[line][col] != INVALID_CELL_VALUE);
    }
}

void Sudoku::RemovePossibleValuesForSection(int sectionLineId, int sectionColId, u8 number)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int line = (sectionLineId * 3) + i;
            int col = (sectionColId * 3) + j;

            auto& possibleValues = m_possibleValues[line][col];
            if (HasPossibleValue(possibleValues, number) && std::popcount(possibleValues) == 2)
                m_cellsWithOnlyOnePossibleValue++;

            RemovePossibleValue(possibleValues, number);

            Assert(possibleValues != 0 || m_grid[line][col] != INVALID_CELL_VALUE);
        }
    }
}

bool Sudoku::IsFinito() const
{
    return m_cellToGuessCount == 0;
}