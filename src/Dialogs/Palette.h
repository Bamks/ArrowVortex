#pragma once

#include <Dialogs/Dialog.h>
namespace Vortex {

	class DialogPalette : public EditorDialog
	{
	public:
		~DialogPalette();
		DialogPalette();

	private:
		//Menu Creation
		void myCreateWidgets();
		
		//"Copy To Palette" update function.
		void myCopyToPalette();
		
		//"Paste From Palette" update function.
		void myPasteFromPalette();
		
		//Palette Button update function
		int setCurrSelection(int index);
		
		//"Destructive Paste" checkbox update function.
		void onCheckboxChange();
		
		//LineEdit.onChange() update function.
		int onUserLabelChange();
		
		//"Horizontal Mirror" checkbox update function.
		void onHMirrorChange();
		
		//"Vertical Mirror" checkbox update function.
		void onVMirrorChange();
	};

}; // namespace Vortex
