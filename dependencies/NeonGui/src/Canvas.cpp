#include "Canvas.h"

void Canvas::CanvasResize()
{
    ControlResize();

    if(_resizeCallback != nullptr && _callbackContext != nullptr)
    {
        int newWidth = (int)((_box.max.x - _box.min.x) * guiContext->windowWidth);
        int newHeight = (int)((_box.max.y - _box.min.y) * guiContext->windowHeight);

        _resizeCallback(_callbackContext,newWidth,newHeight);
    }
}

void Canvas::CanvasRender()
{
    if(_renderCallback != nullptr && _callbackContext != nullptr)
    {
        _renderCallback(_callbackContext);
    }

    ControlRender();
}

void Canvas::CanvasUpdate()
{
    ControlUpdate();

    if(_updateCallback != nullptr && _callbackContext != nullptr)
    {
        _updateCallback(_callbackContext,guiContext->deltaTime);
    }
}

void Canvas::CanvasMouseMove()
{
    ControlMouseMove();

    if(_mouseMoveCallback != nullptr && _callbackContext != nullptr)
    {
        //Relative to the fixedbox of canvas
        glm::vec2 windowRes = glm::vec2(guiContext->windowWidth,guiContext->windowHeight);
        glm::vec2 fixedMin = _box.min * windowRes;
        glm::vec2 newMouseRelativeToBox = guiContext->currentMousePos - fixedMin;
        float newMouseX = newMouseRelativeToBox.x;
        float newMouseY = newMouseRelativeToBox.y;

        _mouseMoveCallback(_callbackContext,newMouseX,newMouseY);
    }
}

void Canvas::CanvasMouseClick(int button, int action)
{
    //focus stuff
    bool isMouseOnControl = IsCursorOnControl(guiContext->currentMousePos);

    if(button == 0 && action == 0)
    {
        if(isMouseOnControl)
        {
            if(!isFocused)
            {
                SetFocused();
            }

            if(_mouseClickCallback != nullptr && _callbackContext  != nullptr)
            {
                _mouseClickCallback(_callbackContext,button,action);
            }
        }
        else 
        {
            if(isFocused)
            {
                SetUnfocused();
            }
        }
    }
    

}

void Canvas::CanvasMouseWheel(float amount, int direction)
{
    //TODODODODODODO
}

void Canvas::CanvasKeyInput(int key,int action, int mods)
{
    if(!isFocused) return;

    if(_keyCallback!=nullptr && _callbackContext != nullptr)
    {
        _keyCallback(_callbackContext,key,action,mods);
    }
}