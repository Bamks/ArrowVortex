#include <Dialogs/Palette.h>
#include <string>
#include <Core/String.h>
#include <Core/WidgetsLayout.h>
#include <Managers/PaletteMan.h>
#include <Editor/Common.h>
namespace Vortex {

	int myCurrSelection = 0;
	std::string s_myCurrSelection = "Palette: " + std::to_string(myCurrSelection);
	WgLabel* l;

	DialogPalette::~DialogPalette()
	{
	}

	DialogPalette::DialogPalette()
	{
		setTitle("Palette");
		myCreateWidgets();
		onChanges(VCM_ALL_CHANGES);
	}

	int DialogPalette::setCurrSelection(int index) {
		gPalette->setCurrentPalette(index);
		myCurrSelection = index;
		updateSelectionLabel();
		HudInfo("Selected Palette %i", index);
		return index;
	}

	void DialogPalette::onCheckboxChange() {
		gPalette->toggleDestructivePaste();
	}

	void DialogPalette::myCreateWidgets()
	{
		myLayout.row().col(0).col(120);

		myLayout.add<GuiWidget>();
		WgButton* copyButton = myLayout.add<WgButton>();
		copyButton->text.set("Copy To Palette");
		copyButton->onPress.bind(this, &DialogPalette::myCopyToPalette);
		copyButton->setTooltip("Copies Selected Notes Into the Palette");


		myLayout.col(0).col(150);

		myLayout.add<GuiWidget>();
		WgButton* pasteButton = myLayout.add<WgButton>();
		pasteButton->text.set("Paste From Palette");
		pasteButton->onPress.bind(this, &DialogPalette::myPasteFromPalette);
		pasteButton->setTooltip("Pastes saved notes at the current cursor position.");

		myLayout.row();

		for (int i = 0; i <= 8; i++)
		{
			myLayout.col(0).col(24);

			myLayout.add<GuiWidget>();
			std::string s = std::to_string(i);
			WgButton* p = myLayout.add<WgButton>();
			p->text.set(s.c_str());
			p->onPress.bind(new Functor::MemberWithArg<int, DialogPalette, int>(this, &DialogPalette::setCurrSelection, i));
			p->setTooltip("Palette Index");
		}

		myLayout.row().col(0).col(80);

		myLayout.add<GuiWidget>();
		WgCheckbox* checkbox = myLayout.add<WgCheckbox>();
		checkbox->text.set("Destructive Paste");
		checkbox->onChange.bind(this, &DialogPalette::onCheckboxChange);



		myLayout.col(120).col(20);

		myLayout.add<GuiWidget>();
		l = myLayout.add<WgLabel>();
		String* selec = new String(s_myCurrSelection.c_str());
		l->text.bind(selec);


	}

	void DialogPalette::myCopyToPalette()
	{

		if (gPalette->copyToPalette() == 0)
			HudInfo("Copied to Palette %i", myCurrSelection);

	}

	void DialogPalette::myPasteFromPalette()
	{
		if (gPalette->pasteFromPalette() == 0)
			HudInfo("Pasted From Palette %i", myCurrSelection);

	}

	void DialogPalette::updateSelectionLabel()
	{
		std::string s = "Palette: " + std::to_string(myCurrSelection);
		l->text.set(s.c_str());
	}



}; // namespace Vortex