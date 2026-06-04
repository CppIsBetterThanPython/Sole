#include "Graphics.h"

void graphics::PrintToConsoleBlock(const Pixels& pixels) {
    ShowConsoleCursor(FALSE);

    static std::string buffer;
    static vector<vector<std::string>> previousScreen = vector<vector<std::string>>(
        pixels.size.x,
        vector<std::string>(std::ceil((double)pixels.size.y / 2), std::string(reinterpret_cast<const char*>(u8"▄")))
    );

    dimensions batchStart;
    std::string batch;

    for (size_t y = 0; y < std::ceil((double)pixels.size.y / 2); y++) {
        for (size_t x = 0; x < pixels.size.x; x++) {
            pixelToChar(pixels[x][y * 2], pixels[x][y * 2 + 1], buffer);
            if (buffer != previousScreen[x][y]) {
                batchStart = (batch.empty()) ? dimensions{ x, y } : batchStart;

                previousScreen[x][y] = buffer;
                batch += buffer;
            }
            else if (!batch.empty()) {
                setCursorPosition(batchStart.x, batchStart.y);
                std::cout << batch;
                batch.clear();
            }
        }
        if (!batch.empty()) {
            batch += "\n";
        }
    }

    if (!batch.empty()) {
        setCursorPosition(batchStart.x, batchStart.y);
        std::cout << batch;
        batch.clear();
    }
}

void graphics::PrintToConsoleAscii(const Pixels& zBuffer) {
    ShowConsoleCursor(FALSE);

    static vector<vector<char>> previousScreen = vector<vector<char>>(
        zBuffer.size.x,
        vector<char>(zBuffer.size.y, ' ')
    );

    dimensions batchStart;
    std::string batch;

    for (size_t y = 0; y < zBuffer.size.y; y++) {
        for (size_t x = 0; x < zBuffer.size.x; x++) {
            char currentChar = pixelToChar(zBuffer[x][y]);
            if (currentChar != previousScreen[x][y]) {
                batchStart = (batch.empty()) ? dimensions{ x, y } : batchStart;

                previousScreen[x][y] = currentChar;
                batch += currentChar;
            }
            else if (!batch.empty()) {
                setCursorPosition(batchStart.x, batchStart.y);
                std::cout << batch;
                batch.clear();
            }
        }
        if (!batch.empty()) {
            batch += "\n";
        }
    }

    if (!batch.empty()) {
        setCursorPosition(batchStart.x, batchStart.y);
        std::cout << batch;
        batch.clear();
    }
}