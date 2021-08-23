#define PIXELWIDTH 4
#define PIXELHEIGHT 8
#define DELAYINMICROSECONDS 33125

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>

Display* WindowDisplay;
Window WindowObj;
int WindowScreen;

char Frame[23][64];

void CleanUp(void) {
    XDestroyWindow(WindowDisplay, WindowObj);
    XCloseDisplay(WindowDisplay);
}

void Die(const char* Format, ...) {
    if (WindowObj) {
        CleanUp();
    }
	va_list VArgs;
	va_start(VArgs, Format);
	vfprintf(stderr, Format, VArgs);
	va_end(VArgs);
    exit(1);
}

int ReadFrame(unsigned int Iteration) {
    int Ret = 0;
    char FName[17];
    snprintf(FName, 17, "frames/%u", Iteration);
    FILE* File = fopen(FName, "r");
    if (!File) {
        Ret = 1;
    } else {
        int i = 0;
        char Buffer[66];
        while (fgets(Buffer, 66, File) != NULL) {
            strncpy(Frame[i], Buffer, 64);
            i++;
        }
        fclose(File);
    }
    return Ret;
}

void Init(void) {
    if ((WindowDisplay = XOpenDisplay(NULL)) == NULL)
        Die("Cannot open display\n");
    WindowScreen = DefaultScreen(WindowDisplay);
    WindowObj = XCreateSimpleWindow(WindowDisplay, RootWindow(WindowDisplay, WindowScreen), 10, 10, PIXELWIDTH * 63, PIXELHEIGHT * 23,  1, BlackPixel(WindowDisplay, WindowScreen), WhitePixel(WindowDisplay, WindowScreen));
    XStoreName(WindowDisplay, WindowObj, "Bad Apple!!");
    XMapWindow(WindowDisplay, WindowObj);
}

void MainLoop() {
    int Open = 1;
    XSelectInput(WindowDisplay, WindowObj, ExposureMask | KeyPressMask);
    while (Open) {
        XEvent Event;
        XNextEvent(WindowDisplay, &Event);
        if (Event.type == Expose) {
            for (unsigned i = 1; i < 6574; i++) {
                if (ReadFrame(i) != 0)
                    Die("Missing frame '%u'\n", i);
                int PosX, PosY = 0;
                for (int j = 0; j < 24; j++) {
                    PosX = 0;
                    for (int k = 0; k < 64; k++) {
                        if (Frame[j][k] == '1')
                            XFillRectangle(WindowDisplay, WindowObj, DefaultGC(WindowDisplay, WindowScreen), PosX, PosY, PIXELWIDTH, PIXELHEIGHT);
                        PosX += PIXELWIDTH;
                    }
                    PosY += PIXELHEIGHT;
                    XFlush(WindowDisplay);
                }
                usleep(DELAYINMICROSECONDS);
                XClearWindow(WindowDisplay, WindowObj);
            }
            Open = 0;
        }
    }
}

int main(void) {
    Init();
    MainLoop();
    CleanUp();
    return 0;
}
