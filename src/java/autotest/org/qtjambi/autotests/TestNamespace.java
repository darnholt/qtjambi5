/****************************************************************************
**
** Copyright (C) 1992-2009 Nokia. All rights reserved.
** Copyright (C) 2009-2015 Peter Droste, Omix Visualization GmbH & Co. KG. All rights reserved.
**
** This file is part of Qt Jambi.
**
** $BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** $END_LICENSE$

**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

package org.qtjambi.autotests;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import org.qtjambi.autotests.generated.NameSpace;

public class TestNamespace extends QApplicationTest {
    @Test public void exists() throws ClassNotFoundException {
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$ObjectA") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$ObjectB") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$ValueA") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$InterfaceA") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$InterfaceAInterface") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$ObjectC") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$ObjectD") != null);
        assertTrue(Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$ValueB") != null);
    }

    @Test public void testInheritance() throws ClassNotFoundException {
        Class<?> clazzC = Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$ObjectC");
        Class<?> clazzA = Class.forName("org.qtjambi.autotests.generated.NameSpace$ObjectA");
        assertEquals(clazzA, clazzC.getSuperclass());

        Class<?> clazzD = Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$ObjectD");
        assertEquals(clazzC, clazzD.getSuperclass());

        Class<?> interfaces[] = clazzD.getInterfaces();
        assertEquals(1, interfaces.length);

        Class<?> interfaceA = Class.forName("org.qtjambi.autotests.generated.NameSpace$NameSpace2$NameSpace3$InterfaceAInterface");
        assertEquals(interfaceA, interfaces[0]);
    }

    @Test public void testCalls() {
    	NameSpace.ObjectA a = new NameSpace.ObjectA();
    	NameSpace.ObjectB b = new NameSpace.ObjectB();
    	NameSpace.ValueA v = new NameSpace.ValueA();

        assertEquals(a, a.aFunc(a));
        assertEquals(b, a.bFunc(b));
        NameSpace.ValueA v2 = a.vFunc(v);
        System.out.println(v.getX());
        System.out.println(v2.getX());
        assertEquals(v, v2);

        assertEquals(a, a.aFuncPrefixed(a));
        assertEquals(b, a.bFuncPrefixed(b));
        assertEquals(v, a.vFuncPrefixed(v));

        {
            NameSpace.NameSpace2.NameSpace3.ObjectD d = new NameSpace.NameSpace2.NameSpace3.ObjectD("fooBar");
            NameSpace.NameSpace2.NameSpace3.ObjectC c = new NameSpace.NameSpace2.NameSpace3.ObjectC("barFoo");

            NameSpace.NameSpace2.NameSpace3.ObjectC temp = c.fooBar(d);
            assertEquals("fooBar", temp.str());
        }
   }

    public static void main(String args[]) {
        org.junit.runner.JUnitCore.main(TestNamespace.class.getName());
    }
}
