/****************************************************************************
**
** Copyright (C) 2011 Darryl L. Miles.  All rights reserved.
** Copyright (C) 2011 D L Miles Consulting Ltd.  All rights reserved.
**
** This file is part of Qt Jambi.
**
**
** $BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
** 
** In addition, as a special exception, the copyright holders grant you
** certain additional rights. These rights are described in the Nokia Qt
** LGPL Exception version 1.0, included in the file LGPL_EXCEPTION.txt in
** this package.
** 
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL3 included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html
** $END_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

package generator;

import org.qtjambi.qt.*;
import org.qtjambi.qt.internal.QtJambiObject.QPrivateConstructor;
import org.qtjambi.qt.core.*;

class QObjectType___ extends QObjectType {
        @Override
        @org.qtjambi.qt.QtBlockedSlot
        protected void finalize() {
            try {
                if(Utils.isDebugLevel(3)) {
                    String className = getClass().getName();
                    String shortClassName = className;
                    int i = shortClassName.lastIndexOf('.');
                    if(i > 0)
                        shortClassName = shortClassName.substring(i + 1);
                    Utils.println(3, shortClassName + ".dtor(" + className + "@" + System.identityHashCode(this) + ") thread=" + Thread.currentThread().getId());
                }
            } finally {
                super.finalize();
            }
        }
}// class

class SharedPointerTest___ extends SharedPointerTest {
	@QtBlockedSlot
	public final org.qtjambi.qt.widgets.QGraphicsItemInterface createSharedObject2()    {
		org.qtjambi.qt.widgets.QGraphicsItemInterface item = privateCreateSharedObject2();
		if(item.toGraphicsObject()!=null){
			item = item.toGraphicsObject();
		}
		return item;
	}
}// class
