# include <Siv3D.hpp>

struct Bullet
{
    Bullet(Vec2 _pos, float _rad, float _size, ColorF _color, int32 _playerCount)
    :pos(_pos)
    ,rad(_rad)
    ,size(_size)
    ,color(_color)
    ,playerCount(_playerCount)
    {
        moveSpeed = Vec2(speed*Math::Sin(rad),-speed*Math::Cos(rad));
    }
    
    void dead()
    {
        alive = false;
    }
    
    void update()
    {
        if(alive)
        {
            pos.moveBy(moveSpeed);
        }
    }
    
    void draw() const
    {
        if(alive)
        {
            Shape2D::NStar(8, size, size*0.5, pos).draw(color);
        }
    }
    
    Vec2 pos;
    Vec2 moveSpeed;
    const ColorF color;
    const int32 playerCount;
    const float rad;
    const float size;
    const float speed=10.0;
    bool alive = true;
};

struct SmallVirus
{
    SmallVirus(Vec2 _pos, float _rad, float _size, ColorF _color, int32 _playerCount)
    :pos(_pos)
    ,rad(_rad)
    ,size(_size)
    ,color(_color)
    ,playerCount(_playerCount)
    {
        stopWatch.start();
        moveSpeed = Vec2(speed*Math::Sin(rad),-speed*Math::Cos(rad));
    }
    
    void dead()
    {
        alive = false;
    }
    
    void update()
    {
        if(alive)
        {
            if(pos.x < 0 || Window::Width() < pos.x)
            {
                moveSpeed.x *= -1;
            }
            
            if(pos.y < 180 || Window::Height() < pos.y)
            {
                moveSpeed.y *= -1;
            }
            
            pos.moveBy(moveSpeed);
        }
    }
    
    void draw() const
    {
        if(alive)
        {
            Shape2D::NStar(8, size, size*0.5, pos).drawFrame(10, color);
        }
    }
    
    Vec2 pos;
    Vec2 moveSpeed;
    Stopwatch stopWatch;
    const ColorF color;
    const int32 playerCount;
    const float rad;
    const float size;
    const float speed=5.0;
    bool alive = true;
};

struct Virus
{
    Virus(Vec2 _pos, float _maxSize, ColorF _color, int32 _playerCount)
    :pos(_pos)
    ,maxSize(_maxSize)
    ,color(_color)
    ,playerCount(_playerCount)
    ,growSpeed(Random(0.02,0.1))
    {}
    
    int32 dead()
    {
        alive = false;
        
        if(playerCounts.size() < 8)
        {
            return 2;
        }
        else
        {
            int32 pCount=0;
            int32 anotherPCount=0;
            for(int i=playerCounts.size()-8; i<playerCounts.size(); ++i)
            {
                if(playerCount == playerCounts[i])
                {
                    ++pCount;
                }
                else
                {
                    ++anotherPCount;
                }
                
                if(pCount <= anotherPCount)
                {
                    return 1;
                }
                else
                {
                    return 5;
                }
            }
        }
        
        return 0;
    }
    
    void setBullet(Bullet bullet)
    {
        bulletColors.push_back(bullet.color);
        playerCounts.push_back(bullet.playerCount);
    }
    
    void update()
    {
        if(alive)
        {
            if(size < maxSize)
            {
                size += growSpeed;
            }
        }
    }
    
    void draw() const
    {
        if(alive)
        {
            Shape2D::NStar(8, size, size*0.5, pos).drawFrame(size*0.2, color);
            
            for (int i=0; i<bulletColors.size(); ++i )
            {
                const Vec2 _pos = OffsetCircular(pos, size*1.2, i * 45_deg);

                Shape2D::NStar(8, size*0.4, size*0.2, _pos).draw(bulletColors[i]);
            }
        }
    }
    
    const Vec2 pos;
    const ColorF color;
    const int32 playerCount;
    float maxSize;
    float size = 10;
    float growSpeed;
    bool alive = true;
    Array<int32> playerCounts;
    Array<Color> bulletColors;
};

void Main()
{
    Window::Resize(1280, 720);
    Graphics::SetBackground(Palette::Whitesmoke);
    
    const float size = 30;
    const float virusSize = 40;
    const float speed = 5;
    const float bulletSize = 20;
    const Array<ColorF> colors = {ColorF(1.0,0.2,0.2,0.8), ColorF(0.2,0.2,1.0,0.8)};
    
    Array<Vec2> poses = {Window::Center()-Vec2(100,0), Window::Center()+Vec2(100,0)};
    Array<Stopwatch> coolTimes = {Stopwatch(), Stopwatch()};
    Array<Array<Bullet>> bullets = {Array<Bullet>(), Array<Bullet>()};
    Array<Array<Virus>> viruses = {Array<Virus>(), Array<Virus>()};
    Array<Array<SmallVirus>> smallViruses = {Array<SmallVirus>(), Array<SmallVirus>()};
    
    for(auto i : step(2))
    {
        coolTimes[i].start();
    }
    
    for(auto i : step(2))
    {
        viruses[0].push_back(Virus(Vec2(Window::Width()-100,300+i*250), virusSize, colors[0],0));
        viruses[1].push_back(Virus(Vec2(100,300+i*250), virusSize, colors[1],1));
    }
    
    while (System::Update())
    {
        for(auto& bullet : bullets[0])
        {
            for(auto& anotherBullet : bullets[1])
            {
                if(Circle(bullet.pos,bullet.size).intersects(Circle(anotherBullet.pos,anotherBullet.size)))
                {
                    if(bullet.alive && anotherBullet.alive)
                    {
                        bullet.dead();
                        anotherBullet.dead();
                    }
                }
            }
        }

        for(auto i : step(2))
        {
            for(auto& virus :viruses[i])
            {
                if(virus.alive && Circle(poses[i],size).intersects(Circle(virus.pos,virus.size)))
                {
                    poses[i].moveBy((poses[i]-virus.pos)*0.1);
                }
                
                for(auto j : step(2))
                {
                    for(auto& smallVirus : smallViruses[j])
                    {
                        if(virus.alive && Circle(smallVirus.pos, smallVirus.size).intersects(Circle(virus.pos,virus.size)))
                        {
                            smallVirus.pos.moveBy((smallVirus.pos-virus.pos)*0.1);
                        }
                    }
                }
            }
            
            double rad = 0;
            double deg = 0;
            
            if (const auto gamepad = Gamepad(i))
            {
                Vec2 moveRange(gamepad.axes[2],gamepad.axes[1]);
                rad = Math::Atan2(moveRange.x,-moveRange.y);
                poses[i].moveBy(moveRange*speed);
                deg = rad* 180.0/Math::Pi;
                
                if(gamepad.buttons[5].pressed() && 200 < coolTimes[i].ms())
                {
                    coolTimes[i].restart();
                    bullets[i].push_back(Bullet(poses[i] + Circular(size*0.7, deg*1.0_deg),rad,bulletSize,colors[i],i));
                }
            }
            
            for(auto& bullet : bullets[i])
            {
                for(auto j : step(2))
                {
                    for(auto& virus : viruses[j])
                    {
                        if(Circle(bullet.pos,bullet.size).intersects(Circle(virus.pos,virus.size)))
                        {
                            if(bullet.alive && virus.alive)
                            {
                                bullet.dead();
                                virus.setBullet(bullet);
                            }
                        }
                    }
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
            
            for(auto& smallVirus :smallViruses[i])
            {
                if(smallVirus.alive && 5000 < smallVirus.stopWatch.ms())
                {
                    smallVirus.alive = false;
                    viruses[i].push_back(Virus(smallVirus.pos, virusSize, smallVirus.color, smallVirus.playerCount));
                }
                
                smallVirus.update();
            }
            
            for(const auto& smallVirus : smallViruses[i])
            {
                smallVirus.draw();
            }
            
            for(auto& virus :viruses[i])
            {
                if(virus.alive && virus.maxSize <= virus.size)
                {
                    int32 virusCount = virus.dead();
                    for(auto k : step(virusCount))
                    {
                        smallViruses[i].push_back(SmallVirus(virus.pos, Random(0,360)*1_deg+k*120_deg, bulletSize, virus.color, virus.playerCount));
                    }
                }
                
                virus.update();
            }
            
            for(const auto& virus : viruses[i])
            {
                virus.draw();
            }

            Circle(poses[i],size*0.4).drawFrame(5,0,colors[i]);
            Circle(poses[i],size).drawFrame(0,5,colors[i]);
            Shape2D::NStar(10, bulletSize, bulletSize*0.5, poses[i] + Circular(size*0.7, deg*1.0_deg)).draw(colors[i]);
            
            Rect(20,50,Window::Width()*((double)viruses[0].size()/(viruses[0].size()+viruses[1].size()))-20,120).draw(colors[0]);
            Rect(Window::Width()*((double)viruses[0].size()/(viruses[0].size()+viruses[1].size())),50,Window::Width()*((double)viruses[1].size()/(viruses[0].size()+viruses[1].size()))-20,120).draw(colors[1]);
            RoundRect(20, 50, Window::Width()-40, 120, 10).drawFrame(20,Palette::Gray);
            
            Circle(Vec2(Window::Width()/2,60),60).draw();
            Circle(Vec2(Window::Width()/2,60),60).drawFrame(10,0,Palette::Gray);
        }
    }
}
