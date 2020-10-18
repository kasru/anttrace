/*
На бесконечной координатной сетке находится муравей.
Муравей может перемещаться на 1 клетку вверх (x,y+1), вниз (x,y-1), влево (x-1,y), вправо (x+1,y), по одной клетке за шаг.
Клетки, в которых сумма цифр в координате X плюс сумма цифр в координате Y больше чем 25 недоступны муравью.
Например, клетка с координатами (59, 79) недоступна, т.к. 5+9+7+9=30, что больше 25.
Сколько клеток может посетить муравей, если его начальная позиция (1000,1000), (включая начальную клетку).
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <utility>
#include <functional>
#include <cstdint>

/// @brief Сумма цифр числа
int sumdigit( int n )
{
    int sum = 0;
    while( n > 0 )
    {
        sum = sum + n % 10;
        n = n / 10;
    }
    return sum;
}

class Pole
{
public:

    Pole( std::size_t w, std::size_t h )
    {
        init( w, h );
    }

    void init( std::size_t w, std::size_t h )
    {
        pole_.resize( w );
        for( int x = 0; x != w; ++x )
        {
            pole_[ x ].resize( h, '0' );
        }
    }

    void setp( std::size_t x, std::size_t y )
    {
        pole_[ x ][ y ] = '1';
    }

    void save_to_pbm_file( const char* filename ) const
    {
        std::ofstream f( filename );
        if( f.is_open() )
        {
            f << "P1" << std::endl;
            f << pole_.size() << " " << pole_.at( 0 ).size() << std::endl;
            for( auto const& xline : pole_ )
            {
                for( auto const& val : xline )
                {
                    f << val << " ";
                }
                f << std::endl;
            }
        }
    }

private:
    using pole_t = std::vector< std::vector< unsigned char > >;
    pole_t pole_;
};

/// @brief Не рекурсивный алгоритм заливки области
std::size_t  fill_area( int x, int y, std::function<bool( int, int )> f_bound, Pole& pole )
{
    struct Point
    {
        int x, y;
    };
    Point tmp;
    std::stack<Point> spt;

    auto hash = []( const Point& p ) { return std::hash<int>()( p.x ) + 31 * std::hash<int>()( p.y ); };
    auto equal = []( const Point& p1, const Point& p2 ) { return p1.x == p2.x && p1.y == p2.y; };
    std::unordered_set<Point, decltype( hash ), decltype( equal )> area( 10, hash, equal );

    spt.emplace( Point{ x, y } );

    while( !spt.empty() )
    {
        tmp = spt.top();
        spt.pop();

        area.insert( tmp );
        pole.setp( tmp.x, tmp.y );

        // движемся вправо
        tmp.x += 1;
        if( !f_bound( tmp.x, tmp.y ) )
        {
            if( area.find( tmp ) == area.end() )
            {
                spt.push( tmp );
            }
        }

        // движемся влево
        tmp.x -= 2;
        if( !f_bound( tmp.x, tmp.y ) )
        {
            if( area.find( tmp ) == area.end() )
            {
                spt.push( tmp );
            }
        }

        // движемся вверх
        tmp.x += 1;
        tmp.y += 1;
        if( !f_bound( tmp.x, tmp.y ) )
        {
            if( area.find( tmp ) == area.end() )
            {
                spt.push( tmp );
            }
        }

        // движемся вниз
        tmp.y -= 2;
        if( !f_bound( tmp.x, tmp.y ) )
        {
            if( area.find( tmp ) == area.end() )
            {
                spt.push( tmp );
            }
        }
    }
    return area.size();
}

int main()
{
    int x = 1000;
    int y = 1000;

    Pole pole( 2*x, 2*y );

    auto count = fill_area(x, y, [](int xx, int yy)->bool
    {
        return sumdigit(xx) + sumdigit( yy ) > 25;
    }, pole );

    std::cout << count << std::endl;

    pole.save_to_pbm_file( "anttrace.pbm" );

    return 0;
}
