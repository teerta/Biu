
#ifndef _BBFUNC_H_
#define _BBFUNC_H_


#include <algorithm>


using namespace std;


namespace Biu {


    template<class Container, class Func>
    inline void DestructContainer(Container& c, Func func)
    {
        for_each(c.begin(), c.end(), func);
        c.clear();
    }

    template<class Container>
    inline void DeleteContainer(Container& c)
    {
        DestructContainer(c, [](typename Container::value_type& val){ delete val; });
    }

    template<class Container>
    void DeleteContainer2nd(Container& c)
    {
        for_each(c.begin(), c.end(), 
            [](typename Container::value_type pa)
            {
                delete pa.second;
            }
            );
        c.clear();
    }

    template<class Container, class Func>
    void DestructContainerMovable2nd(Container& c, Func func)
    {
        for (auto iter = c.begin(); iter != c.end();)
        {
            auto val = iter->second;
            c.erase(iter++);
            func(val);
        }
    }


}


#endif // _BBFUNC_H_
