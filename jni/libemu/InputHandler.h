
#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_

#include <sys/types.h>

#include "GraphicsDriver.h"
#include "Sphere.h"

typedef struct _TouchEvent
{
     int finger;
     float x;
     float y;
     float radius;
} TouchEvent;

#define MAX_BUTTONS 255
#define MAX_FINGER_COUNT 5


typedef struct _Button
{
     Sphere sphere;
     Quad2D quad;
     int keyCode;
     bool state;
     bool lastState;
} Button;

typedef struct _Analog
{
     Sphere sphere;
     Quad2D quad;
     float x;
     float y;
} Analog;


typedef struct Finger
{
     Sphere sphere;
     bool down;
     int id;
     Button** buttons;
     int buttonCount;
     Analog** analogs;
     int analogCount;
} Finger;


class InputHandler
{
public:
     InputHandler(int buttonCount, int analogCount);
     ~InputHandler();

     void clear();

     void resizeArrays(int buttonCount, int analogCount);

     void update();

     void onTouchDown(const TouchEvent &event);
     void onTouchUp(const TouchEvent &event);
     void onTouchMove(const TouchEvent &event);

     void onKeyDown(const int keycode);
     void onKeyUp(const int keycode);

     float getAnalogX(int index) { return _analogs[index].x; }
     float getAnalogY(int index) { return _analogs[index].y; }

     void setAnalog(int analogIndex, int x, int y, float width, float height, int leftcode, int upcode, int rightcode, int downcode, bool visible);
     void setAnalogVisibility(int analogIndex, bool visible);
     void setAnalogTexture(int index, GLint texture);

     void setButton(int buttonIndex, int x, int y, float width, float height, int keycode, bool visible);
     void setButtonVisibility(int buttonIndex, bool visible);
     void setButtonTexture(int buttonIndex, GLint texture);

     void setXSensitivity(float x) { _xSensitivity = x; }
     void setYSensitivity(float y) { _ySensitivity = y; }
     float getXSensitivity() { return _xSensitivity; }
     float getYSensitivity() { return _ySensitivity; }

     bool isButtonUp(int buttonIndex) { return !_buttons[buttonIndex].state; }
     bool isButtonDown(int buttonIndex) { return _buttons[buttonIndex].state; }
     bool wasButtonPressed(int buttonIndex) { return ( _buttons[buttonIndex].state &&
                                                       !_buttons[buttonIndex].lastState ); }
     bool wasButtonReleased(int buttonIndex) { return ( !_buttons[buttonIndex].state &&
                                                       _buttons[buttonIndex].lastState ); }

     void draw(const GraphicsDriver &gfx);
private:
     float _xSensitivity;
     float _ySensitivity;

     int _buttonCount;
     int _analogCount;
     int _fingerCount;

     Button* _buttons;
     Analog* _analogs;
     Finger* _fingers;

     void processDownInput(const TouchEvent &event);

};

#endif /* INPUTHANDLER_H_ */
