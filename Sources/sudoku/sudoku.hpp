#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

using u8 = std::int8_t;
using u16 = std::uint16_t;

struct Sudoku
{
    static const u8 SUDOKU_GRID_SIZE = 9 * 9;
    static const u8 INVALID_CELL_VALUE = 0;

    u8 m_grid[9][9];
    Sudoku();
    Sudoku(std::initializer_list<u8> values);
    Sudoku(std::vector<u8> values);

    Sudoku(const Sudoku& other);
    Sudoku& operator=(const Sudoku& other);

    void Print() const;
    bool IsFinito() const;
};

using Truc = int[9][9];

class SudokuSolver
{
public:
    SudokuSolver();

    void InitializePossibleValues(Sudoku& sodoku);
    u16* GetPossibleValues() const;

    bool Solve(Sudoku& sodoku);

private:
    void PutValueWherePossible();
    void RemovePossibleValuesForColumn(int col, u8 number);
    void RemovePossibleValuesForLine(int line, u8 number);
    void RemovePossibleValuesForSection(int sectionLineId, int sectionColId, u8 number);
    bool HasPossibleValue(u16 possibleValues, u8 number);
    void RemovePossibleValue(u16& possibleValues, u8 number);

    void AssertState() const;

private:
    
    Sudoku* m_currentSudoku;
    u16 m_possibleValues[9][9];  // Bitflied of possible values

    int m_cellsWithOnlyOnePossibleValue;
    int m_cellToGuessCount;
};