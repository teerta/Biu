
#ifndef _BSFUNC_H_
#define _BSFUNC_H_


#include <algorithm>
#include <memory>
#include <iterator>
#include <utility>


using namespace std;


namespace Biu {

    
    // Functions
    
    
    // for list, dequeue, vector and etc.
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    void BMSer(Serializer& s, Collection<T, Allocator>& c)
    {
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](typename Collection<T, Allocator>::reference v)
            {
                s.Serialize(v);
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    void BMSer(Serializer& s, Collection<T*, Allocator>& c)
    {        
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](typename Collection<T*, Allocator>::reference v)
            {
                s.Serialize(*v);
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    void BMSer(Serializer& s, Collection<unique_ptr<T>, Allocator>& c)
    {        
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](typename Collection<unique_ptr<T>, Allocator>::reference v)
            {
                s.Serialize(*v.get());
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    void BMSer(Serializer& s, Collection<shared_ptr<T>, Allocator>& c)
    {        
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](typename Collection<shared_ptr<T>, Allocator>::reference v)
            {
                s.Serialize(*v.get());
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    bool BMDeser(Serializer& s, Collection<T, Allocator>& c)
    {
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            T t;
            if (!s.Deserialize(t))
            {                
                c.clear();
                return false;
            }
            c.push_back(move(t));
        }
        
        return true;
    }
        
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    bool BMDeser(Serializer& s, Collection<T*, Allocator>& c)
    {        
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            T* p = new T();
            if (!s.Deserialize(*p))
            {
                delete p;
                for_each(begin(c), end(c), [&](typename Collection<T*, Allocator>::reference v)
                    {
                        delete v;
                    }                
                );
                c.clear();
                return false;
            }
            c.push_back(p);
        }
        
        return true;
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    bool BMDeser(Serializer& s, Collection<unique_ptr<T>, Allocator>& c)
    {
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            unique_ptr<T> p(new T());
            if (!s.Deserialize(*p.get()))
            {
                c.clear();
                return false;
            }
            c.push_back(move(p));
        }
        
        return true;
    }
    
    template<
        class Serializer, 
        template<class, class> class Collection, 
        class T, 
        class Allocator = std::allocator<T>
    >
    bool BMDeser(Serializer& s, Collection<shared_ptr<T>, Allocator>& c)
    {        
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            auto p = make_shared<T>();
            if (!s.Deserialize(*p.get()))
            {
                c.clear();
                return false;
            }
            c.push_back(move(p));
        }
        
        return true;
    }
    
    
    // for map, multimap and etc.
    
    template<
        class Serializer, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >
    void BMSer(Serializer& s, Collection<K, T, Traits, Allocator>& c)
    {
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](pair<K, T>&& v)
            {
                T t = move(get<1>(v));
                s.Serialize(t);
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    > 
    void BMSer(Serializer& s, Collection<K, T*, Traits, Allocator>& c)
    {
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](pair<K, T*>&& v)
            {
                s.Serialize(*get<1>(v));
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >  
    void BMSer(Serializer& s, Collection<K, unique_ptr<T>, Traits, Allocator >& c)
    {        
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](pair<K, unique_ptr<T> >&& v)
            {
                s.Serialize(*get<1>(v).get());
            }
        );
    }
    
    template<
        class Serializer, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >  
    void BMSer(Serializer& s, Collection<K, shared_ptr<T>, Traits, Allocator >& c)
    {        
        s.Serialize((int)c.size());
        for_each(begin(c), end(c), [&](pair<K, shared_ptr<T> >&& v)
            {
                s.Serialize(*get<1>(v).get());
            }
        );
    }
    
    template<
        class Serializer, 
        class Function, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >
    bool BMDeser(Serializer& s, Collection<K, T, Traits, Allocator>& c, Function func)
    {
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            T t;
            if (!s.Deserialize(t))
            {
                c.clear();
                return false;
            }
            c[func(t)] = move(t);
        }
        
        return true;        
    }
    
    template<
        class Serializer, 
        class Function, 
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    > 
    bool BMDeser(Serializer& s, Collection<K, T*, Traits, Allocator>& c, Function func)
    {
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            T* p = new T();
            if (!s.Deserialize(*p))
            {
                delete p;
                for_each(begin(c), end(c), [&](pair<K, T* >&& v)
                    {
                        delete get<1>(v);
                    }                
                );
                c.clear();
                return false;
            }
            c[func(p)] = p;
        }
        
        return true;  
    }
    
    template<
        class Serializer, 
        class Function,             
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >  
    bool BMDeser(Serializer& s, Collection<K, unique_ptr<T>, Traits, Allocator >& c, Function func)
    {        
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            unique_ptr<T> p(new T());
            if (!s.Deserialize(*p.get()))
            {
                c.clear();
                return false;
            }
            c[func(p.get())] = move(p);
        }
        
        return true; 
    }
    
    template<
        class Serializer, 
        class Function,             
        template<class, class, class, class> class Collection, 
        class K, 
        class T, 
        class Traits = less<K>, 
        class Allocator = std::allocator<pair<const K, T> > 
    >  
    bool BMDeser(Serializer& s, Collection<K, shared_ptr<T>, Traits, Allocator >& c, Function func)
    {        
        size_t cb;
        s.Deserialize(cb);
        for (size_t i = 0; i < cb; ++i)
        {
            auto p = make_shared<T>();
            if (!s.Deserialize(*p.get()))
            {
                c.clear();
                return false;
            }
            c[func(p.get())] = move(p);
        }
        
        return true; 
    }
    
    
}


#endif // _BSFUNC_H_
