#include "addanims.h"
#include "generator_utility.h"

using namespace std;

bool AddAnims(string& line, string animPath, string outputdir,string behaviorFile, string lowerBehaviorFile, string& newMod, vecstr& storeline, int& counter,
	unordered_map<string, bool>& isAdded, bool& addAnim)
{
	string animFile = GetFileName(animPath);

	if (!isAdded[animPath])
	{
		if (activatedBehavior["gender"])
		{
			if (lowerBehaviorFile == "defaultfemale")
			{
				if (!boost::iequals(animPath, "Animations\\female\\" + animFile))
				{
					boost::filesystem::path animation(GetFileDirectory(outputdir));

					if (isFileExist(animation.parent_path().parent_path().string() + "\\Animations\\female\\" + animFile))
					{
						animPath = "Animations\\female\\" + animFile;
					}
					else if (boost::iequals(animPath, "Animations\\male\\" + animFile))
					{
						if (isFileExist(animation.parent_path().parent_path().string() + "\\Animations\\" + animFile))
						{
							animPath = "Animations\\" + animFile;
						}
					}
				}
			}
			else if (lowerBehaviorFile == "defaultmale")
			{
				if (!boost::iequals(animPath, "Animations\\male\\" + animFile))
				{
					boost::filesystem::path animation(GetFileDirectory(outputdir));

					if (isFileExist(animation.parent_path().parent_path().string() + "\\Animations\\male\\" + animFile))
					{
						animPath = "Animations\\male\\" + animFile;
					}
					else if (boost::iequals(animPath, "Animations\\female\\" + animFile))
					{
						if (isFileExist(animation.parent_path().parent_path().string() + "\\Animations\\" + animFile))
						{
							animPath = "Animations\\" + animFile;
						}
					}
				}
			}
		}

		storeline.push_back("				<hkcstring>" + animPath + "</hkcstring>");
		boost::to_lower(animPath);
		boost::to_lower(animFile);
		isAdded[animPath] = true;
		newMod = animPath.substr(11, animPath.find("\\", 11) - 11);
		vector<set<string>>* match_ptr = &animModMatch[lowerBehaviorFile][animFile];
		size_t matchSize = match_ptr->size();
		registeredAnim[lowerBehaviorFile][animFile] = true;
		addAnim = true;

		if (matchSize == 0)
		{
			match_ptr->push_back(set<string>{animPath});
			match_ptr->push_back(set<string>{newMod});
		}
		else if (matchSize == 2)
		{
			match_ptr->at(0).insert(animPath);
			match_ptr->at(1).insert(newMod);
		}
		else
		{
			ErrorMessage(1058);
		}

		++counter;
	}

	return true;
}
