//
//  CListMap.h
//  SongGL
//
//  Created by Song Hohak on 11. 11. 25..
//  Copyright (c) 2011 thinkm. All rights reserved.
//

#ifndef SongGL_CListMap_h
#define SongGL_CListMap_h

#include "CList.h"

/**
 * Simple Map. Similar to the Java or STL implementation.
 */
template <class KEY, class VALUE> 
class CListMap
{
	
private:
	/**
	 * Dynamic array with the keys.
	 */
	CList<KEY>		keys;
	
	/**
	 * Dynamic array with the values.
	 */
	CList<VALUE>		values;
	
	/**
	 * Null element.
	 */
	VALUE			nullVALUE;
    
public:
	
	/**
	 * Constructor of the Map.
	 */
	CListMap()
	{
	}
	
	/**
	 * Destructor of the Map. Elements are removed.
	 */
	virtual ~CListMap()
	{
		clear();
		trimToSize();
	}
    
	/**
	 * Removes all elements from the map.
	 */
	GLvoid clear()
	{
		keys.clear();
		values.clear();
	}
	
	/**
	 * Returns true, if the key is in the Map.
	 * 
	 * @param	key the key
	 * 
	 * @return  true, if key is available	
	 */
	GLboolean containsKey(KEY key)
	{
		return keys.contains(key);
	}
	
	/**
	 * Returns true, if the value is in the Map.
	 * 
	 * @param	value the value
	 * 
	 * @return  true, if value is available	
	 */
	GLboolean containsValue(VALUE value)
	{
		return values.contains(value);
	}
	
	/**
	 * Returns the value by key.
	 * 
	 * @param key the key of the element
	 * 
	 * @return a reference to the element
	 */
	VALUE& get(KEY key)
	{
		GLint index = keys.indexOf(key);
		
		if (index != -1)
			return values.get(index);
		
		memset(&nullVALUE, 0, sizeof(VALUE));		
		
		return nullVALUE;
	}
	
	/**
	 * Returns the keys of the map.
	 * 
	 * @return the list of keys
	 */
	CList<KEY>& getKeys()
	{
		return keys;
	}
    
	/**
	 * Returns the values of the map.
	 * 
	 * @return the list of values
	 */
	CList<VALUE>& getValues()
	{
		return values;
	}
	
	/**
	 * Puts a element with key in the map. If it already exists,
	 * the element is overwritten.
	 * 
	 * @param key the key of the element
	 * @param value the value
	 */
	GLvoid put(KEY key, VALUE value)
	{
		GLint index = keys.indexOf(key);
		
		if (index != -1)
		{
			keys[index] = key;
			values[index] = value;
		}
		else
		{
			keys.add(key);
			values.add(value);
		}
	}
	
	/**
	 * Removes key and its associated value from the map.
	 * 
	 * @param key the key
	 */
	GLvoid remove(KEY key)
	{
		GLint index = keys.indexOf(key);
		
		if (index != -1)
		{
			keys.remove(index);
			values.remove(index);
		}
	}
	
	/**
	 * Returns the size of the map.
	 * 
	 * @return the size
	 */
	GLuint size()
	{
		return keys.size();
	}
    
	/**
	 * Trims the map with capacity equals size.
	 */
	GLvoid trimToSize()
	{
		keys.trimToSize();
		values.trimToSize();
	}
    
};	

#endif
