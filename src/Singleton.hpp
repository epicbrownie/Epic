//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2016 Ronnie Brohn (EpicBrownie)      
//
//                Distributed under The MIT License (MIT).
//             (See accompanying file License.txt or copy at 
//                 https://opensource.org/licenses/MIT)
//
//           Please report any bugs, typos, or suggestions to
//              https://github.com/epicbrownie/Epic/issues
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	struct SingletonTag;

	template<class T, class Tag = SingletonTag>
	class Singleton;
}

//////////////////////////////////////////////////////////////////////////////

// Singleton
template<class T, class Tag>
class Epic::Singleton
{
public:
	using SingletonType = std::decay_t<T>;

	static_assert(std::is_class<SingletonType>::value, "Singleton type must be a class Type.");
	static_assert(std::is_default_constructible<SingletonType>::value, "Singleton type must be default constructible.");

private:
	Singleton() = delete;

private:
	struct _Creator
	{
		_Creator() noexcept { Singleton<T>::Instance(); }
		inline void DoNothing() const noexcept { }
	};

public:
	static SingletonType& Instance() noexcept
	{
		// The singleton instance
		// C++ guarantees that s_Instance is created in a thread-safe manner
		static SingletonType s_Instance;

		// Calling DoNothing forcibly instantiates
		// and initializes the object prior to main()
		s_Creator.DoNothing();

		return s_Instance;
	}

private:
	// When s_Creator is initialized, it will
	// try to call Instance (forcibly instantiating it)
	static _Creator s_Creator;
};

//////////////////////////////////////////////////////////////////////////////

// Static Initialization
template<class T, class Tag>
decltype(Epic::Singleton<T, Tag>::s_Creator) Epic::Singleton<T, Tag>::s_Creator;
