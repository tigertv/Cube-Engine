#include "ResNode.h"


namespace tzw
{
	ResNode::ResNode()
	{

	}

	void ResNode::syncName()
	{

	}

	GamePart* ResNode::getProxy()
	{
		return nullptr;
	}

	void ResNode::handleNameEdit()
	{
		if(getProxy())
		{
			char a[128] = "";
			strcpy(a, getProxy()->getName().c_str());
			bool isInputName = ImGui::InputText(u8"����",a,128);
			if(isInputName)
			{
				getProxy()->setName(a);
				syncName();
			}
		}

	}
}