#include "catch2/catch2.hpp"
#include "sudoku/sudoku.hpp"
#include "tests/reader.hpp"

#include <iostream>

bool PossibleValueEquals(u16 possibleValueField, std::initializer_list<u16> values)
{
    for (u16 value : values)
    {
        if ((possibleValueField & 1 << (value - 1)) == 0)
            return false;

        possibleValueField = (possibleValueField & ~(1 << (value - 1)));
    }

    return possibleValueField == 0;
}

TEST_CASE("possible values", "[possible values]")
{
    SudokuSolver solver;
    Sudoku sudoku({
        1, 0, 0,    0, 0, 7,    0, 9, 0,
        0, 3, 0,    0, 2, 0,    0, 0, 8, 
        0, 0, 9,    6, 0, 0,    5, 0, 0, 
        
        0, 0, 5,    3, 0, 0,    9, 0, 0, 
        0, 1, 0,    0, 8, 0,    0, 0, 2, 
        6, 0, 0,    0, 0, 4,    0, 0, 0, 
        
        3, 0, 0,    0, 0, 0,    0, 1, 0, 
        0, 4, 1,    0, 0, 0,    0, 0, 7, 
        0, 0, 7,    0, 0, 0,    3, 0, 0, 
    });

    solver.InitializePossibleValues(sudoku);

    auto possibleValues = solver.GetPossibleValues();
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 1], {2, 5, 6, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 2], {2, 4, 6, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 3], {4, 5, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 4], {3, 4, 5}));
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 6], {2, 4, 6}));
    REQUIRE(PossibleValueEquals(possibleValues[0 * 9 + 8], {3, 4, 6}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 0], {4, 5, 7}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 2], {4, 6}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 3], {1, 4, 5, 9}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 5], {1, 5, 9}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 6], {1, 4, 6, 7}));
    REQUIRE(PossibleValueEquals(possibleValues[1 * 9 + 7], {4, 6, 7}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 0], {2, 4, 7, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 1], {2, 7, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 4], {1, 3, 4}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 5], {1, 3, 8}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 7], {2, 3, 4, 7}));
    REQUIRE(PossibleValueEquals(possibleValues[2 * 9 + 8], {1, 3, 4}));
}

void TestSoldSudokuList(std::string_view name)
{
    SudokuSolver solver;
    std::vector<reader::SudokuWithSolution> values = reader::ReadFromFile(name);
    int unsolvedCount = 0;
    for (int i = 0; i < values.size(); i++)
    {
        auto& sudokuInitializer = values[i];
        Sudoku s(sudokuInitializer.raw);

        if (!solver.Solve(s))
        {
            std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>\n";

            s.Print();

            std::cout << "\n";

            sudokuInitializer.solution.Print();

            std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<\n";

            unsolvedCount++;
        }
    }

    REQUIRE(unsolvedCount == 0);
}

TEST_CASE("Simple sudoku", "[solver]")
{
    TestSoldSudokuList("simple.txt");
}

TEST_CASE("Easy sudoku", "[solver]")
{
    TestSoldSudokuList("easy.txt");
}

TEST_CASE("Medium sudoku", "[solver]")
{
    TestSoldSudokuList("medium.txt");
}