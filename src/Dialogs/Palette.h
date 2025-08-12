#pragma once

#include <Dialogs/Dialog.h>
namespace Vortex {

	class DialogPalette : public EditorDialog
	{
	public:
		~DialogPalette();
		DialogPalette();

	private:
		void myCreateWidgets();
		void myCopyToPalette();
		void myPasteFromPalette();
		int setCurrSelection(int index);
		void DialogPalette::updateSelectionLabel();
		void onCheckboxChange();
		int myCurrSelection;
	};

}; // namespace Vortex
