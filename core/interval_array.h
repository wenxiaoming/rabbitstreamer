/*
 *  Openmysee
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef CORE_RS_INTERVAL_INTERVAL_H_
#define CORE_RS_INTERVAL_INTERVAL_H_

#include "core/core_struct.h"
#include <limits.h>
#include <assert.h>
#include <stdio.h>

using namespace std;

namespace rs {
namespace core {

class IntervalArray 
{
public:

	explicit IntervalArray() : m_array(NULL), m_totalsize(0), m_validsize(0) {
	};

	explicit IntervalArray(const IntervalArray& another) : m_array(NULL), m_totalsize(another.m_totalsize), m_validsize(another.m_validsize) {
		if(m_totalsize) 		
		{
			m_array = new BlockInterval[m_totalsize];
			memcpy(m_array, another.m_array, m_validsize*sizeof(BlockInterval));
		}
	};
	virtual ~IntervalArray(void) {
		m_totalsize = 0;
		m_validsize = 0;
		delete [] m_array;
		m_array = NULL;
	};

	bool operator == (const IntervalArray& a) const {
		if(m_validsize != a.m_validsize)
			return false;
		if(memcmp(m_array, a.m_array, sizeof(BlockInterval)*m_validsize) != 0)
			return false;
		return true;
	};
	IntervalArray& operator=(const IntervalArray& another) {
		m_totalsize = m_validsize = another.m_validsize;
		delete [] m_array;
		m_array = NULL;

		if(m_totalsize) 
		{
			m_array = new BlockInterval[m_totalsize];
			memcpy(m_array, another.m_array, m_validsize*sizeof(BlockInterval));
		}
        return *this;
	}

	void DeleteArray(const IntervalArray& another) {
		for(uint16_t i = 0; i < another.m_validsize; ++i)
		{
			DelInterval(another.m_array[i].start, another.m_array[i].size);
		}
	}

	void AndOperator(const IntervalArray& another, IntervalArray& result) const	{
		BlockInterval temp;
		result.Clear();
		for(uint16_t i = 0; i < another.m_validsize; ++i) {
			for(uint16_t j = 0; j < m_validsize; ++j) {
				BlockInterval::and_op(m_array[j], another.m_array[i], temp);
				if(temp.size > 0)
					result.AddInterval(temp.start, temp.size);
			}
		}
	}

	void AddInterval(uint32_t start, uint32_t size) {
		if(start == UINT_MAX)
			return;
//		assert(size != 0);
		if(size == 0)
			return;
        uint16_t i = 0;
		for(i = 0; i < m_validsize; ++i) {
			if(m_array[i].start > start) {
				if(m_array[i].start > start+size) {
					break;
				}
				else if(m_array[i].start+m_array[i].size >= start+size) {
					m_array[i].size += m_array[i].start-start;
					m_array[i].start = start;
					return;
				}
				else {
					SafeDelete(i);
					AddInterval(start, size);
					return;
				}
			}
			else if(m_array[i].start+m_array[i].size >= start) {
				if(m_array[i].start+m_array[i].size >= start+size) {
					return;
				}
				if(i == m_validsize-1 || m_array[i+1].start > start+size) {
					m_array[i].size = start+size-m_array[i].start;
					return;
				}
	
				size += start - m_array[i].start;
				start = m_array[i].start;

				SafeDelete(i);
				AddInterval(start, size);
				return;
			}
		}
		SafeInsert(i, start, size);
		return;
	};

	void DelInterval(uint32_t start, uint32_t size) {
		assert(size != 0);
		if(size == 0)
			return;
		for(uint16_t i = 0; i < m_validsize; ++i) {
			if(m_array[i].start > start) {
				if(m_array[i].start > start+size) {
					return;
				}
				else if(m_array[i].start+m_array[i].size > start+size)	{
					m_array[i].size = (m_array[i].start+m_array[i].size) - (start+size);
					m_array[i].start = start+size;
					return;
				} else {
					SafeDelete(i);
					DelInterval(start, size);
					return;
				}
			}
			else if(m_array[i].start+m_array[i].size > start) {
				uint32_t oldSize = m_array[i].size;

				if(m_array[i].start != start) {
					m_array[i].size = start-m_array[i].start;
				}

				if(m_array[i].start+oldSize <= start+size) {
					if(m_array[i].start != start) {
						DelInterval(start, size);
						return;
					}
					SafeDelete(i);
					return;
				}

				if(m_array[i].start == start) {
					m_array[i].size -= start+size-m_array[i].start;
					m_array[i].start = start+size;
					return;
				}
				SafeInsert(i+1, start+size, m_array[i].start+oldSize - (start+size));
				return;
			}
		}
	};

	bool FindBlock(const uint32_t blockID) const {
		for(uint16_t i = 0; i < m_validsize; ++i) {
			if(m_array[i].start > blockID)
				return false;

			else if(m_array[i].start+m_array[i].size > blockID)
				return true;
		}
		return false;
	};

	void CopyIntervalArray(BlockInterval* targetArray, uint8_t& size) const	{
		if(!targetArray || size == 0)
			return;
		if(size >= m_validsize) {
			size = static_cast<uint8_t>(m_validsize);
			memcpy(targetArray, m_array, size*sizeof(BlockInterval));
		}
		else {
			std::sort(m_array, m_array+m_validsize, BlockInterval::cmp_size);	
			memcpy(targetArray, m_array, size*sizeof(BlockInterval));		
			std::sort(m_array, m_array+m_validsize, BlockInterval::cmp_start);	
			std::sort(targetArray, targetArray+size, BlockInterval::cmp_start);	
		}
	};

	void Clear() {
		m_validsize = 0;
	};

	bool IsEmpty() const {
		return (m_validsize == 0);
	}

	uint32_t GetMaxBlockID() const {
		if(m_validsize)
			return m_array[m_validsize-1].start+m_array[m_validsize-1].size;
		return 0;
	};

	uint32_t GetMinBlockID() const {
		if(m_validsize)
			return m_array[0].start;
		return UINT_MAX;
	}

	uint16_t GetValidSize() const {
		return m_validsize;
	};

	uint32_t GetCountInInterval(const uint32_t blockID, const uint32_t len) const {
		uint32_t total = 0;

		BlockInterval result, in;
		in.start = blockID;
		in.size = min(len, UINT_MAX-blockID);

		for(uint16_t i = 0; i < m_validsize; ++i) 
		{
			BlockInterval::and_op(m_array[i], in, result);
			total += result.size;
		}
		return total;
	};

	uint32_t GetContinousCount(const uint32_t blockID) const {
		for(uint16_t i = 0; i < m_validsize; ++i) {
			if(m_array[i].start > blockID)
				return 0;
			else if(m_array[i].start+m_array[i].size > blockID) {
				return m_array[i].start+m_array[i].size-blockID;
			}
		}
		return 0;
	};

	void PopFront(BlockInterval& bi) {
		if(m_validsize) {
			bi = m_array[0];
			SafeDelete(0);
		}
		else
			memset(&bi, 0, sizeof(bi));
	}

	void Print() const {
		printf("\nTOTAL: %d, VALID: %d.\n", m_totalsize, m_validsize);
		for(uint16_t i = 0; i < m_validsize; ++i) {
			printf("START: %d, END: %d, SIZE: %d.\n", m_array[i].start, m_array[i].start+m_array[i].size, m_array[i].size);
		}
	}

	bool Verify() const {
		for(uint16_t i = 0; i < m_validsize; ++i) {
			if(m_array[i].size == 0)
				return false;
			if(UINT_MAX - m_array[i].start < m_array[i].size)
				return false;
			if(i == m_validsize-1)
				break;
			if(m_array[i].start + m_array[i].size >= m_array[i+1].start)
				return false;
		}
		return true;
	}

protected:
	void SafeInsert(uint16_t i, uint32_t start, uint32_t size)	{
		assert(m_validsize <= m_totalsize);
		assert(i <= m_validsize);
		assert(m_validsize <= 0xffff);

		if(m_validsize == 0xffff)
			return;

		BlockInterval* temp = m_array;
		if(m_totalsize == m_validsize)	{
			m_array = new BlockInterval[m_validsize+1];
			memcpy(m_array, temp, i*sizeof(BlockInterval));
		}

		memcpy(m_array+i+1, temp+i, (m_validsize-i)*sizeof(BlockInterval));

		m_array[i].start = start;
		m_array[i].size = size;

		if(m_totalsize == m_validsize) {
			delete [] temp;
			++m_totalsize;
		}

		++m_validsize;
		assert(m_validsize <= 0xffff);
	};

	void SafeDelete(uint16_t i) {
		assert(i < m_validsize);
		::memmove(m_array+i, m_array+i+1, (m_validsize-i-1)*sizeof(BlockInterval));
		--m_validsize;
	}

private:
	BlockInterval* m_array;
	uint16_t m_totalsize;
	uint16_t m_validsize;
};

} // namespace rs::core

#endif