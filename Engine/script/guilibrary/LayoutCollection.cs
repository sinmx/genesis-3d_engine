﻿/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU
 
http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
using System;
using System.Collections.Generic;
using System.Text;

namespace ScriptGUI
{
    internal class LayoutCollection
    {

        public void Add(Layout layout)
        {
            mLayoutTable.Add(layout.Name, layout);
        }
        public bool GetLayout(FString layout_name, out Layout layout)
        {
            return mLayoutTable.TryGetValue(layout_name, out layout);
        }
        public bool GetLayout(Instance  layout_inst, out Layout layout)
        {
			foreach (KeyValuePair<FString, Layout> ly in mLayoutTable)
            {
                if (ly.Value.Root.Instance == layout_inst)
                {
                    layout = ly.Value;
                    return true;
                }
            }
            layout = null;
            return false;
        }
        public void Remove(FString layout_name)
        {
            mLayoutTable.Remove(layout_name);
        }
        public bool ContainKey(FString key)
        {
            return mLayoutTable.ContainsKey(key);
        }

        public int Count
        {
            get
            {
                return mLayoutTable.Count;
            }
        }

        public void Clear()
        {
            mLayoutTable.Clear();
        }

        public void UnloadAll()
        {
            Layout[] copys = new Layout[mLayoutTable.Count];
            mLayoutTable.Values.CopyTo(copys, 0);
            foreach (Layout l in copys)
            {
                l.Unload();
            }
            mLayoutTable.Clear();
        }

        private Dictionary<FString, Layout> mLayoutTable = new Dictionary<FString, Layout>();
    }
}
