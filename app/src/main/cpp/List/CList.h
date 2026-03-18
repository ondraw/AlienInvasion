//
//  CList.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 25..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CList_h
#define SongGL_CList_h

#if defined ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif defined IPHONE
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else //MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#endif
#include <string.h>
/**
 * The size which the dynamic array gets enlarged.
 */
#define	SEGMENTSIZE		32

/**
 * Simple List. Similar to the Java or STL implementation.
 */
template <class T> 
class CList
{
	
private:
    
	/**
	 * Current capacity of the list.
	 */
	GLint	capacity;
	
	/**
	 * Index to the last element in the list.
	 */
	GLint	last;
	
	/**
	 * The values of the list itself.
	 */
	T* values;
	
	/**
	 * Null element.
	 */
	T  nullT;
	
public:
    
	/**
	 * Constructor for the list.
	 */
	CList() :
	capacity(0),
	last(-1),
	values(0)
	{
	}
	
	/**
	 * Destructor of the list.
	 */
	virtual ~CList()
	{
		clear();
		trimToSize();
	}
    
	/**
	 * Adds an element to the list. If needed the array is enlarged.
	 * 
	 * @param	index	the position where to add
	 * @param	value	the value to be inserted
	 */
	GLvoid add(GLint index, T value)
	{
		last++;
        
		if (index < 0 || index > last)
			return;
        
		if (last >= capacity)
		{
			T* oldvalues = values;
			
			capacity += SEGMENTSIZE;
			
			values = new T[capacity];
			
			if (!values)
			{
				values = oldvalues;
				last--;				
				return;
			}
			
			if (oldvalues)
			{		
				// copy before
				for (GLint i = 0; i < index; i++)
				{
					values[i] = oldvalues[i];
				}
				
				// copy after
				for (GLint i = index; i <= last-1; i++)
				{
					values[i] = oldvalues[i];
				}
                
				delete[] oldvalues;
			}
		}
		else
		{
			// move
			for (GLint i = last-1; i >= index; i--)
			{
				values[i+1] = values[i];
			}
		}
		values[index] = value;
	}
    
	/**
	 * Adds a element at the end of the list.
	 * 
	 * @param value	the value to add
	 */
	GLvoid add(T value)
	{
		last++;
		
		if (last >= capacity)
		{
			T* oldvalues = values;
            
			capacity += SEGMENTSIZE;
			
			values = new T[capacity];
			
			if (!values)
			{
				values = oldvalues;
				last--;				
				return;
			}
            
			if (oldvalues)
			{		
				// copy the previous values
				for (GLint i = 0; i <= last-1; i++)
				{
					values[i] = oldvalues[i];
				}
                
				delete[] oldvalues;
			}
		}
        
		values[last] = value;		
	}
	
	/**
	 * Clears and empties the list.
	 */
	GLvoid clear()
	{
		if (values)
		{
			delete[] values;
			values = 0;
			capacity = 0;		
			last = -1;
		}
	}
	
	/**
	 * Returns, if the lement is in the list.
	 * 
	 * @param value the element to look for
	 * 
	 * @return true, if the element is in the list
	 */
	GLboolean contains(T value)
	{
		if (!values)
			return GL_FALSE;
		
		for (GLint i = 0; i <= last; i++)
		{
			if (value == values[i])
				return GL_TRUE;
		}
        
		return GL_FALSE;
	}
	
	/**
	 * Returns the reference of the nth element.
	 * 
	 * @param index	the nth element
	 * 
	 * @return reference to the element in the list
	 */
	T& get(GLint index)
	{
		if (!values || index < 0 || index > last)
		{
			memset(&nullT, 0, sizeof(T));
			
			return nullT;
		}
		
		return values[index];
	}
	
	/**
	 * Returns the position of an element in the list.
	 * 
	 * @param value	the element to search for
	 * 
	 * @return the index of the element. -1 if not found
	 */
	GLint indexOf(T value)
	{
		if (!values)
			return -1;
        
		for (GLint i = 0; i <= last; i++)
		{
			if (value == values[i])
				return i;
		}
        
		return -1;		
	}
	
	/**
	 * Removes an element at the given position.
	 * 
	 * @param	index the position
	 */
	GLvoid remove(GLint index)
	{
		if (!values)
			return;
        
		if (index < 0 || index > last)
			return;
        
		for (GLint i = index + 1; i <= last; i++)
		{
			values[i-1] = values[i];
		}
		last--;		
	}
    
	/**
	 * Removes the given element from the list.
	 * 
	 * @param value te element to be removed
	 */
	GLvoid removeElement(T value)
	{
		if (!values)
			return;
        
		for (GLint i = 0; i <= last; i++)
		{
			if (value == values[i])
			{
				remove(i);
				break;
			}	
		}
	}
	
	/**
	 * Sets a value at the given position. The old value is overwritten.
	 * 
	 * @param index the psoition where to put the element
	 * @param value the element itself
	 */
	GLvoid set(GLint index, T value)
	{
		if (!values)
			return;
        
		if (index < 0 || index > last)
			return;
        
		values[index] = value;			
	}
    
	/**
	 * Returns the size of the list.
	 * 
	 * @return the size
	 */
	GLuint size()
	{
		return last + 1;
	}
	
	/**
	 * Trims the list with capacity equals size.
	 */
	GLvoid trimToSize()
	{
		if (!values)
			return;
        
		T* oldvalues = values;
		
		capacity = size();
		
		if (capacity == 0)
		{
			delete[] oldvalues;
			values = 0;
			return;
		}
        
		values = new T[capacity];
        
		if (!values)
		{
			values = oldvalues;
			return;
		}
        
		// copy the values
		for (GLint i = 0; i <= last; i++)
		{
			values[i] = oldvalues[i];
		}
        
		delete[] oldvalues;
	}
    
	/**
	 * Returns the nth element out of the dynamic array.
	 * 
	 * @param index	the nth element
	 * 
	 * @return reference to the element
	 */
	T& operator [](GLint index)
	{
		if (!values || index < 0 || index > last)
		{
			memset(&nullT, 0, sizeof(T));
            
			return nullT;
		}
        
		return values[index];
	}
	
};

#endif
