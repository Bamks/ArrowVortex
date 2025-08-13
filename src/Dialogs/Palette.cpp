#include <Dialogs/Palette.h>
#include <string>
#include <Core/String.h>
#include <Core/WidgetsLayout.h>
#include <Managers/PaletteMan.h>
#include <Managers/NoteMan.h>
#include <Editor/Selection.h>
#include <Editor/Common.h>
namespace Vortex {
	//Global Label that shows the currently selected palette number.
	WgLabel* myCurrPaletteInfo;
	
	//Global Label that shows the note count of the currently selected palette.
	WgLabel* palSize;
	
	DialogPalette::~DialogPalette()
	{
	}

	DialogPalette::DialogPalette()
	{
		setTitle("Palette");
		myCreateWidgets();
		onChanges(VCM_ALL_CHANGES);
		gPalette->setCurrentPalette(0);

	}

	//Palette Button update function
	//We tell PaletteMan to set the current palette to the new one, and then update the labels that show info.
	int DialogPalette::setCurrSelection(int index) {
		gPalette->setCurrentPalette(index);
		myCurrPaletteInfo->text.set(("Palette: " + std::to_string(gPalette->getCurrentPaletteIndex())).c_str());
		palSize->text.set(("Notes: " + std::to_string(gPalette->getCurrentPaletteSize())).c_str());
		HudInfo("Selected Palette %i", index);
		return index;
	}

	//"Destructive Paste" checkbox update function.
	void DialogPalette::onCheckboxChange() {
		gPalette->toggleDestructivePaste();
	}

	void DialogPalette::myCreateWidgets()
	{
		//"Copy to Palette" Button.
		myLayout.row().col(0).col(150);
		myLayout.add<GuiWidget>();
		WgButton* copyButton = myLayout.add<WgButton>();
		copyButton->text.set("Copy To Palette");
		copyButton->onPress.bind(this, &DialogPalette::myCopyToPalette);
		copyButton->setTooltip("Copies Selected Notes Into the Palette");

		//"Paste From Palette" Button.
		myLayout.col(2).col(150).col(5);
		myLayout.add<GuiWidget>();
		WgButton* pasteButton = myLayout.add<WgButton>();
		pasteButton->text.set("Paste From Palette");
		pasteButton->onPress.bind(this, &DialogPalette::myPasteFromPalette);
		pasteButton->setTooltip("Pastes saved notes at the current cursor position.");
		
		//Adding the Buttons
		myLayout.row();

		for (int i = 0; i <= 9; i++)
		{
			//If the button number is even, we move the row down.
			if (i % 2 == 0) 
			{
				myLayout.row().col(0).col(20);
			}
			
			//If the button number is odd, move to right on the same row.
			else
			{
				myLayout.col(3).col(20);
			}
			
			//Adding the button that selects the palette index.
			myLayout.add<GuiWidget>();
			std::string buttonNumberString = std::to_string(i);
			WgButton* paletteNumberButton = myLayout.add<WgButton>();
			paletteNumberButton->text.set(buttonNumberString.c_str());
			paletteNumberButton->onPress.bind(new Functor::MemberWithArg<int, DialogPalette, int>(this, &DialogPalette::setCurrSelection, i));
			paletteNumberButton->setTooltip("Palette Index");

			//Adding the textbox (WgLineEdit) to the right of each button.
			myLayout.col(0).col(120);
			myLayout.add<GuiWidget>();
			WgLineEdit* paletteLabel = myLayout.add < WgLineEdit>();
			paletteLabel->setEditable(true);
			gPalette->setCurrentPalette(i);
			String* customPaletteLabelString;
			if (gPalette->getUserLabelData() == nullptr) { customPaletteLabelString = new String(""); }
			else customPaletteLabelString = gPalette->getUserLabelData();
			paletteLabel->text.bind(customPaletteLabelString);
			gPalette->setUserLabelData(customPaletteLabelString, i);
			paletteLabel->onChange.bind(this, &DialogPalette::onUserLabelChange);
			
			
			


		}
		//"Destructive Paste" Checkbox.
		myLayout.row().col(0).col(80);
		myLayout.add<GuiWidget>();
		WgCheckbox* checkbox = myLayout.add<WgCheckbox>();
		checkbox->text.set("Destructive Paste");
		checkbox->onChange.bind(this, &DialogPalette::onCheckboxChange);

		//"Palette: " Info Label.
		myLayout.col(120).col(20);
		myLayout.add<GuiWidget>();
		myCurrPaletteInfo = myLayout.add<WgLabel>();
		String* selec = new String(("Palette: " + std::to_string(gPalette->getCurrentPaletteIndex())).c_str());
		myCurrPaletteInfo->text.bind(selec);

		//"Horizontal Mirror" Checkbox.	
		myLayout.row().col(0).col(80);
		myLayout.add<GuiWidget>();
		WgCheckbox* checkMirrorH = myLayout.add<WgCheckbox>();
		checkMirrorH->text.set("Horizontal Mirror");
		checkMirrorH->onChange.bind(this, &DialogPalette::onHMirrorChange);

		//"Notes: " info label.
		myLayout.col(120).col(20);
		myLayout.add<GuiWidget>();
		palSize = myLayout.add<WgLabel>();
		String* noteCount = new String(("Notes: " + std::to_string(gPalette->getCurrentPaletteSize())).c_str());
		palSize->text.bind(noteCount);

		//"Vertical Mirror" Checkbox.
		myLayout.row().col(0).col(80);
		myLayout.add<GuiWidget>();
		WgCheckbox* checkMirrorV = myLayout.add<WgCheckbox>();
		checkMirrorV->text.set("Vertical Mirror");
		checkMirrorV->onChange.bind(this, &DialogPalette::onVMirrorChange);

		//myLayout.col(120).col(20);



		
	}

	//LineEdit.onChange() update function.
	int DialogPalette::onUserLabelChange()
	{
		gPalette->setUserLabelData(gPalette->getUserLabelData(), gPalette->getCurrentPaletteIndex());
		HudInfo("Changed Label");
		return 0;
	}
	
	//"Copy To Palette" update function.
	void DialogPalette::myCopyToPalette()
	{

		if (gPalette->copyToPalette() == 0)
		{
			NoteList notes;
			gPalette->setCurrentPaletteSize(gSelection->getSelectedNotes(notes));
			setCurrSelection(gPalette->getCurrentPaletteIndex());
		
			HudInfo("Copied to Palette %i", gPalette->getCurrentPaletteIndex());
		}
		
	}

	//"Paste From Palette" update function.
	void DialogPalette::myPasteFromPalette()
	{
		if (gPalette->pasteFromPalette() == 0)
		HudInfo("Pasted From Palette %i", gPalette->getCurrentPaletteIndex());

	}

	//"Horizontal Mirror" checkbox update function.
	void DialogPalette::onHMirrorChange() {
		gPalette->toggleMirrorH();
		HudInfo("Toggled Horizontal Mirroring");
	}
	
	//"Vertical Mirror" checkbox update function.
	void DialogPalette::onVMirrorChange() {
		gPalette->toggleMirrorV();
		HudInfo("Toggled Vertical Mirroring");
	}

}; // namespace Vortex