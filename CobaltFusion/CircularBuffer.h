// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#pragma once

#include "windows.h"
#include "boost/noncopyable.hpp"
#include <memory>
#include <string>
#include <boost/thread.hpp>

namespace fusion {
namespace debugviewpp {

class CircularBuffer : boost::noncopyable
{
public:
	explicit CircularBuffer(size_t size);
	virtual ~CircularBuffer();

protected:
	inline size_t PtrAdd(size_t value, size_t add) const
	{
		return ((value + add) & (m_size-1));
	}

	inline const char* ReadPointer()
	{
		return m_buffer.get()+m_readOffset;
	}

	inline char* WritePointer()
	{
		return m_buffer.get()+m_writeOffset;
	}

	static int GetPowerOfTwo(int size);
	virtual bool Empty() const;
	virtual bool Full() const;
	virtual void WaitForReader();

	virtual size_t GetFree() const;
	virtual size_t GetCount() const;

	template <class T> T Read();
	std::string ReadMessage();
	template <class T> void Write(T type);
	void WriteMessage(const char* message);

	size_t m_size;
	std::unique_ptr<char> m_buffer;
	char* m_pBegin;
	char* m_pEnd;
	size_t m_readOffset;
	size_t m_writeOffset;

	boost::condition_variable m_triggerRead;
};

template <class T> 
T CircularBuffer::Read()
{
	auto pBuffer = (T*) m_buffer.get() + m_readOffset;
	m_readOffset = PtrAdd(m_readOffset, sizeof(T));
	return *pBuffer;
}

template <class T> 
void CircularBuffer::Write(T value)
{
	auto pBuffer = (T*) m_buffer.get() + m_writeOffset;
	*pBuffer = value;
	m_writeOffset = PtrAdd(m_writeOffset, sizeof(T));
}

} // namespace debugviewpp 
} // namespace fusion