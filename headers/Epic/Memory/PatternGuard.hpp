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

#include <Epic/TMP/Utility.hpp>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class MemoryCorruptedException;

	struct GuardResponseIgnore;
	struct GuardResponseThrow;
	struct GuardResponseCErr;
	struct GuardResponseCOut;
	struct GuardResponseAssert;
	
	namespace detail
	{
		template<size_t Pattern, class ResponsePolicy>
		class PatternGuardImpl;
	}

	using DefaultGuardPattern = 
		std::integral_constant<size_t,
			(sizeof(size_t) == 2) ? 0xDEAD :
			(sizeof(size_t) == 4) ? 0xDEADC0DE : 
									0xDEADC0DEDEADC0DE
		>;
}

//////////////////////////////////////////////////////////////////////////////

/// MemoryCorruptedException
class Epic::MemoryCorruptedException : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

//////////////////////////////////////////////////////////////////////////////

/// GuardResponseIgnore
struct Epic::GuardResponseIgnore
{
	constexpr bool CheckGuard(size_t Expected, size_t Obtained) const noexcept 
	{
		return true;
	}
};

/// GuardResponseThrow
struct Epic::GuardResponseThrow
{
	bool CheckGuard(size_t Expected, size_t Obtained) const noexcept(false)
	{
		if (Obtained != Expected)
		{
			std::string msg;

			{
				std::ostringstream str;
				str << "PatternGuard detected corrupted memory. [Expected: 0x"
					<< std::uppercase << std::hex
					<< Expected
					<< "; Detected: 0x"
					<< Obtained
					<< "]";

				msg = str.str();
			}

			throw Epic::MemoryCorruptedException(msg);
		}

		return true;
	}
};

/// GuardResponseCErr
struct Epic::GuardResponseCErr
{
	bool CheckGuard(size_t Expected, size_t Obtained) const noexcept
	{
		if (Obtained != Expected)
		{
			std::cerr.setf(std::ios_base::uppercase);

			std::cerr << "PatternGuard detected corrupted memory. [Expected: 0x"
					  << std::hex
					  << Expected
					  << "; Detected: 0x"
					  << Obtained
					  << "]"
					  << std::dec
					  << std::endl;

			std::cerr.unsetf(std::ios_base::uppercase);

			return false;
		}

		return true;
	}
};

/// GuardResponseCOut
struct Epic::GuardResponseCOut
{
	bool CheckGuard(size_t Expected, size_t Obtained) const noexcept
	{
		if (Obtained != Expected)
		{
			std::cout.setf(std::ios_base::uppercase);
			
			std::cout << "PatternGuard detected corrupted memory. [Expected: 0x"
				<< std::hex
				<< Expected
				<< "; Detected: 0x"
				<< Obtained
				<< "]"
				<< std::dec
				<< std::endl;

			std::cout.unsetf(std::ios_base::uppercase);

			return false;
		}

		return true;
	}
};

/// GuardResponseAssert
struct Epic::GuardResponseAssert
{
	bool CheckGuard(size_t Expected, size_t Obtained) const noexcept
	{
		assert((Obtained == Expected) && "PatternGuard detected corrupted memory.");

		return (Obtained == Expected);
	}
};

//////////////////////////////////////////////////////////////////////////////

/// PatternGuard<P>
template<size_t P, class Response>
class Epic::detail::PatternGuardImpl
	: private Response
{
	static_assert(std::is_nothrow_default_constructible<Response>::value,
		"PatternGuard ResponsePolicy must have a non-throwing default constructor.");

public:
	using type = Epic::detail::PatternGuardImpl<P, Response>;
	using ResponsePolicy = Response;

	static constexpr size_t Pattern = P;

public:
	PatternGuardImpl() noexcept
		: m_Pattern(Pattern) { }

	~PatternGuardImpl()
	{
		ResponsePolicy::CheckGuard(Pattern, m_Pattern);
	}

public:
	bool CheckGuard() const
	{
		return ResponsePolicy::CheckGuard(Pattern, m_Pattern);
	}

private:
	size_t m_Pattern;
};

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	template<size_t Pattern = Epic::DefaultGuardPattern::value, class DebugResponsePolicy = Epic::GuardResponseCErr, class ReleaseResponsePolicy = Epic::GuardResponseIgnore>
	using PatternGuard = 
		detail::PatternGuardImpl<Pattern, typename TMP::DebugSwitch<DebugResponsePolicy, ReleaseResponsePolicy>::type>;
}
