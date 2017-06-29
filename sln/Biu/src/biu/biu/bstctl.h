
#include "biu.h"

#ifndef _BSTCTL_H_
#define _BSTCTL_H_


#include <atomic>
#include <functional>
#include "butility.h"


using namespace std;


namespace Biu {
	
	
	// Class BTwoStatesSwitch
	
	template<class T, class V = T>
	class BTwoStatesSwitch
	{
	public:
		typedef T               Type;
		typedef T&              Type_Reference;
		
		BTwoStatesSwitch(const Type_Reference t, V in, V out)
				: m_t(t)
				, m_out(out)
		{
			m_t = in;
		}
		
		~BTwoStatesSwitch()
		{
			m_t = m_out;
		}
		
	private:
		T& m_t;
		V m_out;
	};
	
	
	// Class BAtomicBoolSwitch
	
	class BAtomicBoolSwitch
	{
	public:
		inline BAtomicBoolSwitch(atomic_bool& v, bool in = true, bool out = false)
				: m_v(v)
				, m_out(out)
		{
			m_v = in;
		}
		
		inline ~BAtomicBoolSwitch()
		{
			m_v = m_out;
		}
		
	private:
		atomic_bool& m_v;
		bool m_out;
	};
	
	
	// Class BAtomicCountSwitch
	
	template<class T, class Func, int in = 1, int out = 1>
	class BAtomicCountSwitch
	{
	public:
		typedef T               Type;
		typedef T&              Type_Reference;
		
		BAtomicCountSwitch(const Type_Reference v, Func func)
				: m_v(v)
				, m_func(func)
		{
			m_v += in;
		}
		
		~BAtomicCountSwitch()
		{
			if (!(m_v -= out))
			{
				m_func();
			}
		}
		
	private:
		Type_Reference m_v;
		Func m_func;
	};


	// Class BAtomicSimpleCountSwitch

	template<class T, int in = 1, int out = 1>
	class BAtomicSimpleCountSwitch
	{
	public:
		typedef T               Type;
		typedef T&              Type_Reference;

		BAtomicSimpleCountSwitch(const Type_Reference v)
			: m_v(v)
		{
			m_v += in;
		}

		~BAtomicSimpleCountSwitch()
		{
			m_v -= out;
		}

	private:
		Type_Reference m_v;
	};
	
	
	// Class BScopeCaller
	
	template<class FuncIn, class FuncOut>
	class BScopeCaller
	{
	public:
		BScopeCaller(FuncIn funcIn, FuncOut funcOut)
				: m_funcIn(funcIn)      
				, m_funcOut(funcOut)
		{
			m_funcIn();
		}
		
		~BScopeCaller()
		{
			m_funcOut();
		}
		
	private:
		FuncIn m_funcIn;
		FuncOut m_funcOut;
	};


	// Class BAtomicCountChanger

	template<class T, class TC, int CV = 1>
	class BAtomicCountChanger
	{
	public:
		typedef T               Type;
		typedef T&              Type_Reference;
		typedef TC				ConditionType;
		typedef TC&				ConditionType_Reference;

		BAtomicCountChanger(const Type_Reference v, const ConditionType_Reference c)
			: m_v(v)
			, m_c(c)
		{
		}

		~BAtomicCountChanger()
		{
			if (m_c)
			{
				m_v += CV;
			}
		}

	private:
		Type_Reference m_v;
		ConditionType_Reference m_c;
	};
	
	
}


#endif // _BSTCTL_H_
