#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib/raylib.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI

#define CloseWindow WinCloseWindow
#define ShowCursor WinShowCursor
#define DrawText WinDrawText
#define DrawTextEx WinDrawTextEx

#include <windows.h>

#undef CloseWindow
#undef ShowCursor
#undef DrawText
#undef DrawTextEx

#define SEGOE_UI_TTF_PATH "C:/Windows/Fonts/segoeui.ttf"

#define FOO_NOWPLAYING2_PATH "foo_nowplaying2.txt"
#define FOO_NOWPLAYING2_BUFSIZE 512

#define TASKBAR_HEIGHT 48

// easier to just do it here instead of rewriting the metadata for the files in my library
void Ye(char* str) {
    const char* old_template = "Kanye West";
    const char* new_template = "Ye";

    char* pos;
    while ((pos = strstr(str, old_template)) != NULL)
    {
        size_t old_len = strlen(old_template);
        size_t new_len = strlen(new_template);

        memmove(pos + new_len, pos + old_len, strlen(pos + old_len) + 1);
        memcpy(pos, new_template, new_len);
    }
}

int main() {
    const int windowWidth = 600;
    const int windowHeight = TASKBAR_HEIGHT;
    
    FILE* np2_file = fopen(FOO_NOWPLAYING2_PATH, "r");
    if (!np2_file)
    {
        perror("fopen");
        exit(1);
    }
    size_t np2_bytes_read = 0;
    
    char np2_buffer[FOO_NOWPLAYING2_BUFSIZE];
   
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_MOUSE_PASSTHROUGH);
    InitWindow(windowWidth, windowHeight, "foobar-widget");
    
    HWND hwnd = GetWindowHandle();
    ShowWindow(hwnd, SW_HIDE);
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
    ShowWindow(hwnd, SW_SHOW);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetWindowPosition(0, GetMonitorHeight(GetCurrentMonitor()) - TASKBAR_HEIGHT);
    
    int* codepoints = (int*)malloc(sizeof(int) * 2000);
    int count = 0;
    for (int i = 32; i <= 255; i++) codepoints[count++] = i;        // ascii and latin-1 for english, french, spanish, german, ©, ®
    for (int i = 0x0100; i <= 0x017F; i++) codepoints[count++] = i; // latin extended-a for romanian, polish, turkish diacritics
    for (int i = 0x0400; i <= 0x04FF; i++) codepoints[count++] = i; // cyrillic alphabet for russian, ukrainian, bulgarian, serbian
    for (int i = 0x2000; i <= 0x20CF; i++) codepoints[count++] = i; // general punctuation and currency symbols like euro or pound
    codepoints[count++] = 0x2122;                                   // manually add trademark symbol
    for (int i = 0x2200; i <= 0x22FF; i++) codepoints[count++] = i; // mathematical operators like infinity, sum, plus-minus 
    for (int i = 0x2190; i <= 0x21FF; i++) codepoints[count++] = i; // standard and complex arrows
    Font font = LoadFontEx(SEGOE_UI_TTF_PATH, 32, codepoints, count);
    free(codepoints);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    const int fontSize = 20;
    const int fontSpacing = 0;
    
    Vector2 textMeasurement = MeasureTextEx(font, "Placeholder\nPlaceholder", fontSize, fontSpacing);

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        
        rewind(np2_file);
        np2_bytes_read = fread(np2_buffer, 1, sizeof(np2_buffer) - 1, np2_file);
        np2_buffer[np2_bytes_read] = '\0';
        for (size_t i = 0; np2_buffer[i] != '\0'; i++)
        {
            if (np2_buffer[i] == '\\' && np2_buffer[i + 1] == 'n')
            {
                np2_buffer[i] = '\n';
                memmove(&np2_buffer[i + 1], &np2_buffer[i + 2], strlen(&np2_buffer[i + 2]) + 1);
            }
        }
        Ye(np2_buffer);
        
        BeginDrawing();
            ClearBackground(BLANK);
            DrawTextEx(font, TextFormat("%s", np2_buffer), (Vector2){5, (TASKBAR_HEIGHT - textMeasurement.y) / 2}, fontSize, fontSpacing, WHITE);
        EndDrawing();
    }
    
    fclose(np2_file);
    CloseWindow();
    
    return 0;
}
