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
    
    void dead()
    {
        
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
    const float speed=10.0;
    bool isAlive = true;
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
    
    const float size = 30;
    const float virusSize = 50;
    const float speed = 5;
    const float bulletSize = 20;
    const Array<ColorF> colors = {ColorF(1.0,0.2,0.2), ColorF(0.2,0.2,1.0)};
    
    Array<Vec2> poses = {Window::Center()-Vec2(100,0), Window::Center()+Vec2(100,0)};
    Array<Stopwatch> coolTimes = {Stopwatch(), Stopwatch()};
    Array<Array<Bullet>> bullets = {Array<Bullet>(), Array<Bullet>()};
    Array<Array<Virus>> viruses = {Array<Virus>(), Array<Virus>()};
    
    for(auto i : step(2))
    {
        coolTimes[i].start();
    }
    
    for(auto i : step(3))
    {
        viruses[0].push_back(Virus(Vec2(100,100+i*200), virusSize, colors[0]));
        viruses[1].push_back(Virus(Vec2(Window::Width()-100,100+i*200), virusSize, colors[1]));
    }
    
    while (System::Update())
    {
        double rad = 0;
        double deg = 0;
        
        for(auto i : step(2))
        {
            for(auto& virus :viruses[0])
            {
                if(Circle(poses[i],size).intersects(Circle(virus.pos,virus.size)))
                {
                    poses[i].moveBy((poses[i]-virus.pos)*0.1);
                }
            }
            
            for(auto& virus :viruses[1])
            {
                if(Circle(poses[i],size).intersects(Circle(virus.pos,virus.size)))
                {
                    poses[i].moveBy((poses[i]-virus.pos)*0.1);
                }
            }
            
            if (const auto gamepad = Gamepad(i))
            {
                Vec2 moveRange(gamepad.axes[2],gamepad.axes[1]);
                rad = Math::Atan2(moveRange.x,-moveRange.y);
                poses[i].moveBy(moveRange*speed);
                deg = rad* 180.0/Math::Pi;
                
                if(gamepad.buttons[5].pressed() && 200 < coolTimes[i].ms())
                {
                    coolTimes[i].restart();
                    bullets[i].push_back(Bullet(poses[i] + Circular(size*0.7, deg*1.0_deg),rad,bulletSize,colors[i]));
                }
            }
        
            for(auto& bullet : bullets[i])
            {
                bullet.update();
            }

            for(const auto& bullet : bullets[i])
            {
                bullet.draw();
            }

            for(auto& virus :viruses[i])
            {
                virus.update();
            }
            
            for(const auto& virus : viruses[i])
            {
                virus.draw();
            }

            Circle(poses[i],size*0.4).drawFrame(5,0,colors[i]);
            Circle(poses[i],size).drawFrame(0,5,colors[i]);
            Shape2D::NStar(12, bulletSize, bulletSize*0.5, poses[i] + Circular(size*0.7, deg*1.0_deg)).draw(colors[i]);
        }
        
        
    }
}
