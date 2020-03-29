#include "LuaFns.h"

int lua_nsm_linenumber(lua_State* L)
{
	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "nSl", &ar);
	return ar.currentline;
}

void lua_nsm_pusherrmsg(lua_State* L, const std::string& errStr)
{
	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "nSl", &ar);
	lua_pushstring(L, ("error: " + std::to_string(ar.currentline) + ": " + errStr).c_str());
}

int lua_nsm_lang(lua_State* L)
{
	if(lua_gettop(L) != 1)
	{
		lua_nsm_pusherrmsg(L, "nsm_lang(): expected 1 parameter, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	std::string* nsm_lang;

	lua_getglobal(L, "nsm_lang__");
	if(!lua_islightuserdata(L, 2))
	{
		lua_nsm_pusherrmsg(L, "nsm_lang(): variable 'nsm_lang__' should be of type 'lightuserdata'");
		lua_error(L);
		return 0;
	}
	else if(!lua_isstring(L, 1))
	{
		lua_nsm_pusherrmsg(L, "nsm_lang(): parameter is not a string");
		lua_error(L);
		return 0;
	}
	else
	{
		nsm_lang = (std::string*)lua_topointer(L, 2);
		lua_remove(L, 2);

		std::string langStr = lua_tostring(L, 1);
		lua_remove(L, 1);

		int pos = langStr.find_first_of("nflexc", 0);
		if(pos >= 0)
		{
			if(langStr[pos] == 'n')
				*nsm_lang = "n++";
			else if(langStr[pos] == 'f')
				*nsm_lang = "f++";
			else if(langStr[pos] == 'l')
				*nsm_lang = "lua";
			else if(langStr[pos] == 'e' || langStr[pos] == 'x')
				*nsm_lang = "exprtk";
			else if(langStr[pos] == 'c')
				*nsm_lang = "chai";

			lua_pushnumber(L, 1);
			return 1;
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_lang(): do not recognise language " + quote(langStr));
			lua_error(L);
			return 1;
		}
	}
}

int lua_nsm_mode(lua_State* L)
{
	if(lua_gettop(L) != 1)
	{
		lua_nsm_pusherrmsg(L, "nsm_mode(): expected 1 parameter, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	int* nsm_mode;

	lua_getglobal(L, "nsm_mode__");
	if(!lua_islightuserdata(L, 2))
	{
		lua_nsm_pusherrmsg(L, "nsm_mode(): variable 'nsm_mode__' should be of type 'lightuserdata'");
		lua_error(L);
		return 0;
	}
	else if(!lua_isstring(L, 1))
	{
		lua_nsm_pusherrmsg(L, "nsm_mode(): parameter is not a string");
		lua_error(L);
		return 0;
	}
	else
	{
		nsm_mode = (int*)lua_topointer(L, 2);
		lua_remove(L, 2);

		std::string modeStr = lua_tostring(L, 1);
		lua_remove(L, 1);

		int pos = modeStr.find_first_of("si", 0);
		if(pos >= 0)
		{
			if(modeStr[pos] == 's')
				*nsm_mode = MODE_SHELL;
			else if(modeStr[pos] == 'i')
				*nsm_mode = MODE_INTERP;

			lua_pushnumber(L, 1);
			return 1;
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_mode(): do not recognise mode " + quote(modeStr));
			lua_error(L);
			return 0;
		}
	}
}

int lua_nsm_write(lua_State* L)
{
	int no_params = lua_gettop(L);

	if(no_params < 2)
	{
		lua_nsm_pusherrmsg(L, "nsm_write(): expected 2+ parameters, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	std::string txt;
	for(int p=1; p<no_params; p++)
	{
		if(lua_isnumber(L, 2))
		{
			double val = lua_tonumber(L, 2);
			lua_remove(L, 2);

			if(val == NSM_ENDL)
				txt += "\r\n";
			else
				txt += std::to_string(val);
		}
		else if(lua_isstring(L, 2))
		{
			txt += lua_tostring(L, 2);
			lua_remove(L, 2);
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_write(): parameter number " + std::to_string(p) + " is not a string or a number");
			lua_error(L);
			return 0;
		}
	}

	if(lua_isnumber(L, 1))
	{
		double param = lua_tonumber(L, 1);
		lua_remove(L, 1);

		if(param == NSM_OFILE)
		{
			std::string* indentAmount;
			std::string* parsedText;
			bool indent = 1;

			lua_getglobal(L, "nsm_indentAmount__");
			if(!lua_islightuserdata(L, 1))
			{
				lua_nsm_pusherrmsg(L, "nsm_write(): variable 'nsm_indentAmount__' should be of type 'lightuserdata'");
				lua_error(L);
				return 0;
			}
			indentAmount = (std::string*)lua_topointer(L, 1);
			lua_remove(L, 1);

			lua_getglobal(L, "nsm_parsedText__");
			if(!lua_islightuserdata(L, 1))
			{
				lua_nsm_pusherrmsg(L, "nsm_write(): variable 'nsm_parsedText__' should be of type 'lightuserdata'");
				lua_error(L);
				return 0;
			}
			parsedText = (std::string*)lua_topointer(L, 1);
			lua_remove(L, 1);

			std::istringstream iss(txt);
			std::string ssLine, oldLine;
			int ssLineNo = 0;

			while(!iss.eof())
			{
				getline(iss, ssLine);
				if(0 < ssLineNo++)
					*parsedText += "\n" + *indentAmount;
				oldLine = ssLine;
				*parsedText += ssLine;
			}
			if(indent)
				*indentAmount += into_whitespace(oldLine);

			lua_pushnumber(L, 1);
			return 1;
		}
		else if(param == NSM_CONS)
		{
			bool* consoleLocked;
			std::mutex* os_mtx;

			lua_getglobal(L, "nsm_consoleLocked__");
			if(!lua_islightuserdata(L, 1))
			{
				lua_nsm_pusherrmsg(L, "nsm_write(): variable 'nsm_consoleLocked__' should be of type 'lightuserdata'");
				lua_error(L);
				return 0;
			}
			consoleLocked = (bool*)lua_topointer(L, 1);
			lua_remove(L, 1);

			lua_getglobal(L, "nsm_os_mtx__");
			if(!lua_islightuserdata(L, 1))
			{
				lua_nsm_pusherrmsg(L, "nsm_write(): variable 'nsm_os_mtx__' should be of type 'lightuserdata'");
				lua_error(L);
				return 0;
			}
			os_mtx = (std::mutex*)lua_topointer(L, 1);
			lua_remove(L, 1);

			if(!*consoleLocked)
				os_mtx->lock();
			std::cout << txt;
			if(!*consoleLocked)
				os_mtx->unlock();

			lua_pushnumber(L, 1);
			return 1;
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_write(): not defined for first parameter of type int, value given = " + std::to_string(param));
			lua_error(L);
			return 0;
		}
	}
	else if(lua_isstring(L, 1))
	{
		Variables* vars;
		std::string varName = lua_tostring(L, 1);
		lua_remove(L, 1);

		lua_getglobal(L, "nsm_vars__");
		if(!lua_islightuserdata(L, 1))
		{
			lua_nsm_pusherrmsg(L, "nsm_write(): variable 'nsm_vars__' should be of type 'lightuserdata'");
			lua_error(L);
			return 0;
		}
		vars = (Variables*)lua_topointer(L, 1);
		lua_remove(L, 1);

		VPos vpos;
		if(vars->find(varName, vpos))
		{
			if(vpos.type == "fstream")
			{
				vars->layers[vpos.layer].fstreams[varName] << txt;
				lua_pushnumber(L, 1);
				return 1;
			}
			else if(vpos.type == "ofstream")
			{
				vars->layers[vpos.layer].ofstreams[varName] << txt;
				lua_pushnumber(L, 1);
				return 1;
			}
			else
			{
				//write not defined for varName of type vpos.type
				lua_nsm_pusherrmsg(L, "nsm_write(): not defined for first parameter of type " + quote(vpos.type));
				lua_error(L);
				return 0;
			}
		}
		else
		{
			//no variable named varName
			lua_nsm_pusherrmsg(L, "nsm_write(): " + quote(varName) + " not defined");
			lua_error(L);
			return 0;
		}
	}
	else if(lua_islightuserdata(L, 1))
	{
		std::ofstream* ofs = (std::ofstream*)lua_topointer(L, 1);
		lua_remove(L, 1);

		*ofs << txt;

		lua_pushnumber(L, 1);
		return 1;
	}

	return 0;
}

int lua_nsm_tonumber(lua_State* L)
{
	if(lua_gettop(L) != 1)
	{
		lua_nsm_pusherrmsg(L, "nsm_tonumber(): expected 1 parameter, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	if(lua_islightuserdata(L, 1))
	{
		double* var = (double*)lua_topointer(L, 1);

		lua_remove(L, 1);
		lua_pushnumber(L, *var);
		return 1;
	}
	else if(lua_isstring(L, 1))
	{
		lua_getglobal(L, "nsm_vars__");

		if(!lua_islightuserdata(L, 2))
		{
			lua_nsm_pusherrmsg(L, "nsm_tonumber(): variable 'nsm_vars__' should be of type 'lightuserdata'");
			lua_error(L);
			return 0;
		}

		std::string varName = lua_tostring(L, 1);
		lua_remove(L, 1);
		Variables* vars = (Variables*)lua_topointer(L, 1);
		lua_remove(L, 1);
		VPos vpos;
		double value = 0;

		if(vars->find(varName, vpos))
		{
			if(vars->get_double_from_var(vpos, value))
			{
				lua_pushnumber(L, value);
				return 1;
			}
			else
			{
				lua_nsm_pusherrmsg(L, "nsm_tonumber(): " + quote(varName) + ": failed to get number");
				lua_error(L);
				return 0;
			}
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_tonumber(): " + quote(varName) + ": variable not defined at this scope");
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_nsm_pusherrmsg(L, "nsm_tonumber(): first parameter should either be of type 'lightuserdata' pointing to a variable or of type 'string' giving a variable name");
		lua_error(L);
		return 0;
	}
}

int lua_nsm_tostring(lua_State* L)
{
	if(lua_gettop(L) != 1)
	{
		lua_nsm_pusherrmsg(L, "nsm_tostring(): expected 1 parameter, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	if(lua_islightuserdata(L, 1))
	{
		std::string* var = (std::string*)lua_topointer(L, 1);

		lua_remove(L, 1);
		lua_pushstring(L, var->c_str());
		return 1;
	}
	else if(lua_isstring(L, 1))
	{
		lua_getglobal(L, "nsm_vars__");

		if(!lua_islightuserdata(L, 2))
		{
			lua_nsm_pusherrmsg(L, "nsm_tostring(): variable 'nsm_vars__' should be of type 'lightuserdata'");
			lua_error(L);
			return 0;
		}

		std::string varName = lua_tostring(L, 1);
		lua_remove(L, 1);
		Variables* vars = (Variables*)lua_topointer(L, 1);
		lua_remove(L, 1);
		VPos vpos;
		std::string value = "";
		if(vars->find(varName, vpos))
		{
			if(vars->get_str_from_var(vpos, value, 1, 1))
			{
				lua_pushstring(L, value.c_str());
				return 1;
			}
			else
			{
				lua_nsm_pusherrmsg(L, "nsm_tostring(): " + quote(varName) + ": failed to get string");
				lua_error(L);
				return 0;
			}
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_tostring(): " + quote(varName) + ": variable not defined at this scope");
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_nsm_pusherrmsg(L, "nsm_tostring(): first parameter should either be of type 'lightuserdata' pointing to a variable or of type 'string' giving a variable name");
		lua_error(L);
		return 0;
	}
}

int lua_nsm_tolightuserdata(lua_State* L)
{
	if(lua_gettop(L) != 1)
	{
		lua_nsm_pusherrmsg(L, "nsm_vartolightuserdata(): expected 1 parameter, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	lua_getglobal(L, "nsm_vars__");

	if(!lua_isstring(L, 1))
	{
		lua_nsm_pusherrmsg(L, "nsm_vartolightuserdata(): first parameter for variable name should be of type 'string'");
		lua_error(L);
		return 0;
	}
	else if(!lua_islightuserdata(L, 2))
	{
		lua_nsm_pusherrmsg(L, "nsm_vartolightuserdata(): variable 'nsm_vars__' should be of type 'lightuserdata'");
		lua_error(L);
		return 0;
	}

	std::string varName = lua_tostring(L, 1);
	lua_remove(L, 1);

	Variables* vars = (Variables*)lua_topointer(L, 1);
	lua_remove(L, 1);

	VPos vpos;
	if(vars->find(varName, vpos))
	{
		if(vpos.type.substr(0, 5) == "std::")
		{
		    if(vpos.type == "std::bool")
				lua_pushlightuserdata(L, &vars->layers[vpos.layer].bools[varName]);
		    else if(vpos.type == "std::int")
		    	lua_pushlightuserdata(L, &vars->layers[vpos.layer].ints[varName]);
		    else if(vpos.type == "std::double")
		    	lua_pushlightuserdata(L, &vars->layers[vpos.layer].doubles[varName]);
		    else if(vpos.type == "std::char")
		    	lua_pushlightuserdata(L, &vars->layers[vpos.layer].chars[varName]);
		    else if(vpos.type == "std::string")
				lua_pushlightuserdata(L, &vars->layers[vpos.layer].strings[varName]);
		    else if(vpos.type == "std::long long int")
		    	lua_pushlightuserdata(L, &vars->layers[vpos.layer].llints[varName]);
		}
		else if(vpos.type == "bool" || vpos.type == "int" || vpos.type == "double")
			lua_pushlightuserdata(L, &vars->layers[vpos.layer].doubles[varName]);
		else if(vpos.type == "char" || vpos.type == "string")
			lua_pushlightuserdata(L, &vars->layers[vpos.layer].strings[varName]);
		else if(vpos.type == "ofstream")
			lua_pushlightuserdata(L, &vars->layers[vpos.layer].ofstreams[varName]);
		else
		{
		    lua_nsm_pusherrmsg(L, "nsm_vartolightuserdata(): function not defined for variable " + quote(varName) + " of type " + quote(vpos.type));
			lua_error(L);
			return 0;
		}
	}
	else
	{
	    lua_nsm_pusherrmsg(L, "nsm_vartolightuserdata(): " + quote(varName) + ": variable not defined at this scope");
		lua_error(L);
		return 0;
	}

	// return the number of results 
	return 1;
}

int lua_nsm_setnumber(lua_State* L)
{
	if(lua_gettop(L) != 2)
	{
		lua_nsm_pusherrmsg(L, "nsm_setnumber(): expected 2 parameters, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	lua_getglobal(L, "nsm_vars__");

	if(!lua_isnumber(L, 2))
	{
		lua_nsm_pusherrmsg(L, "nsm_setnumber(): second parameter for value should be of type 'number'");
		lua_error(L);
		return 0;
	}
	else if(!lua_islightuserdata(L, 3))
	{
		lua_nsm_pusherrmsg(L, "nsm_setnumber(): variable 'nsm_vars__' should be of type 'lightuserdata'");
		lua_error(L);
		return 0;
	}

	if(lua_islightuserdata(L, 1))
	{
		double* var = (double*)lua_topointer(L, 1);
		lua_remove(L, 1);
		double value = lua_tonumber(L, 1);
		lua_remove(L, 1);

		*var = value;

		// return the number of results 
		lua_pushnumber(L, 1);
		return 1;
	}
	else if(lua_isstring(L, 1))
	{
		std::string varName = lua_tostring(L, 1);
		lua_remove(L, 1);
		double value = lua_tonumber(L, 1);
		lua_remove(L, 1);
		Variables* vars = (Variables*)lua_topointer(L, 1);
		lua_remove(L, 1);

		VPos vpos;
		if(vars->find(varName, vpos))
		{
			if(vars->set_var_from_double(vpos, value))
				return 0;
			else
			{
				lua_nsm_pusherrmsg(L, "nsm_setnumber(): " + quote(varName) + ": variable assignment failed");
				lua_error(L);
				return 0;
			}
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_setnumber(): " + quote(varName) + ": variable not defined at this scope");
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_nsm_pusherrmsg(L, "nsm_setnumber(): first parameter should either be of type 'lightuserdata' pointing to a variable or of type 'string' giving a variable name");
		lua_error(L);

		return 0;
	}
}

int lua_nsm_setstring(lua_State* L)
{
	if(lua_gettop(L) != 2)
	{
		lua_nsm_pusherrmsg(L, "nsm_setstring(): expected 2 parameters, got " + std::to_string(lua_gettop(L)));
		lua_error(L);
		return 0;
	}

	lua_getglobal(L, "nsm_vars__");

	if(!lua_isstring(L, 2))
	{
		lua_nsm_pusherrmsg(L, "nsm_setstring(): second parameter for value should be of type 'string'");
		lua_error(L);
		return 0;
	}
	else if(!lua_islightuserdata(L, 3))
	{
		lua_nsm_pusherrmsg(L, "nsm_setstring(): variable 'nsm_vars__' should be of type 'lightuserdata'");
		lua_error(L);
		return 0;
	}

	if(lua_islightuserdata(L, 1))
	{
		std::string* var = (std::string*)lua_topointer(L, 1);
		lua_remove(L, 1);
		std::string value = lua_tostring(L, 1);
		lua_remove(L, 1);

		*var = value;

		// return the number of results 
		lua_pushnumber(L, 1);
		return 1;
	}
	else if(lua_isstring(L, 1))
	{
		std::string varName = lua_tostring(L, 1);
		lua_remove(L, 1);
		std::string value = lua_tostring(L, 1);
		lua_remove(L, 1);
		Variables* vars = (Variables*)lua_topointer(L, 1);
		lua_remove(L, 1);

		VPos vpos;
		if(vars->find(varName, vpos))
		{
			if(vars->set_var_from_str(vpos, value))
				return 0;
			else
			{
				lua_nsm_pusherrmsg(L, "nsm_setstring(): " + quote(varName) + ": variable assignment failed");
				lua_error(L);
				return 0;
			}
		}
		else
		{
			lua_nsm_pusherrmsg(L, "nsm_setstring(): " + quote(varName) + ": variable not defined at this scope");
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_nsm_pusherrmsg(L, "nsm_setstring(): first parameter should either be of type 'lightuserdata' pointing to a variable or of type 'string' giving a variable name");
		lua_error(L);

		return 0;
	}
}
	
