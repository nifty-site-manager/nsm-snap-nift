#include "Getline.h"

std::vector<std::string> inputLines;
std::string homeDir;

void save_session(const std::string& path)
{
	std::ofstream ofs(path);

	for(size_t l=0; l<inputLines.size(); l++)
		ofs << inputLines[l] << std::endl;

	ofs.close();
}

void write_prompt(const std::string& lang, const std::string& pwd, const char& promptCh)
{
	//clears current line in console
	clear_console_line();
	if(lang != "")
	{
		std::cout << c_green << lang << c_white;
		if(pwd != "")
			std::cout << ":";
	}
	if(pwd != "")
		std::cout << c_purple << pwd << c_white;
	if(lang != "" || pwd != "")
		std::cout << promptCh << " ";
}

void write_input_line(const std::string& lang, 
	            const std::string& pwd,
                const char& promptCh,
	            const std::string& str,
	            size_t& sPos,
	            const size_t& usableLength,
				const size_t& linePos,
				const size_t& bLinePos)
{
	write_prompt(lang, pwd, promptCh);

	if(sPos + usableLength >= linePos)
	{
		if(str.size()-sPos <= usableLength)
		{
			std::cout << str.substr(sPos, str.size()-sPos);

			size_t ePos = 0;
			for(size_t i=ePos; i<bLinePos; i++)
				std::cout << "\b";
		}
		else
		{
			std::cout << str.substr(sPos, usableLength);

			size_t ePos = str.size() - sPos - usableLength;
			for(size_t i=ePos; i<bLinePos; i++)
				std::cout << "\b";
		}
	}
	else
	{
		sPos = str.size() - usableLength;
		std::cout << str.substr(sPos, usableLength);

		size_t ePos = 0;
		for(size_t i=ePos; i<bLinePos; i++)
			std::cout << "\b";
	}

	//std::cout << std::flush;
	std::fflush(stdout);
}



#if defined _WIN32 || defined _WIN64
	int getline(const std::string& lang, const bool& addPwd, const char& promptCh, std::string& str, bool trackLines)
	{
		char c;
		std::string pwd;
		size_t promptLength = 0, usableLength;
		int prevConsoleWidth = 100000, currConsoleWidth;
		
		size_t cLine = inputLines.size();
		std::string backupStr;

		size_t bLinePos = 0, linePos = str.size(); //line position of insertion
		size_t sPos = 0;

		if(!addPwd)
		{
			promptLength = lang.size() + 2; 
		}

		while(1)
		{
			currConsoleWidth = console_width();
			if(currConsoleWidth != prevConsoleWidth)
			{
				prevConsoleWidth = currConsoleWidth;
				if(addPwd)
				{
					pwd = get_pwd();
					homeDir = home_dir();
					if(homeDir != "" && homeDir == pwd.substr(0, homeDir.size()))
						pwd = "~" + pwd.substr(homeDir.size(), pwd.size()-homeDir.size());

					const int MIN_USABLE_LENGTH = currConsoleWidth/2;
					size_t maxPWDLength = std::max(0, (int)currConsoleWidth - (int)lang.size() - 3 - 1 - 2 - MIN_USABLE_LENGTH);
					if(pwd.size() > maxPWDLength)
						pwd = ".." + pwd.substr(pwd.size()-maxPWDLength, maxPWDLength);

					promptLength = lang.size() + pwd.size() + 3; 
				}
			}
			usableLength = std::max(0, (int)currConsoleWidth - (int)promptLength - 1);

			write_input_line(lang, pwd, promptCh, str, sPos, usableLength, linePos, bLinePos);

			c = _getch();

			if(c == '\r' || c == '\n' || c == 10) //new line
			{
				write_prompt(lang, pwd, promptCh);
				std::cout << str << std::endl;

				if(trackLines)
					if(!inputLines.size() || str != inputLines[inputLines.size()-1])
						inputLines.push_back(str);

				if(c == 10) // ctrl enter
					return NSM_SENTER;
				else
					return 0;
			}
			else if(c == '\t') //tab completion
			{
				int searchPos = linePos-1,
				    trimPos = -1;
				for(; searchPos >= 0 && 
				      str[searchPos] != '\t' && 
				      str[searchPos] != '\n' && 
				      str[searchPos] != '\'' &&
				      str[searchPos] != '"' &&
				      str[searchPos] != '(' &&
				      str[searchPos] != ','; --searchPos)
				{
					if(searchPos && (str[searchPos] == '/' || str[searchPos] == '\\') && trimPos == -1)
						trimPos = searchPos+1;
				}
				++searchPos;
				if(trimPos == -1)
					trimPos = searchPos;
				Path tabPath;
				std::set<std::string> paths;
				std::string searchStr = str.substr(searchPos, linePos-searchPos);
				strip_leading_whitespace(searchStr);
				if(searchStr != "")
				{
					tabPath.set_file_path_from(searchStr.c_str());
					makeSearchable(tabPath);
					paths = lsSetStar(tabPath, 1);
				}
				if(paths.size())
				{
					auto path = paths.begin();
					std::string foundStr = *path;
					++path;
					for(; path!=paths.end(); ++path)
					{
						int pMax = std::min(foundStr.size(), path->size()),
						    pos = 0;
						for(; pos < pMax; ++pos)
						{
							if(foundStr[pos] != (*path)[pos])
								break;
						}
						foundStr = foundStr.substr(0, pos);
					}

					foundStr = foundStr.substr(linePos-trimPos, foundStr.size()-linePos+trimPos);

					if(foundStr == "")
					{
						std::cout << std::endl;
						coutPaths(tabPath.dir, paths, " ");
						std::cout << std::endl;
					}
					else
					{
						str = str.substr(0, linePos) + foundStr + str.substr(linePos, str.size()-linePos);
					
						for(size_t i=0; i<foundStr.size(); i++)
						{
							++linePos;
							if(sPos + usableLength +1 == linePos)
								++sPos;
						}
					}
				}
				else
					std::cout << "\a" << std::flush;
			}
			else if(c == 0)
			{
				c = _getch();

				if(c == -108) //ctrl+tab
					std::cout << "\a" << std::flush;
			}
			else if(c == 1) //ctrl a
			{
				bLinePos = str.size(); //line position of insertion
				linePos = sPos = 0;
			}
			else if(c == 2) //ctrl b
			{
				if(linePos > 0)
				{
					--linePos;
					++bLinePos;

					if(linePos < sPos)
						sPos = linePos;
				}
			}
			else if(c == 3 || c == 26) //ctrl c & ctrl z
			{
				//clear_console_line();
				std::cout << std::endl << c_red << "--terminated by user--" << c_white << std::endl;

				return NSM_KILL;
			}
			else if(c == 4) //ctrl d
			{
				str = str.substr(0, linePos);
				bLinePos = 0;
			}
			else if(c == 5) //ctrl e
			{
				bLinePos = sPos = 0;
				linePos = str.size();
			}
			else if(c == 6) //ctrl f
			{
				if(bLinePos > 0)
				{
					++linePos;
					--bLinePos;

					if(sPos + usableLength == linePos)
						++sPos;
				}
			}
			else if(c == 8) //backspace
			{
				if(linePos > 0)
				{
					str = str.substr(0, linePos-1) + str.substr(linePos, str.size()-linePos);
					--linePos;

					if(sPos > 0 && str.size() - sPos < usableLength)
						--sPos;
				}
			}
			else if(c == 127) //ctrl backspace
			{
				bool foundNonWhitespace = 0;
				do
				{
					if(linePos > 0)
					{
						if(str[linePos-1] != ' ' && str[linePos-1] != '\t')
							foundNonWhitespace = 1;

						str = str.substr(0, linePos-1) + str.substr(linePos, str.size()-linePos);
						--linePos;

						if(sPos > 0 && str.size() - sPos < usableLength)
							--sPos;
					}
					else
						break;
				}while(!foundNonWhitespace || (linePos > 0 && std::isalnum(str[linePos-1])));
			}
			else if(c == 18) //ctrl+r (same as opt/alt+enter)
			{
				write_prompt(lang, pwd, promptCh);
				std::cout << str << std::endl;

				if(trackLines)
					if(!inputLines.size() || str != inputLines[inputLines.size()-1])
						inputLines.push_back(str);

				return NSM_SENTER;
			}
			else if(c == -32) //check for arrow keys
			{
				c = _getch();

				if(c == -115) //ctrl up arrow
				{
				}
				else if(c == -111) //ctrl down arrow
				{
				}
				else if(c == 116) //ctrl right arrow
				{
					do
					{
						if(bLinePos > 0)
						{
							++linePos;
							--bLinePos;

							if(sPos + usableLength == linePos)
								++sPos;
						}
					}while(bLinePos > 0 && std::isalnum(str[linePos]));
				}
				else if(c == 115) //ctrl left arrow
				{
					do
					{
						if(linePos > 0)
						{
							--linePos;
							++bLinePos;

							if(linePos < sPos)
								sPos = linePos;
						}
					}while(linePos > 0 && std::isalnum(str[linePos]));
				}

				else if(c == 72) //up arrow
				{
					if(cLine > 0)
					{
						if(cLine == inputLines.size())
							backupStr = str;
						--cLine;
						str = inputLines[cLine];
						bLinePos = sPos = 0;
						linePos = str.size();
					}
				}
				else if(c == 80) //down arrow
				{
					if(cLine < inputLines.size())
					{
						++cLine;
						if(cLine == inputLines.size())
							str = backupStr;
						else
							str = inputLines[cLine];
						bLinePos = sPos = 0;
						linePos = str.size();
					}
				}
				else if(c == 77) //right arrow
				{
					if(bLinePos > 0)
					{
						++linePos;
						--bLinePos;

						if(sPos + usableLength == linePos)
							++sPos;
					}
				}
				else if(c == 75) //left arrow
				{
					if(linePos > 0)
					{
						--linePos;
						++bLinePos;

						if(linePos < sPos)
							sPos = linePos;
					}
				}
			}
			else
			{
				str = str.substr(0, linePos) + c + str.substr(linePos, str.size()-linePos);
				++linePos;

				if(sPos + usableLength +1 == linePos)
					++sPos;
			}

			write_input_line(lang, pwd, promptCh, str, sPos, usableLength, linePos, bLinePos);
		}

		return 0;
	}
#else  //*nix
	int getline(const std::string& lang, const bool& addPwd, const char& promptCh, std::string& str, bool trackLines)
	{
		char c;
		std::string pwd;
		size_t promptLength = 0, usableLength;
		int prevConsoleWidth = 100000, currConsoleWidth;
		
		size_t cLine = inputLines.size();
		std::string backupStr;

		size_t bLinePos = 0, linePos = str.size(); //line position of insertion
		size_t sPos = 0;

		if(!addPwd)
		{
			promptLength = lang.size() + 2; 
		}

		enable_raw_mode();
		while(1)
		{
			currConsoleWidth = console_width();
			if(currConsoleWidth != prevConsoleWidth)
			{
				prevConsoleWidth = currConsoleWidth;
				if(addPwd)
				{
					pwd = get_pwd();
					homeDir = home_dir();
					if(homeDir != "" && homeDir == pwd.substr(0, homeDir.size()))
						pwd = "~" + pwd.substr(homeDir.size(), pwd.size()-homeDir.size());

					const int MIN_USABLE_LENGTH = currConsoleWidth/2;
					size_t maxPWDLength = std::max(0, (int)currConsoleWidth - (int)lang.size() - 3 - 1 - 2 - MIN_USABLE_LENGTH);
					if(pwd.size() > maxPWDLength)
						pwd = ".." + pwd.substr(pwd.size()-maxPWDLength, maxPWDLength);

					promptLength = lang.size() + pwd.size() + 3; 
				}
			}
			usableLength = std::max(0, (int)currConsoleWidth - (int)promptLength - 1);

			write_input_line(lang, pwd, promptCh, str, sPos, usableLength, linePos, bLinePos);

			c = getchar();

			if(c == '\r' || c == '\n') //new line
			{
				write_prompt(lang, pwd, promptCh);
				std::cout << str << std::endl;

				if(trackLines)
					if(!inputLines.size() || str != inputLines[inputLines.size()-1])
						inputLines.push_back(str);
				disable_raw_mode(); //system("stty cooked");
				return 0;
			}
			else if(c == '\t') //tab completion
			{
				int searchPos = linePos-1,
				    trimPos = -1;
				for(; searchPos >= 0 && 
				      str[searchPos] != '\t' && 
				      str[searchPos] != '\n' && 
				      str[searchPos] != '\'' &&
				      str[searchPos] != '"' &&
				      str[searchPos] != '(' &&
				      str[searchPos] != ','; --searchPos)
				{
					if(searchPos && (str[searchPos] == '/' || str[searchPos] == '\\') && trimPos == -1)
						trimPos = searchPos+1;
				}
				++searchPos;
				if(trimPos == -1)
					trimPos = searchPos;
				Path tabPath;
				std::set<std::string> paths;
				std::string searchStr = str.substr(searchPos, linePos-searchPos);
				strip_leading_whitespace(searchStr);
				if(searchStr != "")
				{
					tabPath.set_file_path_from(searchStr.c_str());
					makeSearchable(tabPath);
					paths = lsSetStar(tabPath, 1);
				}
				if(paths.size())
				{
					auto path = paths.begin();
					std::string foundStr = *path;
					++path;
					for(; path!=paths.end(); ++path)
					{
						int pMax = std::min(foundStr.size(), path->size()),
						    pos = 0;
						for(; pos < pMax; ++pos)
						{
							if(foundStr[pos] != (*path)[pos])
								break;
						}
						foundStr = foundStr.substr(0, pos);
					}

					foundStr = foundStr.substr(linePos-trimPos, foundStr.size()-linePos+trimPos);

					if(foundStr == "")
					{
						std::cout << std::endl;
						coutPaths(tabPath.dir, paths, " ");
						std::cout << std::endl;
					}
					else
					{
						str = str.substr(0, linePos) + foundStr + str.substr(linePos, str.size()-linePos);
					
						for(size_t i=0; i<foundStr.size(); i++)
						{
							++linePos;
							if(sPos + usableLength +1 == linePos)
								++sPos;
						}
					}
				}
				else
				{
					std::cout << "\a" << std::flush;

					/*for(int i=0; i<4; i++)
					{
						str = str.substr(0, linePos) + " " + str.substr(linePos, str.size()-linePos);
						++linePos;

						if(sPos + usableLength + 1 == linePos)
							++sPos;
					}*/
				}
			}
			else if(c == 1) //ctrl a
			{
				bLinePos = str.size(); //line position of insertion
				linePos = sPos = 0;
			}
			else if(c == 2) //ctrl b
			{
				if(linePos > 0)
				{
					--linePos;
					++bLinePos;

					if(linePos < sPos)
						sPos = linePos;
				}
			}
			else if(c == 4) //ctrl d
			{
				str = str.substr(0, linePos);
				bLinePos = 0;
			}
			else if(c == 5) //ctrl e
			{
				bLinePos = sPos = 0;
				linePos = str.size();
			}
			else if(c == 6) //ctrl f
			{
				if(bLinePos > 0)
				{
					++linePos;
					--bLinePos;

					if(sPos + usableLength == linePos)
						++sPos;
				}
			}
			else if(c == 8 || c == 31) //ctrl backspace or ctrl (shift) -
			{
				bool foundNonWhitespace = 0;
				do
				{
					if(linePos > 0)
					{
						if(str[linePos-1] != ' ' && str[linePos-1] != '\t')
							foundNonWhitespace = 1;

						str = str.substr(0, linePos-1) + str.substr(linePos, str.size()-linePos);
						--linePos;

						if(sPos > 0 && str.size() - sPos < usableLength)
							--sPos;
					}
					else
						break;
				}while(!foundNonWhitespace || (linePos > 0 && std::isalnum(str[linePos-1])));
			}
			else if(c == 127) //backspace
			{
				if(linePos > 0)
				{
					str = str.substr(0, linePos-1) + str.substr(linePos, str.size()-linePos);
					--linePos;

					if(sPos > 0 && str.size() - sPos < usableLength)
						--sPos;
				}
			}
			else if(c == 18) //ctrl+r (same as opt/alt+enter)
			{
				write_prompt(lang, pwd, promptCh);
				std::cout << str << std::endl;

				if(trackLines)
					if(!inputLines.size() || str != inputLines[inputLines.size()-1])
						inputLines.push_back(str);
				disable_raw_mode(); //system("stty cooked");

				return NSM_SENTER;
			}
			else if(c == '\33' || c == 27) //check for arrow keys
			{
				c = getchar();

				if(c == 10) //option/alt + enter
				{
					write_prompt(lang, pwd, promptCh);
					std::cout << str << std::endl;

					if(trackLines)
						if(!inputLines.size() || str != inputLines[inputLines.size()-1])
							inputLines.push_back(str);
					disable_raw_mode(); //system("stty cooked");

					return NSM_SENTER;
				}
				else if(c == 91)
				{
					c = getchar();

					if(c == 49)
					{
						c = getchar();

						if(c == 59)
						{
							c = getchar();

							if(c == 51 || c == 53) //alt or ctrl
							{
								c = getchar();

								if(c == 65) //alt+up and ctrl+up
								{
								}
								else if(c == 66) //alt+down and ctrl+down
								{
								}
								else if(c == 67) //alt+right and ctrl+right
								{
									do
									{
										if(bLinePos > 0)
										{
											++linePos;
											--bLinePos;

											if(sPos + usableLength == linePos)
												++sPos;
										}
									}while(bLinePos > 0 && std::isalnum(str[linePos]));
								}
								else if(c == 68) //alt+left and ctrl+left
								{
									do
									{
										if(linePos > 0)
										{
											--linePos;
											++bLinePos;

											if(linePos < sPos)
												sPos = linePos;
										}
									}while(linePos > 0 && std::isalnum(str[linePos]));
								}
							}
						}
					}
					else if(c == 65) //up arrow
					{
						if(cLine > 0)
						{
							if(cLine == inputLines.size())
								backupStr = str;
							--cLine;
							str = inputLines[cLine];
							bLinePos = sPos = 0;
							linePos = str.size();
						}
					}
					else if(c == 66) //down arrow
					{
						if(cLine < inputLines.size())
						{
							++cLine;
							if(cLine == inputLines.size())
								str = backupStr;
							else
								str = inputLines[cLine];
							bLinePos = sPos = 0;
							linePos = str.size();
						}
					}
					else if(c == 67) //right arrow
					{
						if(bLinePos > 0)
						{
							++linePos;
							--bLinePos;

							if(sPos + usableLength == linePos)
								++sPos;
						}
					}
					else if(c == 68) //left arrow
					{
						if(linePos > 0)
						{
							--linePos;
							++bLinePos;

							if(linePos < sPos)
								sPos = linePos;
						}
					}
					else if(c == 90) //shift tab
						std::cout << "\a" << std::flush;
				}
				#if defined __APPLE__
					else if(c == 91)
					{
						c = getchar();

						if(c == 65) //opt+up
						{}
						else if(c == 66) //opt+down
						{}
					}
					else if(c == 98) //opt+left
					{
						do
						{
							if(linePos > 0)
							{
								--linePos;
								++bLinePos;

								if(linePos < sPos)
									sPos = linePos;
							}
						}while(linePos > 0 && std::isalnum(str[linePos]));
					}
					else if(c == 102) //opt+right
					{
						do
						{
							if(bLinePos > 0)
							{
								++linePos;
								--bLinePos;

								if(sPos + usableLength == linePos)
									++sPos;
							}
						}while(bLinePos > 0 && std::isalnum(str[linePos]));
					}
				#endif
				else if(c == 127) //alt backspace
				{
					bool foundNonWhitespace = 0;
					do
					{
						if(linePos > 0)
						{
							if(str[linePos-1] != ' ' && str[linePos-1] != '\t')
								foundNonWhitespace = 1;

							str = str.substr(0, linePos-1) + str.substr(linePos, str.size()-linePos);
							--linePos;

							if(sPos > 0 && str.size() - sPos < usableLength)
								--sPos;
						}
						else
							break;
					}while(!foundNonWhitespace || (linePos > 0 && std::isalnum(str[linePos-1])));
				}
			}
			else
			{
				str = str.substr(0, linePos) + c + str.substr(linePos, str.size()-linePos);
				++linePos;

				if(sPos + usableLength +1 == linePos)
					++sPos;
			}

			//write_input_line(lang, pwd, promptCh, str, sPos, usableLength, linePos, bLinePos);
		}

		disable_raw_mode();

		return 0;
	}
#endif
