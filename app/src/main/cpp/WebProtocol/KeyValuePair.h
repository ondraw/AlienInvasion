/*
 * Mink is a mobile platform developed by Mink, Inc. 
 * Copyright (C) 2003 - 2007 Mink, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY Mink, Mink DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Mink, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@Mink.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Mink" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Mink".
 */


#ifndef INCL_KEY_VALUE_PAIR
#define INCL_KEY_VALUE_PAIR
/** @cond DEV */

#include "fscapi.h"
#include "ArrayElement.h"
#include "globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class is an ArrayElement that keeps a key-value pair.
 */
class MINKBASE_EXPORT_F KeyValuePair : public ArrayElement {
    public:

        KeyValuePair(const char*  key = NULL, const char*  value = NULL);
        virtual ~KeyValuePair();
        ArrayElement* clone();

        /**
         * Sets the key. The string is duplicated so that the caller can
         * independently release its copy. If key is NULL, the older value
         * is released.
         *
         * @param key the new key
         */
        void setKey(const char*  key);

        /**
         * Sets the value. The string is duplicated so that the caller can
         * independently release its copy. If value is NULL, the older value
         * is released.
         *
         * @param value the new value
         */

        void setValue(const char*  value);

        /**
         * Returns the key (the internal buffer address is returned).
         */
        const char*  getKey();

        /**
         * Returns the value (the internal buffer address is returned).
         */
        const char*  getValue();

    private:
        char*  k;
        char*  v;
};


END_NAMESPACE

/** @endcond */
#endif
