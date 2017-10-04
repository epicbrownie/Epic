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

#include <Epic/EON/Error.hpp>
#include <optional>
#include <string>
#include <string_view>

//////////////////////////////////////////////////////////////////////////////

namespace Epic::EON
{
	template<class T>
	class ThrowResult;

	template<class T>
	class OptionalResult;

	template<class T>
	class DefaultResult;

	template<class T>
	class SafeResult;

	template<class T>
	class GuaranteedResult;
}

//////////////////////////////////////////////////////////////////////////////

// ThrowResult
//		Success - Returns a T
//		Empty - Returns a default constructed T
//				Throws SelectionEmptyException for required fields
//		Failed - Throws ExtractionFailedException

template<class T>
class Epic::EON::ThrowResult
{
public:
	using Type = Epic::EON::ThrowResult<T>;
	using ResultType = T;

public:
	ResultType Success(T&& result) const
	{
		return std::move(result);
	}

	ResultType Empty(std::string_view selector, bool isOptional) const
	{
		if (!isOptional)
		{
			std::string msg = "A required field was not found: ";
			msg.append(selector);

			throw SelectionEmptyException(msg);
		}

		return T();
	}

	ResultType Failed(std::string_view selector) const
	{
		std::string msg = "Failed while extracting field: ";
		msg.append(selector);

		throw ExtractionFailedExeception(msg);
	}
};

//////////////////////////////////////////////////////////////////////////////

// OptionalResult
//		Success - Returns an optional<T> with a value
//		Empty - Returns an optional<T> without a value
//		Failed - Returns an optional<T> without a value

template<class T>
class Epic::EON::OptionalResult
{
public:
	using Type = Epic::EON::OptionalResult<T>;
	using ResultType = std::optional<T>;

public:
	ResultType Success(T&& result)
	{
		return std::make_optional(std::move(result));
	}

	ResultType Empty(std::string_view, bool) noexcept
	{
		return std::nullopt;
	}

	ResultType Failed(std::string_view) noexcept
	{
		return std::nullopt;
	}
};

/////////////////////////////////////////////////////////////////////////////

// DefaultResult
//		Success - Returns a T
//		Empty - Returns default T
//				Throws SelectionEmptyException for required fields
//		Failed - Throws ExtractionFailedException

template<class T>
class Epic::EON::DefaultResult
{
public:
	using Type = Epic::EON::DefaultResult<T>;
	using ResultType = T;

private:
	ResultType m_DefaultValue;

public:
	template<class D>
	explicit DefaultResult(D&& defaultValue)
		: m_DefaultValue{ std::forward<D>(defaultValue) } { };

	DefaultResult() = delete;

public:
	ResultType Success(T&& result) const
	{
		return std::move(result);
	}

	ResultType Empty(std::string_view selector, bool isOptional) const
	{
		if (!isOptional)
		{
			std::string msg = "A required field was not found: ";
			msg.append(selector);

			throw SelectionEmptyException(msg);
		}

		return m_DefaultValue;
	}

	ResultType Failed(std::string_view selector) const
	{
		std::string msg = "Failed while extracting field: ";
		msg.append(selector);

		throw ExtractionFailedExeception(msg);
	}
};

//////////////////////////////////////////////////////////////////////////////

// SafeResult
//		Success - Returns an optional<T> with a value
//		Empty - Returns an optional<T> with a default value
//		Failed - Returns an optional<T> without a value

template<class T>
class Epic::EON::SafeResult
{
public:
	using Type = Epic::EON::SafeResult<T>;
	using ResultType = std::optional<T>;

private:
	ResultType m_DefaultValue;

public:
	template<class D>
	explicit SafeResult(D&& defaultValue)
		: m_DefaultValue{ std::make_optional(std::forward<D>(defaultValue)) } { }

	SafeResult() = delete;

public:
	ResultType Success(T&& result)
	{
		return std::make_optional(std::move(result));
	}

	ResultType Empty(std::string_view, bool) noexcept
	{
		return m_DefaultValue;
	}

	ResultType Failed(std::string_view) noexcept
	{
		return std::nullopt;
	}
};

//////////////////////////////////////////////////////////////////////////////

// GuaranteedResult
//		Success - Returns a T
//		Empty - Returns default T
//		Failed - Returns default T

template<class T>
class Epic::EON::GuaranteedResult
{
public:
	using Type = Epic::EON::GuaranteedResult<T>;
	using ResultType = T;

private:
	ResultType m_DefaultValue;

public:
	template<class D>
	explicit GuaranteedResult(D&& defaultValue)
		: m_DefaultValue{ std::forward<D>(defaultValue) } { }

	GuaranteedResult() = delete;

public:
	ResultType Success(T&& result)
	{
		return std::move(result);
	}

	ResultType Empty(std::string_view, bool) noexcept
	{
		return m_DefaultValue;
	}

	ResultType Failed(std::string_view) noexcept
	{
		return m_DefaultValue;
	}
};
