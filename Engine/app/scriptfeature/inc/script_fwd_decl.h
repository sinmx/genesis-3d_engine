/****************************************************************************
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
#ifndef __SCRIPT_FWD_DECL_H__
#define __SCRIPT_FWD_DECL_H__

#include "foundation/util/monotypes.h"
#include "foundation/core/ptr.h"
#include "foundation/util/dictionary.h"
#include "foundation/util/string.h"
#include "foundation/core/scopedptr.h"

namespace App
{
	// pointer define
	/*
	etc 
	typedef GPtr<StateHandler> TStateHandlerPtr
	T + ClassName + Ptr

	typedef GPtr<const StateHandler> TStateHandlerConstPtr
	T + ClassName + ConstPtr
	*/

	// forward declare
	class MonoScript;
	class ScriptInstance;
	class MonoScriptManager;
	class ScriptMessage;

	// typedefs
	typedef GPtr<MonoScript> TMonoScriptPtr;
	typedef GPtr<ScriptInstance> TScriptInstancePtr;
	typedef GPtr<ScriptMessage> TScriptMessagePtr;

	typedef Util::Array<MonoMethod*> TMonoMethodArray;
	typedef Util::Dictionary<Util::String, MonoMethod*> TMonoMethodMap;
	typedef Util::Dictionary<Util::String, MonoClassField*> TMonoClassFieldMap;
	typedef Util::Dictionary<Util::String, MonoMethod*> ScriptMessageHandlerMap;

	typedef Util::Array< TScriptInstancePtr > ScriptInstances;
	typedef Util::Array< ScriptInstances > ScriptInstanceArraies;

}

#endif // - __SCRIPT_FWD_DECL_H__
