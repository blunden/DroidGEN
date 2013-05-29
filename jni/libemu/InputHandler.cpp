
#include <string.h>

#include "InputHandler.h"

#include "common.h"

InputHandler::InputHandler(int buttonCount, int analogCount)
{
     _buttons = NULL;
     _analogs = NULL;
     _fingers = NULL;

     resizeArrays(buttonCount, analogCount);
}


InputHandler::~InputHandler()
{
	clear();
}


void InputHandler::clear()
{
    if (_buttons != NULL)
    {
         delete(_buttons);
         _buttons = NULL;
    }

    if (_analogs != NULL)
    {
         delete(_analogs);
         _analogs = NULL;
    }

    if (_fingers != NULL)
    {
         for (int i = 0; i < _fingerCount; i++)
         {
              if (_fingers[i].buttons != NULL)
              {
                   delete(_fingers[i].buttons);
              }

              if (_fingers[i].analogs != NULL)
              {
                   delete(_fingers[i].analogs);
              }
         }

         delete(_fingers);
         _fingers = NULL;
    }
}


void InputHandler::resizeArrays(int buttonCount, int analogCount)
{
     LOGD("resizeArrays(%d, %d)", buttonCount, analogCount);
     clear();

     if (buttonCount > 0 && buttonCount <= MAX_BUTTONS)
     {
          _buttons = new Button[buttonCount];
          for (int i = 0; i < buttonCount; i++)
          {
               _buttons[i].state = false;
               _buttons[i].lastState = false;
               _buttons[i].keyCode = -1;
          }

          _buttonCount = buttonCount;
     }

     if (analogCount > 0 && analogCount <= MAX_BUTTONS)
     {
          _analogs = new Analog[analogCount];
          for (int i = 0; i < analogCount; i++)
          {
               _analogs[i].x = 0.0f;
               _analogs[i].y = 0.0f;
          }

          _analogCount = analogCount;
     }

     // TODO: bad code, will reset fingers...
     _fingerCount = MAX_FINGER_COUNT;
     _fingers = new Finger[_fingerCount];
     bool reset = false;
     if (_fingers == NULL)
     {
          reset = true;
     }

     for (int i = 0; i < _fingerCount; i++)
     {
          //if (reset)
          {
               _fingers[i].down = false;
               _fingers[i].buttons = NULL;
               _fingers[i].buttonCount = 0;
               _fingers[i].analogs = NULL;
               _fingers[i].analogCount = 0;
          }

          _fingers[i].buttons = new Button*[_buttonCount];
          _fingers[i].analogs = new Analog*[_analogCount];
     }
}


void InputHandler::onKeyUp(int keycode)
{
     //LOGD("onKeyUp(%d)", keycode);

     for (int i = 0; i < _buttonCount; i++)
     {
          if (_buttons[i].keyCode == keycode)
          {
               _buttons[i].state = false;
               return;
          }
     }
}


void InputHandler::onKeyDown(int keycode)
{
     //LOGD("onKeyDown(%d)", keycode);

     // check buttons
     for (int i = 0; i < _buttonCount; i++)
     {
          if (_buttons[i].keyCode == keycode)
          {
               _buttons[i].state = true;
               return;
          }
     }
}


void InputHandler::onTouchUp(const TouchEvent &event)
{
     //LOGD("onTouchUp(%d, %f, %f, %f", event.finger, event.x, event.y, event.radius);

     // see which finger this is from
     int fing = event.finger;
     if (fing >= 0 && fing < MAX_FINGER_COUNT && _fingers[fing].down)
     {
          for (int j = 0; j < _fingers[fing].buttonCount; j++)
          {
               _fingers[fing].buttons[j]->state = false;
          }

          for (int j = 0; j < _fingers[fing].analogCount; j++)
          {
               _fingers[fing].analogs[j]->x = 0;
               _fingers[fing].analogs[j]->y = 0;
          }

          _fingers[fing].down = false;
          _fingers[fing].buttonCount = 0;
          _fingers[fing].analogCount = 0;
     }
}


void InputHandler::onTouchDown(const TouchEvent &event)
{
     //LOGD("onTouchDown(%d, %f, %f, %f", event.finger, event.x, event.y, event.radius);
     processDownInput(event);
}


void InputHandler::onTouchMove(const TouchEvent &event)
{
     //LOGD("onTouchMove(%d, %f, %f, %f", event.finger, event.x, event.y, event.radius);
     processDownInput(event);
}


void InputHandler::processDownInput(const TouchEvent &event)
{
	//LOGD("processDownInput(Finger: %d)", event.finger);
	int fing = event.finger;
     if (fing < 0 || fing >= MAX_FINGER_COUNT)
     {
    	 return;
     }

     if (!_fingers[fing].down)
     {
          // min radius
          float radius = event.radius;
          if (radius <= 0)
          {
               radius = 0.05;
          }

          _fingers[fing].sphere.setX(event.x);
          _fingers[fing].sphere.setY(event.y);
          _fingers[fing].sphere.setRadius(radius * 100);

          _fingers[fing].down = true;
     }

     // search for the finger index to store if none already
     if (_fingers[fing].down)
     {
          // min radius
          float radius = event.radius;
          if (radius <= 0)
          {
               radius = 0.05;
          }

          _fingers[fing].sphere.setX(event.x);
          _fingers[fing].sphere.setY(event.y);
          _fingers[fing].sphere.setRadius(radius * 100);

          // clear old buttons it was touching
          for (int i = 0; i < _fingers[fing].buttonCount; i++)
          {

               _fingers[fing].buttons[i]->state = false;
          }
          _fingers[fing].buttonCount = 0;

          // find and set new buttons it is touching
          for (int i = 0; i < _buttonCount; i++)
          {
               if (_buttons[i].sphere.intersects(&_fingers[fing].sphere))
               {
                    int index = _fingers[fing].buttonCount++;
                    _fingers[fing].buttons[index] = _buttons+i;

                    //LOGD("Collision(%p, button: %d, finger button: %d, total: %d)", _buttons+i, i, index, _fingers[fing].buttonCount);

                    _buttons[i].state = true;
               }
          }

          // clear old analogs
          for (int i = 0; i < _fingers[fing].analogCount; i++)
          {
               _fingers[fing].analogs[i]->x = 0;
               _fingers[fing].analogs[i]->y = 0;
          }
          _fingers[fing].analogCount = 0;

          // search new analogs and set
          for (int i = 0; i < _analogCount; i++)
          {
               if (_analogs[i].sphere.intersects(&_fingers[fing].sphere))
               {
                    int index = _fingers[fing].analogCount++;
                    _fingers[fing].analogs[index] = _analogs+i;

                    float xDist = (_fingers[fing].sphere.getX() - _analogs[i].sphere.getX()) / _analogs[i].sphere.getRadius();
                    float yDist = (_fingers[fing].sphere.getY() - _analogs[i].sphere.getY()) / _analogs[i].sphere.getRadius();

                    _analogs[i].x = xDist;
                    _analogs[i].y = yDist;
               }
          }
     }
}


void InputHandler::setAnalog(int analogIndex, int x, int y, float width, float height,
                              int leftcode, int upcode, int rightcode, int downcode, bool visible)
{
     LOGD("setAnalog(%p)", _analogs);
     _analogs[analogIndex].quad.SetDimensions(x, y, width, height);
     _analogs[analogIndex].quad.Visible = visible;

     float max = width > height ? width : height;

     float radius = max / 2.0f;

     // create new ones (up, right, down, left)
     _analogs[analogIndex].sphere.setX(x + radius);
     _analogs[analogIndex].sphere.setY(y + radius);
     _analogs[analogIndex].sphere.setRadius(radius);
}


void InputHandler::setAnalogTexture(int index, GLint texture)
{
     _analogs[index].quad.TextureId = texture;
}


void InputHandler::setAnalogVisibility(int analogIndex, bool visible)
{
     _analogs[analogIndex].quad.Visible = visible;
}


void InputHandler::setButton(int buttonIndex, int x, int y, float width, float height, int keycode, bool visible)
{
     float max = width > height ? width : height;

     _buttons[buttonIndex].quad.SetDimensions(x, y, width, height);
     _buttons[buttonIndex].quad.Visible = visible;

     _buttons[buttonIndex].sphere.setX(x + (width/2.0f));
     _buttons[buttonIndex].sphere.setY(y + (height/2.0f));
     _buttons[buttonIndex].sphere.setZ(0);
     _buttons[buttonIndex].sphere.setRadius(max / 2.0f);

     _buttons[buttonIndex].keyCode = keycode;
}


void InputHandler::setButtonVisibility(int buttonIndex, bool visible)
{
     _buttons[buttonIndex].quad.Visible = visible;
}


void InputHandler::setButtonTexture(int buttonIndex, GLint texture)
{
     _buttons[buttonIndex].quad.TextureId = texture;
}


void InputHandler::update()
{
     //memcpy(_lastButtonFlags, _buttonFlags, sizeof(bool) * _buttonCount);
     for (int i = 0; i < _buttonCount; i++)
     {
          _buttons[i].lastState = _buttons[i].state;
     }
}


void InputHandler::draw(const GraphicsDriver &gfx)
{
     for (int i = 0; i < _analogCount; i++)
     {
          if (_analogs[i].quad.Visible)
          {
               gfx.DrawQuad2D(&_analogs[i].quad);
          }
     }

     for (int i = 0; i < _buttonCount; i++)
     {
          if (_buttons[i].quad.Visible)
          {
               gfx.DrawQuad2D(&_buttons[i].quad);
          }
     }
}

