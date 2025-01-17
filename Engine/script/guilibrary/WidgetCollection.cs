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
    internal class WidgetCollection
    {
        public void Add(Widget widget)
        {
            mWidgetTable.Add(widget.Name, widget);
        }
        public bool GetWidget(FString widget_name, out Widget widget)
        {
            return mWidgetTable.TryGetValue(widget_name, out widget);
        }
        public void Remove(FString layout_name)
        {
            mWidgetTable.Remove(layout_name);
        }
        public bool Contains(FString widget_name)
        {
            return mWidgetTable.ContainsKey(widget_name);
        }

        public void Clear()
        {
            mWidgetTable.Clear();
        }

        public bool TryGetWidget(Instance widget_inst, out Widget widget)
        {
			foreach (KeyValuePair<FString, Widget> item in mWidgetTable)
            {
                if (item.Value.Instance == widget_inst)
                {
                    widget = item.Value;
                    return true;
                }
            }
            widget = null;
            return false;
        }

        public int Count
        {
            get
            {
                return mWidgetTable.Count;
            }
        }

        public void UnloadAll()
        {
            Widget[] copys = new Widget[mWidgetTable.Count];
            mWidgetTable.Values.CopyTo(copys, 0);
            foreach (Widget w in copys)
            {
                w.Dispose();
            }
            mWidgetTable.Clear();
        }

        private Dictionary<FString, Widget> mWidgetTable = new Dictionary<FString, Widget>();
    }
}
