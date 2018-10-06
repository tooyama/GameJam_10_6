# include <Siv3D.hpp>

struct Bullet
{
    
};

struct Virus
{
    
};

void Main()
{
    Window::Resize(1280, 720);
    Graphics::SetBackground(Palette::Whitesmoke);
    
    const float size = 50;
    const float speed = 5;
    const float bulletSize = 20;
    const ColorF p1Color(ColorF(1.0,0.2,0.2));
    
    Vec2 pos(Window::Center());
    
    while (System::Update())
    {
        double rad = 0;
        
        if (const auto gamepad = Gamepad(0))
        {
            Vec2 moveRange(gamepad.axes[2],gamepad.axes[1]);
            rad = Math::Atan2(moveRange.x,-moveRange.y);
            pos.moveBy(moveRange*speed);
        }
        
        double deg = rad* 180.0/Math::Pi;
        
        Circle(pos,size*0.4).drawFrame(5,0,p1Color);
        Circle(pos,size).drawFrame(0,5,p1Color);
        Shape2D::NStar(12, bulletSize, bulletSize*0.5, pos + Circular(size*0.7, deg*1.0_deg)).draw(p1Color);
    }
}
