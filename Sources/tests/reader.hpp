#pragma once

#include <string>
#include <sudoku/sudoku.hpp>
#include <vector>

namespace reader
{
struct SudokuWithSolution
{
    Sudoku raw;
    Sudoku solution;
};
std::vector<SudokuWithSolution> ReadFromFile(std::string_view filePath);
}  // namespace reader