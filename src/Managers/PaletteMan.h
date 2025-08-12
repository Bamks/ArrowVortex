#pragma once
namespace Vortex {
	struct PaletteMan {
		static void create();
		static void destroy();
		virtual int pasteFromPalette() = 0;
		virtual int copyToPalette() = 0;
		virtual String getFromPalette(int index) = 0;
		virtual int addToPalette(String entry, int index) = 0;
		virtual int setCurrentPalette(int index) = 0;
		virtual int toggleDestructivePaste() = 0;
	};
	extern PaletteMan* gPalette;


};