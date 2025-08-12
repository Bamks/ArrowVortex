#include <System/System.h>
#include <Editor/Selection.h>
#include <Managers/PaletteMan.h>
namespace Vortex {
	//Maximum size of the palette.
	const int MAX_PALETTE_ENTRIES = 20;



	struct PaletteManImpl : public PaletteMan
	{
		//Palette Array where the clipboard Strings are stored. One entry is a typical user clipboard String (Select Notes -> Ctrl+c).
		String* my_s[MAX_PALETTE_ENTRIES];

		//Size of the palette;
		int myPaletteSize = MAX_PALETTE_ENTRIES;

		//The current entry in the palette that PaletteMan functions perform operations on.
		int myCurrSelection;

		//If this is false, pasting from palette clears the area affected by the paste before placing the notes (aka destructive paste). If this is true, it simply adds on to the existing notes (non destructive paste).
		bool myShouldInsertNotes;

		~PaletteManImpl() {};
		PaletteManImpl() {
			for (int i = 0; i < MAX_PALETTE_ENTRIES; i++)
			{
				my_s[i] = nullptr;
			}
			myCurrSelection = 0;
			myShouldInsertNotes = true;

		}

		//Copies the provided String and sets the palette (from the palette array) at the specified index to that String.
		int addToPalette(String entry, int index)
		{
			if (index > MAX_PALETTE_ENTRIES - 1)
				return 1;
			if (my_s[index] != nullptr)
			{
				delete (my_s[index]);
			}
			my_s[index] = new String(entry);

			return 0;
		}

		//Retrives the string at a given location in the palette array.
		String getFromPalette(int index)
		{
			if (index < MAX_PALETTE_ENTRIES)
				return *my_s[index];
			else
			{
				HudWarning("Invalid Palette Index");
				return nullptr;
			}
		}

		//Retrives the saved clipboard string at the current selection, sets the User clipboard to that String, and pastes that data at the current Cursor location.
		int pasteFromPalette()
		{
			if (my_s[myCurrSelection] == nullptr)
			{
				HudWarning("Cannot paste from empty palette");
				return 1;
			}
			gSystem->setClipboardText(getFromPalette(myCurrSelection));
			gNotes->pasteFromClipboard(myShouldInsertNotes);
			return 0;
		}

		//Copies the currently selected notes to the User's clipboard and saves the clipboard string to the palette array.
		int copyToPalette()
		{
			if (gNotes->noneSelected())
			{
				HudWarning("Must select notes to copy");
				return 1;
			}
			gNotes->copyToClipboard(false);
			addToPalette(gSystem->getClipboardText(), myCurrSelection);
			return 0;
		}

		int setCurrentPalette(int index)
		{
			if (index < MAX_PALETTE_ENTRIES) {
				myCurrSelection = index;
			}
			else
			{
				HudWarning("Palette index is greater than maximum permitted size");
				return 1;
			}
			return 0;
		}

		int toggleDestructivePaste()
		{
			myShouldInsertNotes = !myShouldInsertNotes;
			HudInfo("Toggled destructive paste: %i", !myShouldInsertNotes);
			return 0;
		}
	};

	PaletteMan* gPalette = nullptr;

	void PaletteMan::create()
	{
		gPalette = new PaletteManImpl;
	}

	void PaletteMan::destroy()
	{

		delete ((PaletteManImpl*)gPalette);
		gPalette = nullptr;
	}


}; // namespace Vortex
