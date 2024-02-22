#include "reader.hpp"

namespace reader
{
std::vector<SudokuWithSolution> reader::ReadFromFile(std::string_view filePath)
{
    auto values = std::vector<SudokuWithSolution>();

    FILE* handle = fopen(filePath.data(), "r");
    if (handle == nullptr)
        return values;

    fseek(handle, 0, SEEK_END);
    size_t fileSize = ftell(handle);
    rewind(handle);

    char* fileContent = new char[fileSize];
    fileSize = fread(fileContent, sizeof(char), fileSize, handle);

    size_t iCurrentPos = 0;

    std::vector<u8> rawValues;
    rawValues.reserve(81);

    int currentSudoku = 0;

    while (iCurrentPos < fileSize)
    {
        const char current = fileContent[iCurrentPos];
        if (current == '\n')
        {
            iCurrentPos++;
        }
        else if (current == '.')
        {
            rawValues.push_back(0);
            iCurrentPos++;
        }
        else
        {
            rawValues.push_back(current - '0');
            iCurrentPos++;
        }

        if (rawValues.size() == 9 * 9)
        {
            if (currentSudoku % 2 == 0)
            {
                values.emplace_back();
                
                values[currentSudoku / 2].raw = Sudoku(rawValues);
            }
            else
            {
                values[currentSudoku / 2].solution = Sudoku(rawValues);
            }

            rawValues.clear();
            currentSudoku++;
        }
    }

    delete[] fileContent;
    fclose(handle);
    return values;
}
}  // namespace reader
