/*
 * Program: EECS 348 Assignment 5 (revision of Assignment 4)
 * Description: Loads five Sudoku puzzles, finds every solution using recursive
 *              backtracking, displays results, and saves each puzzle's solutions.
 * Inputs: puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
 * Outputs: Console results and solution1.txt through solution5.txt
 * Original author: Prabesh Sapkota
 * Original creation date: October 15, 2026
 * Revised by: Conor Rothe
 * Revision date: October 29, 2026
 * Collaborators: Original Assignment 4 author (credited above)
 * Other sources: Original code cites ChatGPT (OpenAI). This revision builds
 *                on AI-generated Code 2 (provider identified in the GenAI
 *                analysis) with additional ChatGPT (OpenAI) assistance.
 * Revision notes: Reused candidate checks, strengthened file error handling,
 *                 and preserved the original console behavior.
 */

#include <iostream> // Provides console input and output.
#include <fstream> // Provides file-reading tools.
#include <string> // Provides the string type.
#include <vector> // Stores and validates the puzzle's 81 input cells.
#include <cctype> // Provides the isspace function.
using namespace std; // Allows standard-library names without the std:: prefix.

class SudokuSolver { // Defines a class that stores and solves one Sudoku puzzle.
private: // Begins the private section of the class.
    int board[9][9]; // Stores the Sudoku grid; zero represents an empty cell.
    int solutionCount; // Counts how many complete solutions have been found.
    ofstream outputFile; // Writes each solution to the current puzzle's solutionN.txt file.

    bool isSafe(int row, int col, int digit) { // Checks whether digit can go in the specified cell.
        for (int j = 0; j < 9; j++) { // Checks all cells in the selected row.
            if (board[row][j] == digit) { // Detects the same digit in that row.
                return false; // Rejects a digit that already appears in the row.
            } // Ends the row-conflict condition.
        } // Ends the row scan.

        for (int i = 0; i < 9; i++) { // Checks all cells in the selected column.
            if (board[i][col] == digit) { // Detects the same digit in that column.
                return false; // Rejects a digit that already appears in the column.
            } // Ends the column-conflict condition.
        } // Ends the column scan.

        int startRow = (row / 3) * 3; // Finds the first row of the cell's 3-by-3 block.
        int startCol = (col / 3) * 3; // Finds the first column of the cell's 3-by-3 block.

        for (int i = startRow; i < startRow + 3; i++) { // Visits the three rows of the block.
            for (int j = startCol; j < startCol + 3; j++) { // Visits the three columns of the block.
                if (board[i][j] == digit) { // Detects the same digit inside the block.
                    return false; // Rejects a digit that already appears in the block.
                } // Ends the block-conflict condition.
            } // Ends the inner block scan.
        } // Ends the outer block scan.

        return true; // Confirms that the digit follows all Sudoku placement rules.
    } // Ends isSafe.

    void printBoard() const { // Prints the current Sudoku grid without modifying it.
        for (int i = 0; i < 9; i++) { // Visits each row of the board.
            if (i % 3 == 0) { // Checks whether a new group of three rows begins.
                cout << "+-------+-------+-------+" << endl; // Prints the horizontal grid border.
            } // Ends the row-group condition.

            for (int j = 0; j < 9; j++) { // Visits each cell in the current row.
                if (j % 3 == 0) { // Checks whether a new group of three columns begins.
                    cout << "| "; // Prints the left boundary of a 3-column group.
                } // Ends the column-group condition.

                if (board[i][j] == 0) { // Checks whether the current cell is empty.
                    cout << "_ "; // Displays an underscore for an empty cell.
                } else { // Handles a cell containing a digit.
                    cout << board[i][j] << " "; // Displays the digit stored in the cell.
                } // Ends the empty/non-empty condition.
            } // Ends the scan of the current row.

            cout << "|" << endl; // Prints the right boundary and starts a new output line.
        } // Ends the scan of all rows.

        cout << "+-------+-------+-------+" << endl; // Prints the bottom border of the grid.
    } // Ends printBoard.

    void writeBoardToFile() { // Writes the current board to the output file in the input-file format.
        for (int i = 0; i < 9; i++) { // Visits each row of the board.
            for (int j = 0; j < 9; j++) { // Visits each cell in the current row.
                if (j > 0) { // Checks whether a separator is needed before this cell.
                    outputFile << " "; // Separates cells with a single space, as in the input files.
                } // Ends the separator condition.

                if (board[i][j] == 0) { // Checks whether the current cell is empty.
                    outputFile << "_"; // Writes an underscore for an empty cell, matching the input format.
                } else { // Handles a cell containing a digit.
                    outputFile << board[i][j]; // Writes the digit stored in the cell.
                } // Ends the empty/non-empty condition.
            } // Ends the scan of the current row.

            outputFile << "\n"; // Ends the current row of the output file.
        } // Ends the scan of all rows.
    } // Ends writeBoardToFile.

    // Revised with ChatGPT: keep the legal digits found during the MRV scan.
    // This avoids checking those digits a second time in solveRecursive.
    bool findBestEmptyCell(int& bestRow, int& bestCol, unsigned int& bestMask) {
        int fewestCandidates = 10; // No cell can have more than nine options.
        bestRow = -1; // -1 means we have not found an empty cell yet.
        bestCol = -1;
        bestMask = 0;

        for (int row = 0; row < 9; row++) { // Search all rows.
            for (int col = 0; col < 9; col++) { // Search each cell in the row.
                if (board[row][col] != 0) { // Ignore digits that are already fixed.
                    continue;
                }

                int candidateCount = 0;
                unsigned int candidateMask = 0; // Each bit represents a legal digit.
                for (int digit = 1; digit <= 9; digit++) {
                    if (isSafe(row, col, digit)) { // Use the original rule checker.
                        candidateCount++;
                        candidateMask |= (1U << digit); // Remember this legal option.
                    }
                }

                if (candidateCount == 0) { // This branch cannot lead to a solution.
                    bestRow = row;
                    bestCol = col;
                    bestMask = 0;
                    return true;
                }

                if (candidateCount < fewestCandidates) { // Use the most restricted cell.
                    fewestCandidates = candidateCount;
                    bestRow = row;
                    bestCol = col;
                    bestMask = candidateMask; // Save its legal digits for recursion.
                    if (fewestCandidates == 1) {
                        return true; // We cannot improve on one legal choice.
                    }
                }
            }
        }
        return bestRow != -1; // False only when all cells have been filled.
    } // Ends findBestEmptyCell.

    void solveRecursive() { // Original backtracking search with saved legal digits.
        int emptyRow = -1;
        int emptyCol = -1;
        unsigned int candidates = 0;

        if (!findBestEmptyCell(emptyRow, emptyCol, candidates)) { // Puzzle is complete.
            solutionCount++;
            cout << "\nSolution " << solutionCount << ":" << endl;
            printBoard(); // Preserve Assignment 4's console display.

            if (solutionCount > 1) {
                outputFile << "\n"; // Separate solutions in the text file.
            }
            writeBoardToFile(); // Save this solution without keeping all boards.
            return; // Keep backtracking to find any other solutions.
        }

        if (candidates == 0) { // No digit fits the chosen cell.
            return; // Stop this unsuccessful branch immediately.
        }

        for (int digit = 1; digit <= 9; digit++) {
            if ((candidates & (1U << digit)) == 0) { // Skip forbidden digits.
                continue;
            }
            board[emptyRow][emptyCol] = digit; // Try a saved legal digit.
            solveRecursive(); // Search from the updated board.
            board[emptyRow][emptyCol] = 0; // Undo the move before trying another.
        }
    } // Ends solveRecursive.

public: // Begins the public section of the class.
    SudokuSolver() : solutionCount(0) { // Initializes the solution counter when an object is created.
        for (int row = 0; row < 9; row++) { // Visits each row during initialization.
            for (int col = 0; col < 9; col++) { // Visits each column during initialization.
                board[row][col] = 0; // Marks every cell as empty.
            } // Ends the initialization of one row.
        } // Ends board initialization.
    } // Ends the constructor.

    bool loadPuzzle(const string& filename) { // Loads and validates the basic format of a puzzle file.
        ifstream inputFile(filename); // Opens the specified file for reading.

        if (!inputFile.is_open()) { // Checks whether the file opened successfully.
            cerr << "Error: Could not open " << filename << endl; // Reports a missing or inaccessible file.
            return false; // Stops loading when the file cannot be opened.
        } // Ends the file-open check.

        vector<char> cells; // Temporarily stores all 81 cell symbols before changing the board.
        char ch; // Stores each character read from the file.

        while (inputFile.get(ch)) { // Reads the file one character at a time.
            if ((ch >= '1' && ch <= '9') || ch == '_') { // Accepts digits one through nine and underscores.
                cells.push_back(ch); // Saves a valid Sudoku cell symbol.
            } else if (ch == '0') { // Allows zero as another way to mark an empty cell.
                cells.push_back('_'); // Normalizes zero to the underscore blank marker.
            } else if (!isspace(static_cast<unsigned char>(ch))) { // Rejects unexpected non-whitespace characters.
                cerr << "Error: Invalid character in " << filename << endl; // Reports an unexpected character.
                return false; // Stops loading a malformed puzzle file.
            } // Ends the character-validation branches.
        } // Ends the file-reading loop.

        if (cells.size() != 81) { // Requires exactly 81 cell symbols for a 9-by-9 puzzle.
            cerr << "Error: " << filename << " must contain exactly 81 Sudoku cells." << endl; // Reports the incorrect cell count.
            return false; // Rejects a puzzle with too few or too many cells.
        } // Ends the cell-count check.

        for (int row = 0; row < 9; row++) { // Copies the validated symbols into each board row.
            for (int col = 0; col < 9; col++) { // Copies the symbols into each board column.
                char cell = cells[row * 9 + col]; // Retrieves the symbol at the current linear position.

                if (cell == '_') { // Checks whether the symbol represents a blank cell.
                    board[row][col] = 0; // Stores blank cells internally as zero.
                } else { // Handles a symbol containing a digit.
                    board[row][col] = cell - '0'; // Converts the digit character into its integer value.
                } // Ends the cell-conversion condition.
            } // Ends copying one row.
        } // Ends copying all 81 cells.

        return true; // Confirms that the file was read and formatted correctly.
    } // Ends loadPuzzle.

    bool isInitialBoardValid() { // Checks that the puzzle's original digits do not conflict.
        for (int row = 0; row < 9; row++) { // Examines each row of the starting puzzle.
            for (int col = 0; col < 9; col++) { // Examines each cell in the current row.
                if (board[row][col] == 0) { // Checks whether the current cell is blank.
                    continue; // Skips blank cells because they cannot conflict with a given digit.
                } // Ends the blank-cell condition.

                int digit = board[row][col]; // Saves the digit currently being checked.
                board[row][col] = 0; // Temporarily clears the cell so it does not conflict with itself.
                bool valid = isSafe(row, col, digit); // Checks the saved digit against its row, column, and block.
                board[row][col] = digit; // Restores the original digit after checking it.

                if (!valid) { // Checks whether another given digit conflicts with this one.
                    return false; // Rejects the starting puzzle if a conflict is found.
                } // Ends the conflict condition.
            } // Ends the scan of the current row.
        } // Ends the scan of the starting board.

        return true; // Confirms that all given digits are mutually consistent.
    } // Ends isInitialBoardValid.

    // Revised with ChatGPT: report whether the failure message was saved.
    bool writeNoSolution(const string& outputFilename) {
        ofstream failFile(outputFilename.c_str()); // Replace any stale output.
        if (!failFile.is_open()) {
            cerr << "Error: Could not write " << outputFilename << endl;
            return false;
        }
        failFile << "No solution found.\n"; // Exact required message.
        failFile.close(); // Flush the message and check for a disk error.
        if (!failFile) {
            cerr << "Error: Failed to finish writing " << outputFilename << endl;
            return false;
        }
        return true;
    } // Ends writeNoSolution.

    // Revised with ChatGPT: stop early if the solution file cannot be written.
    bool solve(const string& filename, const string& outputFilename) {
        cout << "\n========================================" << endl;
        cout << "Puzzle file: " << filename << endl;
        cout << "========================================" << endl;
        cout << "\nOriginal puzzle:" << endl;
        printBoard(); // Keep the original console presentation.

        if (!isInitialBoardValid()) { // Don't search puzzles with conflicting clues.
            cout << "\nNo solution found: the original puzzle has conflicting digits." << endl;
            return writeNoSolution(outputFilename);
        }

        solutionCount = 0; // Start each puzzle with a new solution count.
        outputFile.open(outputFilename.c_str());
        if (!outputFile.is_open()) { // Don't continue if required output is unavailable.
            cerr << "Error: Could not write " << outputFilename << endl;
            return false;
        }

        solveRecursive(); // Find and write every solution using backtracking.
        if (solutionCount == 0) {
            cout << "\nNo solution found." << endl;
            outputFile << "No solution found.\n";
        } else {
            cout << "\nTotal solutions found: " << solutionCount << endl;
            outputFile << "\nSolutions found: " << solutionCount << "\n";
        }

        outputFile.close(); // Flush output before reporting success.
        if (!outputFile) { // Catch write or close failures.
            cerr << "Error: Failed to finish writing " << outputFilename << endl;
            return false;
        }
        return true;
    } // Ends solve.
}; // Ends the SudokuSolver class definition.

int main() { // Processes all five required puzzle files.
    const string filenames[] = {"puzzle1.txt", "puzzle2.txt", "puzzle3.txt", "puzzle4.txt", "puzzle5.txt"};
    bool allFilesProcessed = true; // Track errors while still checking all puzzles.

    for (int i = 0; i < 5; i++) {
        const string& filename = filenames[i];
        string outputFilename = "solution" + to_string(i + 1) + ".txt";
        SudokuSolver solver; // Keep each puzzle's board and count separate.

        if (solver.loadPuzzle(filename)) {
            if (!solver.solve(filename, outputFilename)) {
                allFilesProcessed = false; // The output file could not be saved.
            }
        } else {
            // Preserve Code 2's output-file behavior for missing or bad inputs.
            // Also report a process error because the input itself was invalid.
            if (!solver.writeNoSolution(outputFilename)) {
                cerr << "Error: Unable to create " << outputFilename << endl;
            }
            allFilesProcessed = false;
        }
    }

    return allFilesProcessed ? 0 : 1; // Nonzero means at least one file failed.
} // Ends main.
