#pragma once
#include <Editor/Editing.h>
namespace Vortex {
	struct PaletteMan {
		static void create();
		static void destroy();
		virtual int pasteFromPalette() = 0;
		virtual int copyToPalette() = 0;
		//virtual int setPaletteString(String label);
		virtual String getFromPalette(int index) = 0;
		virtual int addToPalette(String entry, int index) = 0;
		//virtual void updatePaletteLabel() = 0;
		virtual int setCurrentPalette(int index) = 0;
		virtual int toggleDestructivePaste() = 0;
		virtual void setUserLabelData(String* s, int index) = 0;
		virtual String* getUserLabelData() = 0;
		virtual void toggleMirrorH() = 0;
		virtual void toggleMirrorV() = 0;
		virtual int getCurrentPaletteSize() = 0;
		virtual int getCurrentPaletteIndex() = 0;
		virtual void setCurrentPaletteSize(int size) = 0;
	};
	extern PaletteMan* gPalette;


};