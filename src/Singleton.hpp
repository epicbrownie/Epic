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
	template<typename T>
	class Singleton;
}

//////////////////////////////////////////////////////////////////////////////

// Singleton
template<typename T>
class Epic::Singleton
{
public:
	using singleton_type = std::decay_t<T>;

	static_assert(std::is_class<singleton_type>::value, "Singleton type must be a class type.");
	static_assert(std::is_default_constructible<singleton_type>::value, "Singleton type must be default constructible.");

private:
	Singleton() = delete;

private:
	struct _Creator
	{
		_Creator() noexcept { Singleton<T>::Instance(); }
		inline void DoNothing() const noexcept { }
	};

public:
	static singleton_type& Instance() noexcept
	{
		// The singleton instance
		// C++ guarantees that s_Instance is created in a thread-safe manner
		static singleton_type s_Instance;

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
template<typename T>
decltype(Epic::Singleton<T>::s_Creator) Epic::Singleton<T>::s_Creator;


