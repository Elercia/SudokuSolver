#include <bit>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using i8 = std::int16_t;
using u16 = std::uint16_t;
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

using Clock = std::chrono::high_resolution_clock;

constexpr u16 AllPossibleValues = 511;

bool HasPossibleValue(u16 possibleValues, i8 number)
{
    return (possibleValues & (1 << (number - 1))) != 0;
}

u16 RemovePossibleValue(u16 possibleValues, i8 number)
{
    u16 result = possibleValues & ~(1 << (number - 1));
    // Assert(result != 0);
    return result;
}

struct Sudoku
{
    i8 m_grid[9][9] = {0};

    u16 m_possibleValues[9][9];  // Bitflied of possible values

    int m_moves = 0;

    void SetValue(int i, int j, i8 number)
    {
        m_moves++;
        m_grid[i][j] = number;
        m_possibleValues[i][j] = 0;

        RemovePossibleValuesForCol(j, number);
        RemovePossibleValuesForLine(i, number);
        RemovePossibleValuesForSection(i / 3, j / 3, number);
    }

    int LineContains(int line, i8 num)
    {
        for (int i = 0; i < 9; i++)
        {
            if (m_grid[line][i] == num)
                return i;
        }

        return -1;
    }

    int ColContains(int col, i8 num)
    {
        for (int i = 0; i < 9; i++)
        {
            if (m_grid[i][col] == num)
                return i;
        }

        return -1;
    }

    std::pair<int, int> SectionContains(int idLine, int idCol, i8 num)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (m_grid[(idLine * 3) + i][(idCol * 3) + j] == num)
                    return {(idLine * 3) + i, (idCol * 3) + j};
            }
        }

        return {-1, -1};
    }

    bool IsValid()
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                i8 number = m_grid[i][j];

                if (number != 0 && LineContains(i, number) != j)
                    return false;
                if (number != 0 && ColContains(j, number) != i)
                    return false;
                if (number != 0 && SectionContains(i / 3, j / 3, number) != std::pair(i, j))
                    return false;
            }
        }

        return true;
    }

    void FillPossibleValueFor(int line, int col)
    {
        Assert(m_grid[line][col] == 0);

        auto& possibleValueAtPos = m_possibleValues[line][col];
        possibleValueAtPos = AllPossibleValues;

        for (i8 i = 1; i <= 9; i++)
        {
            if (LineContains(line, i) != -1)
            {
                possibleValueAtPos = RemovePossibleValue(possibleValueAtPos, i);
            }
        }

        for (i8 i = 1; i <= 9; i++)
        {
            if (ColContains(col, i) != -1)
            {
                possibleValueAtPos = RemovePossibleValue(possibleValueAtPos, i);
            }
        }

        for (i8 i = 1; i <= 9; i++)
        {
            if (SectionContains(line / 3, col / 3, i) != std::pair(-1, -1))
            {
                possibleValueAtPos = RemovePossibleValue(possibleValueAtPos, i);
            }
        }
    }

    void RemoveImpossiblePossibleValues()
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (m_grid[i][j] != 0)
                    continue;

                auto& possibleValueAtPos = m_possibleValues[i][j];

                if (std::popcount(possibleValueAtPos) == 0)
                {
                    Assert(false);
                }
                else
                {
                    int sectionLine = i / 3;
                    int sectionCol = j / 3;

                    for (i8 number = 1; number <= 9; number++)
                    {
                        if (!HasPossibleValue(possibleValueAtPos, number))
                            continue;

                        bool sameLine = true;
                        bool sameCol = true;

                        for (int iSectionLine = 0; iSectionLine < 3; iSectionLine++)
                        {
                            for (int iSectionCol = 0; iSectionCol < 3; iSectionCol++)
                            {
                                auto& possibleValuesSection =
                                    m_possibleValues[(sectionLine * 3) + iSectionLine][(sectionCol * 3) + iSectionCol];

                                if (HasPossibleValue(possibleValuesSection, number))
                                {
                                    if ((sectionLine * 3) + iSectionLine != i)
                                        sameLine = false;

                                    if ((sectionCol * 3) + iSectionCol != j)
                                        sameCol = false;
                                }
                            }
                        }

                        if (sameCol)
                        {
                            for (int it = 0; it < 9; it++)
                            {
                                if (it / 3 != sectionLine)
                                {
                                    auto& thisPossibleValues = m_possibleValues[it][j];

                                    if (HasPossibleValue(thisPossibleValues, number))
                                    {
                                        thisPossibleValues = RemovePossibleValue(thisPossibleValues, number);
                                    }
                                }
                            }
                        }

                        if (sameLine)
                        {
                            for (int it = 0; it < 9; it++)
                            {
                                if (it / 3 != sectionCol)
                                {
                                    auto& thisPossibleValues = m_possibleValues[i][it];

                                    if (HasPossibleValue(thisPossibleValues, number))
                                    {
                                        thisPossibleValues = RemovePossibleValue(thisPossibleValues, number);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void FillPossibleValues()
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (m_grid[i][j] == 0)
                {
                    FillPossibleValueFor(i, j);
                }
            }
        }

        RemoveImpossiblePossibleValues();
    }

    bool ValidatePossibleValues()
    {
        bool hasChanges = false;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (m_grid[i][j] != 0)
                    continue;

                auto& possibleValueAtPos = m_possibleValues[i][j];
                auto bitCount = std::popcount(possibleValueAtPos);

                if (bitCount == 0)
                {
                    Assert(false);
                }
                else if (bitCount == 1)
                {
                    // gives the first bit set
                    u16 tmp = possibleValueAtPos ^ (~-possibleValueAtPos);
                    i8 number = (i8) std::popcount(tmp);  //(i8) log2(possibleValueAtPos & -possibleValueAtPos) +
                                                                // 1;
                    SetValue(i, j, number);

                    hasChanges = true;
                }
                else
                {
                    int sectionIdLine = i / 3;
                    int sectionIdCol = j / 3;

                    bool bAlone = true;

                    for (i8 number = 1; number <= 9; number++)
                    {
                        if (!HasPossibleValue(possibleValueAtPos, number))
                            continue;

                        for (int sectionLine = 0; sectionLine < 3; sectionLine++)
                        {
                            for (int sectionCol = 0; sectionCol < 3; sectionCol++)
                            {
                                int otherLine = (sectionIdLine * 3) + sectionLine;
                                int otherCol = (sectionIdCol * 3) + sectionCol;

                                if (otherLine == i && otherCol == j)
                                    continue;

                                auto& otherPossibleValues = m_possibleValues[otherLine][otherCol];

                                if (HasPossibleValue(otherPossibleValues, number))
                                {
                                    bAlone = false;
                                    break;
                                }
                            }
                            if (bAlone == false)
                                break;
                        }

                        if (bAlone)
                        {
                            hasChanges = true;

                            SetValue(i, j, number);

                            break;
                        }
                    }
                }
            }
        }

        return hasChanges;
    }

    bool IsFinito()
    {
        return BlankValues() == 0;
    }

    int BlankValues()
    {
        int blankValues = 0;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (m_grid[i][j] == 0)
                    blankValues++;
            }
        }

        return blankValues;
    }

    void Print()
    {
        std::string s;
        s.reserve(9 * 9 + 9 * 9 + 9);

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                s += std::to_string(m_grid[i][j]);
                s += " ";
            }
            s += "\n";
        }

        std::cout << s << "\nDone in " << m_moves << " moves\n";
    }

    bool Solve()
    {
        FillPossibleValues();

        do
        {
            RemoveImpossiblePossibleValues();

        } while (ValidatePossibleValues());

        return IsFinito();
    }

    void RemovePossibleValuesForCol(int col, i8 number)
    {
        for (int line = 0; line < 9; line++)
        {
            auto& possibleValues = m_possibleValues[line][col];
            possibleValues = RemovePossibleValue(possibleValues, number);
        }
    }

    void RemovePossibleValuesForLine(int line, i8 number)
    {
        for (int col = 0; col < 9; col++)
        {
            auto& possibleValues = m_possibleValues[line][col];
            possibleValues = RemovePossibleValue(possibleValues, number);
        }
    }

    void RemovePossibleValuesForSection(int sectionLineId, int sectionColId, i8 number)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int line = (sectionLineId * 3) + i;
                int col = (sectionColId * 3) + j;

                auto& possibleValues = m_possibleValues[line][col];
                possibleValues = RemovePossibleValue(possibleValues, number);
            }
        }
    }
};

int main()
{
    bool solved = false;
    bool bTest = true;

    Clock::time_point begin = Clock::now();

    for (int tryNb = 0; tryNb < (bTest ? 1000000 : 1); tryNb++)
    {
        // clang-format off
        Sudoku s = {
        {
        {0,0,9,     0,0,5,      0,1,0},
        {0,8,5,     1,0,0,      3,4,0},
        {0,0,7,     0,4,2,      0,0,8},

        {0,5,0,     0,2,1,      8,6,0},
        {0,0,6,     5,0,0,      2,0,0},
        {8,0,0,     0,6,4,      5,9,0},

        {0,0,0,     0,5,3,      0,0,0},
        {4,0,0,     8,7,6,      1,3,5},
        {0,0,0,     0,1,0,      0,0,0}
        }
        /*{
        {0,0,0,     4,6,0,      0,7,1},
        {7,0,0,     0,1,8,      0,0,0},
        {0,0,6,     0,7,0,      0,0,0},

        {0,0,0,     0,4,0,      7,3,0},
        {0,8,0,     0,0,0,      2,4,0},
        {4,2,0,     0,0,9,      0,0,0},

        {0,6,8,     0,9,0,      0,0,4},
        {0,0,5,     6,0,0,      0,1,0},
        {0,0,2,     1,0,0,      0,5,0}
        }*/
        };
        // clang-format on

        Assert(s.IsValid());

        solved = s.Solve();
    }

    Clock::time_point end = Clock::now();

    auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    auto durationNS = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

    std::cout << "Finished in " << (durationMS > 0 ? durationMS : durationNS) << (durationMS > 0 ? " ms" : " ns");

    return solved ? 0 : -1;
}