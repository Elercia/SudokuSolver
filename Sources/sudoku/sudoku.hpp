#pragma once

#include <cstdint>
#include <initializer_list>

using u8 = std::int8_t;
using u16 = std::uint16_t;

class Sudoku
{
public:
    Sudoku(std::initializer_list<u8> values);

    void Print() const;
    bool IsFinito() const;

    bool Solve();

private:
    void InitializePossibleValues();
    void PutValueWherePossible();
    void RemovePossibleValuesForColumn(int col, u8 number);
    void RemovePossibleValuesForLine(int line, u8 number);
    void RemovePossibleValuesForSection(int sectionLineId, int sectionColId, u8 number);

private:
    static const u8 SUDOKU_GRID_SIZE = 9 * 9;
    static const u8 INVALID_CELL_VALUE = 0;

    u8 m_grid[9][9];
    u16 m_possibleValues[9][9];  // Bitflied of possible values

    int m_cellsWithOnlyOnePossibleValue;
    int m_cellToGuessCount;
};