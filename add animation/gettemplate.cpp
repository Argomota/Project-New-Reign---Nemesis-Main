#include "gettemplate.h"

#pragma warning(disable:4503)

using namespace std;

getTemplate::getTemplate()
{
	string templateDirectory = "behavior templates\\";
	string newpath;
	vecstr codelist;
	set<string> corelist;
	read_directory(templateDirectory, codelist);

	for (auto& code : codelist)
	{
		if (boost::to_lower_copy(code) != code)
		{
			ErrorMessage(1191, code, boost::to_lower_copy(code));
		}

		if (code.find(".", 0) == NOT_FOUND)
		{
			newpath = templateDirectory + code;
			boost::filesystem::path FOF(newpath);

			if (code == "t" || code == "aaprefix" || code == "aaset" || code == "md" || code == "rd" || code == "+")
			{
				ErrorMessage(3009, code);
			}

			if (boost::filesystem::is_directory(FOF))
			{
				vecstr folderlist;
				read_directory(newpath, folderlist);
				bool isCore = false;
				bool isOptionExist = false;
				bool registered = false;
				unordered_map<string, unordered_map<int, bool>> isStateJoint;		// behavior, node(function) ID, true/false; is this node(function) joining the animation template with the main branch?
				vector<boost::filesystem::path> pathVector;

				for (auto& folder : folderlist)
				{
					newpath = templateDirectory + code + "\\" + folder;
					boost::filesystem::path FOF2(newpath);

					if (boost::iequals(folder, "option_list.txt") && !boost::filesystem::is_directory(FOF2))
					{
						OptionList option(newpath, code);
						option.setDebug(debug);
						optionlist[code] = option;
						templatelist[code] = true;
						isOptionExist = true;

						if (option.core)
						{
							isCore = true;
							string corefile = boost::to_lower_copy(option.coreBehavior);
							corelist.insert(corefile);
							coreBehaviorCode[corefile] = code;
						}
					}

					pathVector.push_back(FOF2);
				}

				for (unsigned int l = 0; l < pathVector.size(); ++l)
				{
					if (boost::filesystem::is_directory(pathVector[l]))
					{
						vecstr filelist;
						newpath = pathVector[l].string();
						read_directory(newpath, filelist);
						string behaviorFolder = pathVector[l].stem().string();
						string lowerBehaviorFolder = boost::algorithm::to_lower_copy(behaviorFolder);
						bool noGroup = true;

						if (isCore)
						{
							if (behaviorJoints[lowerBehaviorFolder].size() == 0)
							{
								ErrorMessage(1182, code, templateDirectory + code + "\\option_list.txt");
							}

							behaviorJoints[boost::to_lower_copy(optionlist[code].coreBehavior)] = behaviorJoints[lowerBehaviorFolder];
						}

						for (unsigned int i = 0; i < filelist.size(); ++i)
						{
							newpath = templateDirectory + code + "\\" + behaviorFolder + "\\" + filelist[i];
							boost::filesystem::path file(newpath);
							string lowerfilename = boost::to_lower_copy(filelist[i]);

							if (!boost::filesystem::is_directory(file))
							{
								if (boost::iequals(lowerfilename, code + "_group.txt"))
								{
									if (behaviortemplate[code + "_group"][lowerBehaviorFolder].size() == 0)
									{
										noGroup = false;

										if (!GetFunctionLines(newpath, behaviortemplate[code + "_group"][lowerBehaviorFolder]))
										{
											return;
										}
									}
									else
									{
										ErrorMessage(1019, newpath);
									}
								}
								else if (boost::iequals(lowerfilename, code + "_master.txt"))
								{
									if (behaviortemplate[code + "_master"][lowerBehaviorFolder].size() == 0)
									{
										noGroup = false;

										if (!GetFunctionLines(newpath, behaviortemplate[code + "_master"][lowerBehaviorFolder]))
										{
											return;
										}
									}
									else
									{
										ErrorMessage(1019, newpath);
									}
								}
								else if (boost::iequals(lowerfilename, code + ".txt"))
								{
									registered = true;
									grouplist[lowerBehaviorFolder].insert(code);

									if (behaviortemplate[code][lowerBehaviorFolder].size() == 0)
									{
										if (!GetFunctionLines(newpath, behaviortemplate[code][lowerBehaviorFolder]))
										{
											return;
										}
									}
									else
									{
										ErrorMessage(1019, newpath);
									}
								}
								else if (lowerfilename[0] == '#')
								{
									string number = boost::regex_replace(string(lowerfilename), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));

									if (lowerfilename == "#" + number + ".txt" && isOnlyNumber(number))
									{
										existingFunctionID[code][lowerBehaviorFolder].push_back(stoi(number));
									}

									vecstr storeline;

									if (!GetFunctionLines(newpath, storeline))
									{
										return;
									}

									bool isJoint = false;
									bool isStateMachine = false;

									for (unsigned int j = 0; j < storeline.size(); ++j)
									{
										if (storeline[j].find("class=\"hkbStateMachine\" signature=\"") != NOT_FOUND)
										{
											isStateMachine = true;
										}

										if (isStateMachine && storeline[j].find("<!-- NEW ^" + code) != NOT_FOUND)
										{
											if (!isJoint)
											{
												isJoint = true;
											}
										}
										else if (isStateMachine && storeline[j].find("<!-- CLOSE -->") != NOT_FOUND)
										{
											if (isJoint)
											{
												isJoint = false;
											}
										}

										if (isStateMachine && storeline[j].find("#" + code) != NOT_FOUND)
										{
											if (isJoint)
											{
												isStateJoint[lowerBehaviorFolder][stoi(number)] = true;
												break;
											}
										}
									}
								}
							}
							else if (lowerBehaviorFolder == "animationdatasinglefile")
							{
								if (boost::filesystem::is_directory(newpath))
								{
									vecstr headerlist;
									read_directory(newpath, headerlist);
									grouplist[lowerBehaviorFolder].insert(code);
									string project = filelist[i] + ".txt";

									for (unsigned int j = 0; j < headerlist.size(); ++j)
									{
										string header = headerlist[j].substr(0, headerlist[j].find_last_of("."));

										if (header[0] == '$' && (header.back() == '$' || header.find_last_of("$UC") == header.length() - 1))
										{
											if (animdatatemplate[code][project][header].size() == 0)
											{
												if (!GetFunctionLines(newpath + "\\" + headerlist[j], animdatatemplate[code][project][header], true))
												{
													return;
												}
											}
											else
											{
												ErrorMessage(1019, newpath);
											}
										}
										else
										{
											existingAnimDataHeader[code][project].insert(header);
										}
									}
								}
							}
							else if (lowerBehaviorFolder == "animationsetdatasinglefile")
							{
								if (boost::filesystem::is_directory(newpath) && filelist[i].find("~") != NOT_FOUND)
								{
									vecstr headerlist;
									read_directory(newpath, headerlist);
									grouplist[lowerBehaviorFolder].insert(code);
									string project = filelist[i] + ".txt";

									while (project.find("~") != NOT_FOUND)
									{
										project.replace(project.find("~"), 1, "\\");
									}

									for (auto& curheader : headerlist)
									{
										boost::filesystem::path thisfile(newpath + "\\" + curheader);

										if (!boost::filesystem::is_directory(thisfile) && thisfile.extension().string() == ".txt")
										{
											string header = thisfile.stem().string();

											if (header[0] == '$' && header.back() == '$')
											{
												if (asdtemplate[code][project][header].size() == 0)
												{
													if (!GetFunctionLines(thisfile.string(), asdtemplate[code][project][header], false))
													{
														return;
													}
												}
												else
												{
													ErrorMessage(1019, newpath);
												}
											}
											else
											{
												existingASDHeader[code][project].insert(header + ".txt");
											}
										}
									}
								}
							}
						}

						if (optionlist[code].multiState[lowerBehaviorFolder].size() > 1)
						{
							if (isStateJoint[lowerBehaviorFolder].size() == 0)
							{
								ErrorMessage(1074, templateDirectory + code);
							}
							else if (isStateJoint[lowerBehaviorFolder].size() != optionlist[code].multiState[lowerBehaviorFolder].size())
							{
								ErrorMessage(1073, templateDirectory + code);
							}

							for (auto it = optionlist[code].multiState[lowerBehaviorFolder].begin(); it != optionlist[code].multiState[lowerBehaviorFolder].end(); ++it)
							{
								if (isStateJoint[lowerBehaviorFolder][it->second])
								{
									mainBehaviorJoint[code][lowerBehaviorFolder][it->first] = it->second;
								}
								else
								{
									ErrorMessage(1075, templateDirectory + code);
								}
							}
						}
						else if (lowerBehaviorFolder != "animationdatasinglefile" && lowerBehaviorFolder != "animationsetdatasinglefile")
						{
							if (optionlist[code].multiState[lowerBehaviorFolder].size() == 1)
							{
								WarningMessage(1008, templateDirectory + code + "\\option_list.txt");
							}

							if (isStateJoint[lowerBehaviorFolder].size() > 1)
							{
								ErrorMessage(1072, templateDirectory + code);
							}
							else if (isStateJoint[lowerBehaviorFolder].size() == 0)
							{
								ErrorMessage(1074, templateDirectory + code);
							}

							mainBehaviorJoint[code][lowerBehaviorFolder][0] = isStateJoint[lowerBehaviorFolder].begin()->first;
						}

						// Error checking
						if (noGroup)
						{
							if (!optionlist[code].ignoreGroup)
							{
								if (optionlist[code].groupMin != -1 || optionlist[code].ruleOne.size() != 0 || optionlist[code].ruleTwo.size() != 0)
								{
									ErrorMessage(1061, code, templateDirectory + behaviorFolder + "\\" + code);
								}

								if (behaviortemplate[code + "_master"].size() != 0)
								{
									ErrorMessage(1085, templateDirectory + behaviorFolder + "\\" + code);
								}
							}
						}
						else
						{
							if (optionlist[code].ignoreGroup)
							{
								ErrorMessage(1079, code, templateDirectory + code + "\\option_list.txt");
							}
						}

						filelist.clear();
					}
				}

				if (!isOptionExist && registered)
				{
					ErrorMessage(1021, newpath);
				}
				
				folderlist.clear();
			}
		}
	}

	for (auto& animlist : grouplist)
	{
		// match behavior file, is the behavior file a core behavior?
		if (corelist.find(animlist.first) != corelist.end())
		{
			for (auto& animcode : animlist.second)
			{
				coreTemplate[animcode] = animlist.first;
			}
		}
	}
}
