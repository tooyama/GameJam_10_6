# include <Siv3D.hpp>

void Main()
{
    Vec2 pos(Window::Center());
    
    while (System::Update())
    {
        if (const auto gamepad = Gamepad(0))
        {
            Vec2 moveRange(gamepad.axes[2],gamepad.axes[1]);
            
            pos.moveBy(moveRange*2.0);
        }
        
        Circle(pos,20).draw();
    }
}
