#include <System/System.h>
#include <Editor/Selection.h>
#include <Managers/PaletteMan.h>
#include <string>
#include <Editor/Editing.h>
#include <Managers/StyleMan.h>
#include <set>
#include <algorithm>
#include <Managers/NoteMan.h>
#include <Core/ByteStream.h>
#include <Core/Utils.h>
#include <Core/StringUtils.h>
#include <Core/VectorUtils.h>
#include <Managers/TempoMan.h>
namespace Vortex {
	//Maximum size of the palette.
	const int MAX_PALETTE_ENTRIES = 20;
	int pal_sizes[MAX_PALETTE_ENTRIES];
	String* userPaletteLabels[MAX_PALETTE_ENTRIES];
	
	//The current entry in the palette that PaletteMan functions perform operations on.
	int CurrSelection;
	int currPalSize;
	bool shouldMirrorNoteH = false;
	bool shouldMirrorNoteV = false;
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
			currPalSize = 0;
			for (int i = 0; i < MAX_PALETTE_ENTRIES; i++)
			{
				my_s[i] = nullptr;
				userPaletteLabels[i] = nullptr;
				pal_sizes[i] = 0;
			}
			int currPalSize = 0;
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

			if (shouldMirrorNoteH && !shouldMirrorNoteV) { gEditing->mirrorNotes((Editing::MIRROR_H)); }
			else if (shouldMirrorNoteV && !shouldMirrorNoteH) { gEditing->mirrorNotes((Editing::MIRROR_V)); }
			else if (shouldMirrorNoteV && shouldMirrorNoteH) { gEditing->mirrorNotes((Editing::MIRROR_HV)); }

			return 0;
		}

		//Copies the currently selected notes to the User's clipboard and saves the clipboard string to the palette array.
		int copyToPalette()
		{
			bool timeBased = false;
			
			if (gNotes->noneSelected())
			{
				HudWarning("Must select notes to copy");
				return 1;
			}
			
			
			//COPIED FROM NOTESMAN
			////////////////////////////////////////////////////////////////
			// Get the note selection.
			NoteList notes;
			int numNotes = gSelection->getSelectedNotes(notes);
			if (notes.empty()) return 1;

			// Encode the note data and send it to the clipboard.
			if (numNotes > 0)
			{
				WriteStream stream;
				stream.write<uchar>(timeBased);
				if (timeBased)
				{
					notes.encode(stream, gTempo->getTimingData(), true);
				}
				else
				{
					notes.encode(stream, true);
				}
				SetClipboardData("notes", stream.data(), stream.size());
				currPalSize = numNotes;
				//QUIET
				//HudInfo("Copied %i notes", numNotes);
			}
			//////////////////////////////////////////////////////////////
			
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

		void setUserLabelData(String* s, int index)
		{
			userPaletteLabels[index] = s;
		}

		String* getUserLabelData()
		{
			return  userPaletteLabels[myCurrSelection];
		}

		void toggleMirrorH()
		{
			shouldMirrorNoteH = !shouldMirrorNoteH;
		}

		void toggleMirrorV()
		{
			shouldMirrorNoteV = !shouldMirrorNoteV;
		}

		//A quiet version of mirrorNotes(), since we don't want to clutter the HUD.
		void quietMirrorNotes(Editing::MirrorType type)
		{
			NoteEdit edit;
			gSelection->getSelectedNotes(edit.add);

			if (edit.add.empty())
			{
				HudNote("There are no notes selected.");
				return;
			}
			edit.rem = edit.add;

			// Mirror the selected notes.
			auto style = gStyle->get();
			switch (type)
			{
			case Editing::MIRROR_H:
				gEditing->switchColumns(edit.add, style->mirrorTableH); break;
			case Editing::MIRROR_V:
				gEditing->switchColumns(edit.add, style->mirrorTableV); break;
			case Editing::MIRROR_HV:
				gEditing->switchColumns(edit.add, style->mirrorTableH);
				gEditing->switchColumns(edit.add, style->mirrorTableV); break;
			};

			// Resort the notes per row.
			auto ptr = edit.add.begin();
			for (int i = 0, size = edit.add.size(); i < size;)
			{
				int row = (ptr + i)->row, begin = i;
				while (i != size && (ptr + i)->row == row) ++i;
				std::sort(ptr + begin, ptr + i, LessThanRowCol<Note, Note>);
			}

			// Perform the mirror operation.
			static const NotesMan::EditDescription descs = {"",""};
			gNotes->modify(edit, false, &descs + type);

			// Reselect the mirrored notes.
			if (gSelection->getType() == Selection::NOTES)
			{
				gNotes->select(SELECT_SET, edit.add.begin(), edit.rem.size());
			}
		}

		void setCurrentPaletteSize(int size)
		{
			pal_sizes[myCurrSelection] = size;
		}
		
		int getCurrentPaletteSize() {
			return pal_sizes[myCurrSelection];
		}

		int getCurrentPaletteIndex()
		{
			return myCurrSelection;
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
