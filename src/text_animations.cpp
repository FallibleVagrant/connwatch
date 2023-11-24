//Is there a better way to do this?
//Looking at the C preprocessor and it doesn't seem possible to do this exactly...
//Maybe just a program to format stuff into this automatically.
//Or maybe a class?

#include "text_animations.h"

const char* sframe1[] = {
	" ------  ",
	"|       |",
	"|       |",
	"|       |",
	"  ------ ",
};
const char* sframe2[] = {
	" ----- - ",
	"|       |",
	"|       |",
	"|       |",
	" - ----- ",
};
const char* sframe3[] = {
	" ---- -- ",
	"|       |",
	"|       |",
	"|       |",
	" -- ---- ",
};
const char* sframe4[] = {
	" --- --- ",
	"|       |",
	"|       |",
	"|       |",
	" --- --- ",
};
const char* sframe5[] = {
	" -- ---- ",
	"|       |",
	"|       |",
	"|       |",
	" ---- -- ",
};
const char* sframe6[] = {
	" - ----- ",
	"|       |",
	"|       |",
	"|       |",
	" ----- - ",
};
const char* sframe7[] = {
	"  ------ ",
	"|       |",
	"|       |",
	"|       |",
	" ------  ",
};
const char* sframe8[] = {
	" ------- ",
	"        |",
	"|       |",
	"|        ",
	" ------- ",
};
const char* sframe9[] = {
	" ------- ",
	"|       |",
	"         ",
	"|       |",
	" ------- ",
};
const char* sframe10[] = {
	" ------- ",
	"|        ",
	"|       |",
	"        |",
	" ------- ",
};

const char** sanimation_frames[] = {
	sframe1,
	sframe2,
	sframe3,
	sframe4,
	sframe5,
	sframe6,
	sframe7,
	sframe8,
	sframe9,
	sframe10,
};

const unsigned int sanimation_lens[] = {
	sizeof(sframe1) / sizeof(sframe1[0]),
	sizeof(sframe2) / sizeof(sframe2[0]),
	sizeof(sframe3) / sizeof(sframe3[0]),
	sizeof(sframe4) / sizeof(sframe4[0]),
	sizeof(sframe5) / sizeof(sframe5[0]),
	sizeof(sframe6) / sizeof(sframe6[0]),
	sizeof(sframe7) / sizeof(sframe7[0]),
	sizeof(sframe8) / sizeof(sframe8[0]),
	sizeof(sframe9) / sizeof(sframe9[0]),
	sizeof(sframe10) / sizeof(sframe10[0]),
};

const int num_sframes = sizeof(sanimation_frames) / sizeof(sanimation_frames[0]);

const char* tframe1[] = {
	"         ",
	"         ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe2[] = {
	"  '     '",
	"         ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe3[] = {
	"  '     '",
	"         ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe4[] = {
	"         ",
	"  '    ' ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe5[] = {
	"         ",
	"  '    ' ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe6[] = {
	"         ",
	"         ",
	" '  O '  ",
	"   /|\\   ",
	"         ",
};
const char* tframe7[] = {
	"         ",
	"         ",
	"'   O '  ",
	"   /|\\   ",
	"         ",
};
const char* tframe8[] = {
	"         ",
	"         ",
	"    O    ",
	"'  /|\\ ' ",
	"         ",
};
const char* tframe9[] = {
	"         ",
	"         ",
	"    O    ",
	"'  /|\\  '",
	"         ",
};
const char* tframe10[] = {
	"         ",
	"         ",
	"    O    ",
	"'  /|\\   ",
	"        '",
};
const char* tframe11[] = {
	"         ",
	"         ",
	"    O    ",
	"   /|\\   ",
	"'       '",
};
const char* tframe12[] = {
	"         ",
	"         ",
	"    O    ",
	"   /|\\   ",
	"'        ",
};
const char* tframe13[] = {
	"         ",
	"         ",
	"    O    ",
	"   /|\\   ",
	"         ",
};
const char* tframe14[] = {
	"         ",
	"         ",
	"    O    ",
	"   /|\\   ",
	"         ",
};

const char** tanimation_frames[] = {
	tframe1,
	tframe2,
	tframe3,
	tframe4,
	tframe5,
	tframe6,
	tframe7,
	tframe8,
	tframe9,
	tframe10,
	tframe11,
	tframe12,
	tframe13,
	tframe14,
};

const unsigned int tanimation_lens[] = {
	sizeof(tframe1) / sizeof(tframe1[0]),
	sizeof(tframe2) / sizeof(tframe2[0]),
	sizeof(tframe3) / sizeof(tframe3[0]),
	sizeof(tframe4) / sizeof(tframe4[0]),
	sizeof(tframe5) / sizeof(tframe5[0]),
	sizeof(tframe6) / sizeof(tframe6[0]),
	sizeof(tframe7) / sizeof(tframe7[0]),
	sizeof(tframe8) / sizeof(tframe8[0]),
	sizeof(tframe9) / sizeof(tframe9[0]),
	sizeof(tframe10) / sizeof(tframe10[0]),
	sizeof(tframe11) / sizeof(tframe11[0]),
	sizeof(tframe12) / sizeof(tframe12[0]),
	sizeof(tframe13) / sizeof(tframe13[0]),
	sizeof(tframe14) / sizeof(tframe14[0]),
};

const int num_tframes = sizeof(tanimation_frames) / sizeof(tanimation_frames[0]);

const char* gframe1[] = {
	"     --  ",
	"|        ",
	"|       |",
	" \\      |",
	"       / ",
};
const char* gframe2[] = {
	"|    --  ",
	"|        ",
	"|        ",
	"        |",
	"      -/ ",
};
const char* gframe3[] = {
	"|     -- ",
	"|        ",
	"|        ",
	"         ",
	"     --/ ",
};
const char* gframe4[] = {
	"      -- ",
	"|       \\",
	"|        ",
	"|        ",
	"    ---/ ",
};
const char* gframe5[] = {
	"       - ",
	"/       \\",
	"|       |",
	"|        ",
	"    ---  ",
};
const char* gframe6[] = {
	" /    -- ",
	"/       \\",
	"|        ",
	"         ",
	"    --   ",
};
const char* gframe7[] = {
	" /=    - ",
	"/       \\",
	"       / ",
	"         ",
	"    --   ",
};
const char* gframe8[] = {
	"-==    - ",
	"        \\",
	"       / ",
	"         ",
	"    -    ",
};
const char* gframe9[] = {
	"--=      ",
	"        \\",
	"       / ",
	"      /  ",
	"         ",
};
const char* gframe10[] = {
	"---      ",
	"        |",
	"       / ",
	"      /  ",
	"         ",
};
const char* gframe11[] = {
	"--       ",
	"        |",
	"  *    / ",
	"      /  ",
	"         ",
};
const char* gframe12[] = {
	"-        ",
	"         ",
	"  0    / ",
	"     -/  ",
	"         ",
};
const char* gframe13[] = {
	"         ",
	"   /     ",
	"  0    / ",
	"     -/  ",
	"         ",
};
const char* gframe14[] = {
	"         ",
	"   /    }",
	"  0    / ",
	"-     /  ",
	"         ",
};
const char* gframe15[] = {
	"    /    ",
	"   /    }",
	"  0    / ",
	"--    }  ",
	"         ",
};
const char* gframe16[] = {
	"    /    ",
	"   /    }",
	"  0    / ",
	"---   }  ",
	"         ",
};
const char* gframe17[] = {
	"    /-   ",
	"   /    }",
	"  0    / ",
	" ---  |  ",
	"         ",
};
const char* gframe18[] = {
	"    /-   ",
	"   /     ",
	"  0    / ",
	"  --- |  ",
	"      |  ",
};
const char* gframe19[] = {
	"    /--  ",
	"         ",
	"  0    / ",
	"   ---|  ",
	"      |  ",
};
const char* gframe20[] = {
	"    /--  ",
	" \\       ",
	"  0    / ",
	"    --Y  ",
	"      |  ",
};
const char* gframe21[] = {
	"\\    --- ",
	" \\       ",
	"  0    / ",
	"     -X  ",
	"      |  ",
};
const char* gframe22[] = {
	"\\   ---  ",
	" \\       ",
	" |0      ",
	"     -X  ",
	"      |  ",
};
const char* gframe23[] = {
	"\\   ---  ",
	" \\       ",
	" /0      ",
	"     -X  ",
	"      |  ",
};
const char* gframe24[] = {
	"\\  ---   ",
	" \\       ",
	" /%      ",
	"      X  ",
	"      |  ",
};
const char* gframe25[] = {
	"   ---   ",
	" \\       ",
	" //   ^  ",
	"/    [#] ",
	"      v  ",
};
const char* gframe26[] = {
	"    --^  ",
	" \\    ^  ",
	" /-   ^  ",
	"<<<<<+++>",
	"      v  ",
};
const char* gframe27[] = {
	"    --^  ",
	" \\    ^  ",
	" /   -+- ",
	"<<<<+++++",
	"     -+- ",
};
const char* gframe28[] = {
	"    --^  ",
	"      +  ",
	" /   -+- ",
	"<<<<+++++",
	"     -+- ",
};
const char* gframe29[] = {
	"    --^  ",
	"      +  ",
	" /   -+- ",
	"<++++- -+",
	"     -+- ",
};
const char* gframe30[] = {
	"    --+  ",
	"      |  ",
	" /    |  ",
	"+----- --",
	"      |  ",
};
const char* gframe31[] = {
	"    --|  ",
	"      |  ",
	" /    |  ",
	"------ --",
	"      |  ",
};
const char* gframe32[] = {
	"    --| /",
	"      |  ",
	"-/    |  ",
	"------ --",
	"      |  ",
};
const char* gframe33[] = {
	"    --' /",
	"      ' |",
	"-}       ",
	"-----   -",
	"         ",
};
const char* gframe34[] = {
	"    --  /",
	"        |",
	"-}       ",
	"--       ",
	"         ",
};
const char* gframe35[] = {
	"    --  |",
	"        |",
	" }      |",
	"{        ",
	"         ",
};
const char* gframe36[] = {
	"    --   ",
	"\\       |",
	" }      |",
	"|       |",
	"         ",
};
const char* gframe37[] = {
	"    --   ",
	"\\        ",
	" }      |",
	" |      |",
	"       / ",
};
const char* gframe38[] = {
	"    --   ",
	"|        ",
	"|       |",
	" \\      |",
	"       / ",
};

const char** ganimation_frames[] = {
	gframe1,
	gframe2,
	gframe3,
	gframe4,
	gframe5,
	gframe6,
	gframe7,
	gframe8,
	gframe9,
	gframe10,
	gframe11,
	gframe12,
	gframe13,
	gframe14,
	gframe15,
	gframe16,
	gframe17,
	gframe18,
	gframe19,
	gframe20,
	gframe21,
	gframe22,
	gframe23,
	gframe24,
	gframe25,
	gframe26,
	gframe27,
	gframe28,
	gframe29,
	gframe30,
	gframe31,
	gframe32,
	gframe33,
	gframe34,
	gframe35,
	gframe36,
	gframe37,
};

const unsigned int ganimation_lens[] = {
	sizeof(gframe1) / sizeof(gframe1[0]),
	sizeof(gframe2) / sizeof(gframe2[0]),
	sizeof(gframe3) / sizeof(gframe3[0]),
	sizeof(gframe4) / sizeof(gframe4[0]),
	sizeof(gframe5) / sizeof(gframe5[0]),
	sizeof(gframe6) / sizeof(gframe6[0]),
	sizeof(gframe7) / sizeof(gframe7[0]),
	sizeof(gframe8) / sizeof(gframe8[0]),
	sizeof(gframe9) / sizeof(gframe9[0]),
	sizeof(gframe10) / sizeof(gframe10[0]),
	sizeof(gframe11) / sizeof(gframe11[0]),
	sizeof(gframe12) / sizeof(gframe12[0]),
	sizeof(gframe13) / sizeof(gframe13[0]),
	sizeof(gframe14) / sizeof(gframe14[0]),
	sizeof(gframe15) / sizeof(gframe15[0]),
	sizeof(gframe16) / sizeof(gframe16[0]),
	sizeof(gframe17) / sizeof(gframe17[0]),
	sizeof(gframe18) / sizeof(gframe18[0]),
	sizeof(gframe19) / sizeof(gframe19[0]),
	sizeof(gframe20) / sizeof(gframe20[0]),
	sizeof(gframe21) / sizeof(gframe21[0]),
	sizeof(gframe22) / sizeof(gframe22[0]),
	sizeof(gframe23) / sizeof(gframe23[0]),
	sizeof(gframe24) / sizeof(gframe24[0]),
	sizeof(gframe25) / sizeof(gframe25[0]),
	sizeof(gframe26) / sizeof(gframe26[0]),
	sizeof(gframe27) / sizeof(gframe27[0]),
	sizeof(gframe28) / sizeof(gframe28[0]),
	sizeof(gframe29) / sizeof(gframe29[0]),
	sizeof(gframe30) / sizeof(gframe30[0]),
	sizeof(gframe31) / sizeof(gframe31[0]),
	sizeof(gframe32) / sizeof(gframe32[0]),
	sizeof(gframe33) / sizeof(gframe33[0]),
	sizeof(gframe34) / sizeof(gframe34[0]),
	sizeof(gframe35) / sizeof(gframe35[0]),
	sizeof(gframe36) / sizeof(gframe36[0]),
	sizeof(gframe37) / sizeof(gframe37[0]),
};

const int num_gframes = sizeof(ganimation_frames) / sizeof(ganimation_frames[0]);

const char*** warn_animations[] = {
	sanimation_frames,
	tanimation_frames,
	ganimation_frames,
};

const unsigned int* warn_animation_lens[] = {
	sanimation_lens,
	tanimation_lens,
	ganimation_lens,
};

const int warn_animation_num_frames[] = {
	num_sframes,
	num_tframes,
	num_gframes,
};

const int num_animations = 3;
