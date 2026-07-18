#pragma once
#ifndef CONTAINERBASE_H
#define CONTAINERBASE_H

#include "Gui.h"
#include <vector>

class ContainerBase : public Control
{
protected:
    std::vector<Control* > _children;

public:
    ContainerBase();
    ~ContainerBase();

    void VirtualUpdate() override
    {
        ContainerBaseUpdate();
    }


    void VirtualRender() override
    {
        ContainerBaseRender();
    }
    void VirtualResize() override
    {
        ContainerBaseResize();
    }
    bool VirtualMouseMove() override
    {
        return ContainerBaseMouseMove();
    }
    bool VirtualClick(int button, int action) override
    {
        return ContainerBaseClick(button,action);
    }
    void VirtualMouseWheel(float amount, int direction) override
    {
        ContainerBaseMouseWheel(amount,direction);
    }

    virtual void AddControl(Control* control)
    {   
        ContainerBaseAddControl(control);
        //std::cout<<"Adding control to a containerbase\n";
    }
    std::vector<Control*>& GetChildren() {return _children;}

protected:
    void ContainerBaseUpdate();
    void ContainerBaseRender();
    void ContainerBaseResize();
    bool ContainerBaseMouseMove();
    bool ContainerBaseClick(int button, int action);
    void ContainerBaseMouseWheel(float amount, int direction);
    
    void ContainerBaseAddControl(Control* control);

    void SubControlSetGuicontext(GuiContext* guicontext) override
    {
        //std::cout<<"Children count: "<<_children.size() << "\n";
        for(int i=0;i<_children.size();++i)
        {
            //std::cout<<"Setting Child's guicontext\n";
            _children[i]->SetGuiContext(guicontext);

            
        }
    }
};


#endif