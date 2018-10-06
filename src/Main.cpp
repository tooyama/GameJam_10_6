# include <Siv3D.hpp>

struct Bullet
{
    Bullet(Vec2 _pos, float _rad, float _size, ColorF _color)
    :pos(_pos)
    ,rad(_rad)
    ,size(_size)
    ,color(_color)
    {
        moveSpeed = Vec2(speed*Math::Sin(rad),-speed*Math::Cos(rad));
    }
    
    void update()
    {
        if(pos.x < 0 || Window::Width() < pos.x)
        {
            moveSpeed.x *= -1;
        }
        
        if(pos.y < 0 || Window::Height() < pos.y)
        {
            moveSpeed.y *= -1;
        }
            
        pos.moveBy(moveSpeed);
    }
    
    void draw() const
    {
        Shape2D::NStar(12, size, size*0.5, pos).draw(color);
    }
    
    Vec2 pos;
    Vec2 moveSpeed;
    const ColorF color;
    const float rad;
    const float size;
    float speed=10.0;
};

struct Virus
{
    Virus(Vec2 _pos, float _maxSize, ColorF _color)
    :pos(_pos)
    ,maxSize(_maxSize)
    ,color(_color)
    {}
    
    void update()
    {
        if(size < maxSize)
        {
            size += 1;
        }
    }
    
    void draw() const
    {
        Shape2D::NStar(12, size, size*0.5, pos).drawFrame(size*0.2, color);
    }
    
    const Vec2 pos;
    ColorF color;
    float maxSize;
    float size = 0;
};

void Main()
{
    Window::Resize(1280, 720);
    Graphics::SetBackground(Palette::Whitesmoke);
    
    const float size = 50;
    const float speed = 5;
    const float bulletSize = 20;
    const ColorF p1Color(ColorF(1.0,0.2,0.2));
    
    Vec2 p1Pos(Window::Center());
    
    Array<Bullet> p1Bullets;
    Array<Virus> p1Viruses;
    Stopwatch p1CoolTime;
    
    p1CoolTime.start();
    
    while (System::Update())
    {
        double rad = 0;
        double deg = 0;
        
        if (const auto gamepad = Gamepad(0))
        {
            Vec2 moveRange(gamepad.axes[2],gamepad.axes[1]);
            rad = Math::Atan2(moveRange.x,-moveRange.y);
            p1Pos.moveBy(moveRange*speed);
            deg = rad* 180.0/Math::Pi;
            
            if(gamepad.buttons[5].pressed() && 100 < p1CoolTime.ms())
            {
                p1CoolTime.restart();
                p1Bullets.push_back(Bullet(p1Pos + Circular(size*0.7, deg*1.0_deg),rad,bulletSize,p1Color));
            }
            
            if(gamepad.buttons[4].down())
            {
                p1Viruses.push_back(Virus(p1Pos, size, p1Color));
            }
        }
        
        for(auto& p1Bullet : p1Bullets)
        {
            p1Bullet.update();
        }

        for(const auto& p1Bullet : p1Bullets)
        {
            p1Bullet.draw();
        }
        
        for(auto& p1Virus : p1Viruses)
        {
            p1Virus.update();
        }
        
        for(const auto& p1Virus : p1Viruses)
        {
            p1Virus.draw();
        }
        
        Circle(p1Pos,size*0.4).drawFrame(5,0,p1Color);
        Circle(p1Pos,size).drawFrame(0,5,p1Color);
        Shape2D::NStar(12, bulletSize, bulletSize*0.5, p1Pos + Circular(size*0.7, deg*1.0_deg)).draw(p1Color);
    }
}
